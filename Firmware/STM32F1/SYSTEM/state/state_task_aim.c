#include "state_mech.h"
#include "charge_current.h"
#include "BspTime.h"
#include "cp_signal.h"
#include "s1_switch.h"
#include "IM1281B.h"
#include "ui.h"
#include <stdio.h>
#include "usart.h"
#include "eeprom.h"
#include "rtc.h"
#include "ds18b20.h"
#include "Ds64RomSearch.h"

#define STATE_ADDR 0X20000100

volatile uint32_t statemech __attribute__((at(STATE_ADDR)));

volatile uint32_t emgstate 			__attribute__((at(BITBAND(STATE_ADDR,0))));
volatile uint32_t outputen 			__attribute__((at(BITBAND(STATE_ADDR,1))));
volatile uint32_t lock 				__attribute__((at(BITBAND(STATE_ADDR,2))));
volatile uint32_t output 			__attribute__((at(BITBAND(STATE_ADDR,3))));

volatile uint32_t sw_error 			__attribute__((at(BITBAND(STATE_ADDR,4))));
volatile uint32_t overtemp 			__attribute__((at(BITBAND(STATE_ADDR,5))));
volatile uint32_t overvoltage 		__attribute__((at(BITBAND(STATE_ADDR,6))));
volatile uint32_t undervoltage 		__attribute__((at(BITBAND(STATE_ADDR,7))));

volatile uint32_t overcurrent 		__attribute__((at(BITBAND(STATE_ADDR,8))));
volatile uint32_t emgstop 			__attribute__((at(BITBAND(STATE_ADDR,9))));
volatile uint32_t chargegun 		__attribute__((at(BITBAND(STATE_ADDR,10))));
volatile uint32_t chargestart 		__attribute__((at(BITBAND(STATE_ADDR,11))));

volatile uint32_t S1_Switch_State 	__attribute__((at(BITBAND(STATE_ADDR,12))));
volatile uint32_t rfidlock 			__attribute__((at(BITBAND(STATE_ADDR,13))));
volatile uint32_t blelock 			__attribute__((at(BITBAND(STATE_ADDR,14))));
volatile uint32_t outvol 			__attribute__((at(BITBAND(STATE_ADDR,15))));

volatile uint32_t meter_err 		__attribute__((at(BITBAND(STATE_ADDR,16))));
volatile uint32_t ble_pre_lock 		__attribute__((at(BITBAND(STATE_ADDR,17))));

volatile uint32_t cp_voltage = 0;//cp线电压，以0.01v单位
volatile uint8_t charge_state = 0;//充电状态
volatile uint8_t cp_state = 0;//cp线档位
volatile uint8_t stop_reason = 0;//停止原因
//1;//过压
//2;//过流
//3;//车主动断开
//4;//充电期间直接拔枪
//5;//应急开关
//6;//过热
//7;//接触器输出错误
//8;//计量出错
//9;//到达限时条件
//10；//到达能量条件
//11; //温度采集出错
//255;//未知
volatile uint16_t avaliable_current = 0;//当前电流(0.1a)

volatile uint16_t usr_set_current = 0;//用户设置电流(0.1a)
volatile uint16_t usr_sel_limit_mode = 0;//用户选择的限制模式
//0；用户选择不限制
//1；用户选择限制时间
//2；用户选择限制能量
volatile uint16_t time_limit = 0;//用户选择的限制时长（单位分钟）
volatile uint16_t energy_limit = 0;//用户选择的限制能量（单位0.1kwh）

volatile uint32_t energy_start_time = 0;//本轮充电开始时表头的能量记录

volatile uint32_t energy_charged = 0;//本轮充电的电量
volatile uint32_t time_charged = 0;//本轮充电的时长

volatile uint32_t alarm_on = 0;//是否启动警告

volatile uint32_t temp_list[3]={0};//温度列表

//从eeprom拿出数据到临时变量里
void Get_Datas_From_EEPROM(){
	usr_sel_limit_mode = EEPROM_Get_Usr_Limit_Mode();
	usr_set_current = EEPROM_Get_Current();
	time_limit = EEPROM_Get_Time_Limit();
	energy_limit = EEPROM_Get_Energy_Limit();
}

