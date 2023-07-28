#include "state_mech.h"
#include "gpio.h"
#include "dlt645.h"
#include "charge_current.h"
#include "BspTime.h"
#include "cp_signal.h"
#include "s1_switch.h"
#include "uart.h"

uint16_t bdata statemech = 0;

sbit emgstate			=statemech^0;
sbit outputen			=statemech^1;
sbit lock				=statemech^2;
sbit output				=statemech^3;

sbit sw_error			=statemech^4;
sbit overtemp			=statemech^5;
sbit overvoltage		=statemech^6;
sbit undervoltage		=statemech^7;

sbit overcurrent		=statemech^8;
sbit emgstop			=statemech^9;
sbit chargegun			=statemech^10;
sbit chargestart		=statemech^11;

sbit S1_Switch_State	=statemech^12;
sbit rfidlock			=statemech^13;
sbit blelock 			=statemech^14;
sbit outvol 			=statemech^15;

uint32_t cp_voltage = 0;//cp线电压，以0.01v单位
uint8_t charge_state = 0;//充电状态
uint8_t cp_state = 0;//cp线档位
uint8_t stop_reason = 0;//停止原因
uint16_t avaliable_current = 0;//当前电流
//1;//过压
//2;//过流
//3;//车主动断开
//4;//充电期间直接拔枪
//5;//应急开关
//6;//过热
//7;//接触器输出错误
//255;//未知

void State_Task_Init(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//33 EMG总输入 
	GPIO_InitStructure.Pin  = GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	//23 输出电压检测 22 21 20 外部输入信号
	GPIO_InitStructure.Pin  = GPIO_Pin_6|GPIO_Pin_5;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//36 温度错误 35 应急停止
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//34 接触器输出
	
	//GPIO_InitStructure.Pin  = GPIO_Pin_0;
	//GPIO_InitStructure.Mode = GPIO_OUT_PP;
	//GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	//20 指示输出
	P34 = 0;
	
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
}

//硬件紧急输入采集
void HW_EMG_Task(void){
	emgstate = !P33; 	//NOR输出，低有效
	emgstop = P35; 		//光偶上拉输出 高有效
	overtemp = P36;		//光偶上拉输出 高有效
}
//输出状态
void Out_Task(void){
	if(	(outputen) 	 && 
		!(emgstate) && 
		!(emgstop) &&
	    !(overtemp)
	) output = 1;
	else output = 0;
	P34 = output;
	outvol = P23;
	PrintString1("^");
	WriteData1((uint8_t *)&statemech,2);
	WriteData1((uint8_t *)&stop_reason,1);
	WriteData1((uint8_t *)&avaliable_current,2);
	PrintString1("&\r\n");
}
//采集电压任务
void Elec_Vol_Task(void){
	//判断电压状态
	if((*(float *)elec_condition.Voltage) > ac_voltage_max)
	{
	overvoltage = 1;
	}
	else {
		overvoltage = 0;
		if ((*(float *)elec_condition.Voltage) < ac_voltage_min && !(((*(float *)elec_condition.Voltage)>=-EPSINON) && ((*(float *)elec_condition.Voltage)<=EPSINON)))
		{
		undervoltage = 1;
		}
		else 
		{
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
		if((*(float *)elec_condition.Current) > (ac_current_size_20*avaliable_current/10))
		{//大于值，申请定时器，开始计时
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(!IS_TIMEOUT_1MS(Overcurrenttimer,5000)){
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
				overcurrent = 1;
			}
		}
		else
		{
			if(Overcurrenttimer != NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			overcurrent = 0;
		}	
	}
	else
	{//20a以下用ac_current_size_0判断 +2a
		if((*(float *)elec_condition.Current) > (ac_current_size_0+(avaliable_current/10L)))
		{//大于值，申请定时器，开始计时
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(!IS_TIMEOUT_1MS(Overcurrenttimer,5000)){
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
//充电任务，负责整个充电管理
void Charge_task(void){
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
					if(!IS_TIMEOUT_1MS(connecttimer,2000))
					{//到2s电压都没有掉下去，是错误状态
						returnTimer(connecttimer);//计时器没用了
						connecttimer = NULL;
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
				PrintString1("#gun insert\r\n");
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
				PrintString1("#gun pull out\r\n");
				S1_Switch_Off();//s1切换到12v
				chargegun = 0;//设置拔枪
				rfidlock = 1;//返回时上锁
				charge_state = charge_state_idle;
			}
			//状态2下,自检如果通过，那么开始输出pwm
			else if(cp_state == 9){
				//lock解锁状态，那么打开pwm，继续运行
				if(lock == 0&&!(emgstate) && chargestart == 0)
				{
					PrintString1("#self test ok\r\n");
					S1_Switch_On();//s1切换到pwm
					charge_state = charge_state_gun_pwm;
					chargestart = 1;//设置已经经历过插枪检测过程，不拔出不允许再次启动
					stop_reason = 0;//重新插枪了，清理停止原因
				}
				else{
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
				PrintString1("#gun pull out(pwm)\r\n");
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
					PrintString1("#ev ok power on\r\n");
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
				PrintString1("#gun pull out(charging)\r\n");
				stop_reason = 4;//充电期间直接拔枪
				chargegun = 0;//设置拔枪
				outputen = 0;//关闭电能输出（可能带载）
				charge_state = charge_state_nogun_pwm;//转向1'
			}
			//状态3'下检测到9v电压则意味着车子自己主动断开
			else if(cp_state == 9){
				stop_reason = 3;//车主动断开
				PrintString1("#ev power out(charging)\r\n");
				outputen = 0;//关闭电能输出（可能带载，取决于车子是否降低电流）
				charge_state = charge_state_gun_pwm;//转向2'
			}
			//状态3'下检测到6v电压则很正常
			else if(cp_state == 6){
				if(overvoltage||overcurrent||emgstate){//达到断电条件
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
					PrintString1("#pripare stop(charging)\r\n");
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
				PrintString1("#ev power out(stopping)\r\n");
				returnTimer(stoppingtimer);//计时器没用了
				stoppingtimer = NULL;
				outputen = 0;//关闭电能输出（可能带载，取决于车子是否降低电流）
				charge_state = charge_state_insert_gun;//转向2
			}
			else if(cp_state == 6){
			//模式3时检测到6v，也就是ev还在降流
				if(!IS_TIMEOUT_1MS(stoppingtimer,3000)){
				//超过3秒
				PrintString1("#power out(stopping force)\r\n");
				returnTimer(stoppingtimer);//计时器没用了
				stoppingtimer = NULL;	
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
				PrintString1("#pwm off(nogun_pwm)\r\n");
				S1_Switch_Off();//s1切换到12v
				rfidlock = 1;//返回时上锁
				charge_state = charge_state_idle;
			}
			//状态1'状态检测到9v情况，意味着又插进来了
			if(cp_state == 9){
				PrintString1("#gun return(nogun_pwm)\r\n");
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
			PrintString1("#smt err\r\n");
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
		if(P22 == 0)
		{
			blelock = 0;//锁定中，解锁有效
			//P20 = 0;
		}
	}
	else{
		if(P21 == 0)
		{
			blelock = 1;//解锁中，上锁有效
			//P20 = 1;
		}
	}
	if(rfidlock)
		if(P20 == 0)
		{
			rfidlock = 0;//锁定中，解锁有效
		}
	lock = !((!blelock)|(!rfidlock));
}

void main_task(void){
	Elec_Vol_Task();//实现电压判断，需要测试
	Elec_Curr_Task();//实现电流判断，需要测试
	Lock_Task();//输入信号判断逻辑
	Get_CP_Vol_Task();//cp电压采集
	HW_EMG_Task();//判断应急输入设备状态
	Charge_task();//充电操作逻辑
	Out_Task();//判断输出逻辑，并且执行输出(和过温，急停有关）
}