void State_Task_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = RELAY_OUT_PIN; //接触器输出
	GPIO_Init(RELAY_OUT_GPIO, &GPIO_InitStructure);
	RELAY_OUT_GPIO_OUT = 0;
	
	GPIO_InitStructure.GPIO_Pin = AC_LED_PIN; //市电led指示灯
	GPIO_Init(AC_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GUN_LED_PIN; //插枪led指示灯
	GPIO_Init(GUN_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = UCK_LED_PIN; //解锁led指示灯
	GPIO_Init(UCK_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CHG_LED_PIN; //充电中led指示灯
	GPIO_Init(CHG_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = EMG_OUT_PIN; //警告led指示灯+蜂鸣器
	GPIO_Init(EMG_OUT_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_InitStructure.GPIO_Pin = EMG_STOP_PIN; //紧急开关输入
	GPIO_Init(EMG_STOP_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BTN_PIN; //按钮输入
	GPIO_Init(BTN_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SELA_PIN; //旋钮左侧输入
	GPIO_Init(SELA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SELB_PIN; //旋钮右侧输入
	GPIO_Init(SELB_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = RFID_UNLOCK_PIN; //rfid锁输入
	GPIO_Init(RFID_UNLOCK_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLE_LOCK_PIN; //蓝牙锁输入
	GPIO_Init(BLE_LOCK_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BLE_UNLOCK_PIN; //蓝牙解锁输入
	GPIO_Init(BLE_UNLOCK_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AC_OUT_IN_PIN; //AC输出感应输入
	GPIO_Init(AC_OUT_IN_GPIO, &GPIO_InitStructure);
	
	//初始化sbit变量
	emgstate = 0;//紧急状态输入 1紧急 0正常
	outputen = 0;//输出允许 1允许 0不允许
	lock = 1;//外部锁定系统 1锁定 0解锁
	output = 0;//输出 1输出 0关闭输出
	sw_error = 0;//软件错误标记 1错误 0正常
	overtemp = 0;//过温 1紧急 0正常
	overvoltage = 0;//过压 1紧急 0正常
	undervoltage = 0;//欠压 1紧急 0正常
	overcurrent = 0;//过流 1紧急 0正常
	emgstop = 0;//紧急停止按钮 1紧急 0正常
	chargegun = 0;//插枪状态 1插入 0拔出
	chargestart = 0;//充电启动过的状态 1启动过了 0没有
	rfidlock = 1;//rfid锁定系统 1锁定 0解锁
	blelock = 1;//ble锁定系统 1锁定 0解锁
	outvol = 0;//输出电压状态
	meter_err = 0;//电能模块工作异常标志 1错误 0正常
}

//硬件紧急输入采集
void HW_EMG_Task(void){
	uint32_t highest_temp = 0;
	u8 i;
	emgstop = EMG_STOP_GPIO_IN; 	//应急开关输入，高有效
	for(i=0;i<3;i++){
		if(temp_list[i]>highest_temp){
			highest_temp = temp_list[i];
		}
	}
	if (highest_temp > temp_threshold_high){
		overtemp = 1;
	} 
	else{
		overtemp = 0;
	}
	emgstate = emgstop || overtemp; //上面两个输入的或关系
	if(emgstate){
		alarm_on=1;
	}
}
//输出状态
void Out_Task(void){
	if(	(outputen)&&!(emgstate)&&!(emgstop)&&!(overtemp)){
		if(output == 0){
			output = 1;
			printf("#功率级：启动输出\r\n");
		}
	}else{
		if(output == 1){
			output = 0;
			printf("#功率级：关闭输出\r\n");
		}
	}
	RELAY_OUT_GPIO_OUT = output;
	outvol = AC_OUT_IN_GPIO_IN;
}
//输出各种指示灯
void LED_Task(void){
	static uint8_t is_alarm_on = 0;
	static autotimer * alarmtimer = NULL;
	if (alarmtimer == NULL)alarmtimer = obtainTimer(0);
	
	AC_LED_GPIO_OUT	 = !(overvoltage||undervoltage);
	GUN_LED_GPIO_OUT = chargegun;
	UCK_LED_GPIO_OUT = lock;
	CHG_LED_GPIO_OUT = output;
	if(alarm_on==1){
		is_alarm_on = 1;
		alarm_on = 0;
		EMG_OUT_GPIO_OUT = 1;
		setTimer(alarmtimer,0);
	}
	if(is_alarm_on){
		if(IS_TIMEOUT_MS(alarmtimer,10)){
			EMG_OUT_GPIO_OUT = 0;
			is_alarm_on = 0;
		}
	}
}
//采集电压任务
void Elec_Vol_Task(void){
	//判断电压状态
	if((elec_condition.Voltage*0.0001) > ac_voltage_max){
		overvoltage = 1;
	}else{
		overvoltage = 0;
		if ((elec_condition.Voltage*0.0001) < ac_voltage_min && !(((elec_condition.Voltage*0.0001)>=-EPSINON) && ((elec_condition.Voltage*0.0001)<=EPSINON))){
			undervoltage = 1;
		}else {
			undervoltage = 0;
		}	
	}
}

//采集电流任务
void Elec_Curr_Task(void){
	//判断电流状态
	static autotimer *Overcurrenttimer = NULL;
	if(avaliable_current > 200)
	{//20a以上用ac_current_size_20判断，1.1倍
		if((elec_condition.Current*0.0001) > (ac_current_size_20*avaliable_current/10))
		{//大于值，申请定时器，开始计时
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(IS_TIMEOUT_MS(Overcurrenttimer,5000)){
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
				overcurrent = 1;
			}
		}
		else
		{
			if(Overcurrenttimer != NULL)
			{//刚从过流恢复
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
			}
			overcurrent = 0;
		}	
	}
	else
	{//20a以下用ac_current_size_0判断 +2a
		if((elec_condition.Current*0.0001) > (ac_current_size_0+(avaliable_current/10L)))
		{//大于值，申请定时器，开始计时
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(IS_TIMEOUT_MS(Overcurrenttimer,5000)){
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
				overcurrent = 1;
			}
		}
		else
		{
			if(Overcurrenttimer != NULL)
			{//刚从过流恢复
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
			}
			overcurrent = 0;
		}	
	}
}
//充电总结，会更新energy_charged和time_charged
void Charge_Summary(void){
	//总结本次充电
	energy_charged = elec_condition.Energy - energy_start_time;
	time_charged = RTC_GetCounter()/60;
	
	printf("#充电事件：断电时，充入电能量%01.1fKWh\r\n",energy_charged*0.0001);
	printf("#充电事件：断电时，充电时长为%u分钟\r\n",time_charged);
}
//充电任务，负责整个充电管理
void Charge_Task(void){
	static autotimer *stoppingtimer = NULL;//停止计时器
	static autotimer *connecttimer = NULL;//接触器粘连计时器
	switch(charge_state){
	case charge_state_idle:
		{
			if(cp_state == 12){
				//状态1下检测到12v电压则挺正常的，啥都不用干
				S1_Switch_Off();//s1切换到12v
				chargestart = 0;//设置未启动状态
				if(outvol == 1&& connecttimer == NULL){
					connecttimer = obtainTimer(0);//取一个定时器，设置时间0
				}
				else if(outvol == 1 && connecttimer != NULL){
					if(IS_TIMEOUT_MS(connecttimer,2000))
					{//到2s电压都没有掉下去，是错误状态
						returnTimer(connecttimer);//计时器没用了
						connecttimer = NULL;
						printf("#安全事件：检测到接触器输出无法断开\r\n");
						charge_state = charge_state_err;//进入0
					}
				}
				else if(outvol == 0 && connecttimer == NULL){
					//0是正常状态，并且无需计时器
				}
				else if(outvol == 0 && connecttimer != NULL){
					returnTimer(connecttimer);//计时器没用了
					connecttimer = NULL;
				}
			}
			else if(cp_state == 9){
				//状态1下检测到9v电压则意味着插枪
				printf("#充电事件：检测到插枪\r\n");
				if (connecttimer != NULL){
					returnTimer(connecttimer);//计时器没用了
					connecttimer = NULL;
				}
				chargegun = 1;//设置插枪
				charge_state = charge_state_insert_gun;
			}
			else if(cp_state == 6){
			}
			else{
				//charge_state = charge_state_err;//进入0
			}
			break;
		}
	case charge_state_insert_gun:
		{
			//状态2下检测到12v电压则意味着拔枪
			if(cp_state == 12){
				printf("#充电事件：检测到拔枪\r\n");
				S1_Switch_Off();//s1切换到12v
				chargegun = 0;//设置拔枪
				rfidlock = 1;//返回时上锁
				if(ble_pre_lock){
					//蓝牙锁预位上锁
					ble_pre_lock = 0;
					blelock = 1;
				}
				charge_state = charge_state_idle;
			}
			//状态2下,自检如果通过，那么开始输出pwm
			else if(cp_state == 9){
				//lock解锁状态，那么打开pwm，继续运行
				if(lock == 0 && !(emgstate) && chargestart == 0 && !(meter_err))
				{
					printf("#充电事件：自检正常，准备启动充电\r\n");
					printf("#充电事件：用户启动时选择电流为%01.1fA\r\n",avaliable_current/10.0);
					switch(usr_sel_limit_mode){
						case 0://0；用户选择不限制
						{
							printf("#充电事件：用户启动时设置不做充电停止限制\r\n");	
							break;
						}
						case 1://1；用户选择限制时间
						{
							printf("#充电事件：用户启动时设置充电时间长度为%dH%dM\r\n",time_limit/60,time_limit%60);	
							break;
						}
						case 2://2；用户选择限制能量
						{
							printf("#充电事件：用户启动时设置限制能量为%dkWh\r\n",energy_limit);	
							break;
						}
					}		
					S1_Switch_On();//s1切换到pwm
					charge_state = charge_state_gun_pwm;
					chargestart = 1;//设置已经经历过插枪检测过程，不拔出不允许再次启动
					stop_reason = 0;//重新插枪了，清理停止原因
				}
				else{
					if(chargestart == 1){
						//这是启用pwm之后，又一次回到这里，只有可能是充电完成，执行断开之后的情况	
						
					}
					//什么都不做，让车子等
				}
			}
			else if(cp_state == 6){
				//状态2下,如果稳定6v,存在问题,需要处理
				//charge_state = charge_state_err;//进入0
			}
			else{
				//charge_state = charge_state_err;//进入0
			}
			break;
		}
	case charge_state_gun_pwm:
		{
			//状态2'下检测到12v电压则意味着拔枪
			if(cp_state == 12){
				printf("#充电事件：检测到在PWM输出期间拔枪\r\n");
				chargegun = 0;//设置拔枪
				charge_state = charge_state_nogun_pwm;
			}
			else if(cp_state == 9){
				//状态2'下检测到9v电压则意味着一切正常
			}
			//状态2'下检测到6v电压则意味着ev准备好
			else if(cp_state == 6){
				//lock解锁状态，那么打开pwm，继续运行
				if(lock == 0&&!(emgstate))
				{
					printf("#充电事件：受电设备自检通过，请求输出，启动输出\r\n");
					energy_start_time = elec_condition.Energy;//存下当前的电能量
					printf("#充电事件：准备启动充电时，系统记录电能量%01.1fKWh\r\n",elec_condition.Energy*0.0001);
					RTC_Set(0);//设置rtc时间为0
					printf("#充电事件：准备启动充电，时间重置\r\n");
					
					outputen = 1;//启用电能输出
					charge_state = charge_state_charging;
				}
				else{
					//什么都不做，让车子等
				}
			}
			else
			{
				//charge_state = charge_state_err;//进入0
			}
			break;
		}
	case charge_state_charging:
		{
			//状态3'下检测到12v电压则意味着拔枪
			if(cp_state == 12){
				printf("#充电事件：检测到在充电期间拔枪（警告）\r\n");
				stop_reason = 4;//充电期间直接拔枪
				chargegun = 0;//设置拔枪
				outputen = 0;//关闭电能输出（可能带载）
				charge_state = charge_state_nogun_pwm;//转向1'
			}
			//状态3'下检测到9v电压则意味着车子自己主动断开
			else if(cp_state == 9){
				stop_reason = 3;//车主动断开
				printf("#充电事件：检测到在充电期间，受电端请求断电\r\n");
				outputen = 0;//关闭电能输出（可能带载，取决于车子是否降低电流）
				
				//总结本次充电
				Charge_Summary();
				
				charge_state = charge_state_gun_pwm;//转向2'
			}
			//状态3'下检测到6v电压则很正常
			else if(cp_state == 6){
				//开始处理用户选择的断电条件
				u8 user_stop = 0;
				switch(usr_sel_limit_mode){
					case 0://0；用户选择不限制
					{	
						//不限制的情况下，充电期间循环不做任何操作
						break;
					}
					case 1://1；用户选择限制时间
					{
						if (RTC_GetCounter()/60 >= time_limit){
							printf("#充电事件：检测到在充电期间，达到用户限制的时间\r\n");
							stop_reason = 9;//到达限时条件
							user_stop = 1;
						}
						break;
					}
					case 2://2；用户选择限制能量
					{
						if (((elec_condition.Energy - energy_start_time)*0.0001 >= energy_limit*0.1)){
							printf("#充电事件：检测到在充电期间，达到用户限制的能量\r\n");
							stop_reason = 10;//到达能量条件
							user_stop = 1;
						}
						break;
					}
				}

				//开始处理断电条件
				if(overvoltage||overcurrent||emgstate||user_stop||meter_err){//达到断电条件
					if(stoppingtimer == NULL)
					{
						stoppingtimer = obtainTimer(0);//取一个定时器，设置时间0
					}
					else
					{
						setTimer(stoppingtimer,0);//定时器，设置时间0
					}
					if(overvoltage)stop_reason = 1;//过压
					if(overcurrent)stop_reason = 2;//过流
					if(emgstate&&emgstop)stop_reason = 5;//应急开关
					if(emgstate&&overtemp)stop_reason = 6;//过热
					if(meter_err)stop_reason = 8;//计量出错
					printf("#充电事件：到达断电条件，进入有序断电\r\n");
					
					//总结本次充电
					Charge_Summary();
					
					S1_Switch_Off();//s1切换到12v
					charge_state = charge_state_stopping;//转向3
				}
			}
			else
			{
				//charge_state = charge_state_err;//进入0
			}
			break;
		}
	case charge_state_stopping:
		{	
			if(cp_state == 12){
			}
			//模式3时检测到9v，也就是ev断开准备好
			else if(cp_state == 9){
				printf("#充电事件：有序断电期间，受电端请求断电确认\r\n");
				if(stoppingtimer != NULL){
					returnTimer(stoppingtimer);//计时器没用了
					stoppingtimer = NULL;
				}
				outputen = 0;//关闭电能输出（可能带载，取决于车子是否降低电流）
				charge_state = charge_state_insert_gun;//转向2
			}
			else if(cp_state == 6){
			//模式3时检测到6v，也就是ev还在降流
				if(IS_TIMEOUT_MS(stoppingtimer,3000)){
					//超过3秒
					printf("#充电事件：有序断电过程中受电端超时响应，将会带载强制断开\r\n");
					if(stoppingtimer != NULL){
						returnTimer(stoppingtimer);//计时器没用了
						stoppingtimer = NULL;
					}
					outputen = 0;//关闭电能输出（很可能带载，取决于车子是否降低电流）
					charge_state = charge_state_insert_gun;//转向2
				}
			}
			else
			{
				//charge_state = charge_state_err;//进入0
			}
			break;
		}
	case charge_state_nogun_pwm:
		{
			//状态1'状态检测到12v情况，意味着枪确认断开了，关闭pwm
			if(cp_state == 12){
				printf("#充电事件：无枪pwm模式检测到拔枪\r\n");
				S1_Switch_Off();//s1切换到12v
				rfidlock = 1;//返回时上锁
				charge_state = charge_state_idle;
			}
			//状态1'状态检测到9v情况，意味着又插进来了
			if(cp_state == 9){
				printf("#充电事件：无枪pwm模式检测到插枪\r\n");
				charge_state = charge_state_gun_pwm;
			}
			if(cp_state == 6){
			}	
			else
			{
				//charge_state = charge_state_err;//进入0
			}
			break;
		}
	case charge_state_err:
		{
			printf("#充电事件：状态0触发，存在异常！\r\n");
			rfidlock = 1;//上锁
			S1_Switch_Off();//s1切换到12v
			outputen = 0;//关闭电能输出（可能带载，取决于车子是否降低电流）
		}

	}
	
}

//输入锁定判断逻辑
void Lock_Task(void){
	//in1 22 解锁
	//in2 21 锁定
	//in3 20 暂时不用
	if(blelock){
		if(BLE_UNLOCK_GPIO_IN){blelock = 0;}//锁定中，解锁有效
	}
	else{
		if(BLE_LOCK_GPIO_IN){
			if(chargegun){
				//充电枪插入后禁止上锁
				printf("#鉴权事件：插枪期间上锁，锁定预位，等待充电枪退出\r\n");
				ble_pre_lock = 1;//上锁预位
			}
			else{
				blelock = 1;//解锁中（且未插枪），上锁有效
			}
			
		}
	}
	
	if(rfidlock)
		if(RFID_UNLOCK_GPIO_IN){rfidlock = 0;}//锁定中，解锁有效
		
	if(lock != !((!blelock)|(!rfidlock)))
	{
		lock = !((!blelock)|(!rfidlock));
		lock?printf("#鉴权事件：系统上锁\r\n"):printf("#鉴权事件：系统解锁\r\n");
	}
}

void Update_Charge_Current_Task(void){
	Set_Charge_Current(usr_set_current);
}

//向监控串口传递状态量
void Print_State_Task(void){
	PrintString1("^");
	WriteData1((uint8_t *)&statemech,4);
	WriteData1((uint8_t *)&stop_reason,1);
	WriteData1((uint8_t *)&avaliable_current,2);
	PrintString1("&\r\n");
//	PrintString1("$");
//	WriteData1((uint8_t *)&(elec_condition.Voltage),4);
//	WriteData1((uint8_t *)&(elec_condition.Current),4);
//	WriteData1((uint8_t *)&(elec_condition.ActivePower),4);
//	WriteData1((uint8_t *)&(elec_condition.Energy),4);
//	PrintString1("&\r\n");
}

void Get_Temp_Task(void){
	static autotimer * temptimer = NULL;
	static u8 step = 0;
	static u8 errtimes = 0;
	u8 i;
	u32 temp;
	if (temptimer == NULL)temptimer = obtainTimer(0);
	switch(step){
		case 0:{
			//0代表需要启动采集
			for(i=0;i<3;i++){
				DS18B20_Start(DsGuid[i].guid.u64Bits);
			}
			setTimer(temptimer,0);
			step++;
			break;
		}
		case 1:{
			if(IS_TIMEOUT_MS(temptimer,1000)){
				//时间到了
				for(i=0;i<3;i++){
					temp = DS18B20_Get_Temp_After_Start(DsGuid[i].guid.u64Bits);
					if (temp == 0 || temp == 850){
						//特殊情况，不更新数据
						printf("#测温系统：测试数据有误，跳过本次测试\r\n");
						errtimes++;
					}
					else{
						temp_list[i] = temp;
						//printf("#测温系统：测试数据%d\r\n",temp);
					}
					if(errtimes >= 3){
						errtimes = 0;
						temp_list[0] = 10240;
					}
				}
				step = 0;
			}
			break;
		}
	}
}

void main_task(void){
	static uint32_t mainloopcounter = 0;
	static autotimer * mainlooptimer = NULL;
	if (mainlooptimer == NULL)mainlooptimer = obtainTimer(0);
	
	IM1281B_Update_Task();						//更新表头数据
	Elec_Vol_Task();							//实现电压判断，需要测试
	Elec_Curr_Task();							//实现电流判断，需要测试
	Get_Temp_Task();							//测温流程
	Lock_Task();								//输入信号判断逻辑
	Get_CP_Vol_Task();							//cp电压采集
	HW_EMG_Task();								//判断应急输入设备状态
	Charge_Task();								//充电操作逻辑
	Update_Charge_Current_Task();				//更新用户设置电流值
	Out_Task();									//判断输出逻辑，并且执行输出(和过温，急停有关）
	//Print_State_Task();//输出状态量
	Ui_Task();
	LED_Task();
	//delay_ms(20);
	
	if(S1_Switch_State && meter_err){
		S1_Switch_Off();//s1切换到12v
		stop_reason = 8;//计量出错
		charge_state = charge_state_stopping;//转向3
		printf("METER ERR!\r\n");
	}
	mainloopcounter++;
	if(IS_TIMEOUT_MS(mainlooptimer,60000)){
		printf("#主控系统：内核运行频率：%d hz\r\n",mainloopcounter/60);
		mainloopcounter = 0;
		setTimer(mainlooptimer,0);
	}
}
