#include "ui.h"
#include "state_mech.h"
#include "IM1281B.h"
#include "BspTime.h"
#include "key.h"
#include "eeprom.h"
#include <string.h>
#include <stdio.h>

volatile uint8_t menu_layer; //菜单等级，0为关闭，1为主界面，2为第一级菜单，3为第二级菜单
volatile uint8_t menu_layer_1; //第一级菜单的位置变量 0为可以调节0.1单位的电流菜单，1为预设电流菜单，2为时间限制页面，3为能量限制页面，4为不做限制页面

volatile uint16_t menu_set_current = 60;
//设置电流，最小6a（60），最大32a（320）
volatile uint16_t menu_select_current = 0;
//选择电流，只有几个档位
//0  1  2   3   4   5
//6，8，12，16，24，32 安
volatile uint16_t menu_set_hour = 0;
//设置小时
volatile uint16_t menu_set_min = 0;
//设置分钟
volatile uint16_t menu_set_energy = 0;
//设置能量限额



autotimer * menutimer = NULL;

//ui硬件部分初始化
void Ui_Hw_Init(void){
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
}

//ui整个系统初始化
void Ui_System_Init(void){
	Ui_Hw_Init();
	menu_layer = 1;//回到主界面
	menu_layer_1 = 1;//第一级菜单为档位电流菜单
	
	menu_set_current = usr_set_current;
	menu_select_current = 0;
	
	menu_set_hour = time_limit/60;
	menu_set_min = time_limit%60;
	
	menu_set_energy = energy_limit;
}

//显示32高字符串,一屏显示两层
//line:显示的目标行，1或者2
//*chr:字符串起始地址 
//mode:0,反色显示;1,正常显示
void OLED_Show32in2line(u8 line,u8 *chr,u8 mode){
	//printf("show 2line in line %d:%s",line,chr);
	u8 len = strlen((char*)chr);
	u8 x = (128-(len*16))/2;
	while(*chr!=0x00)//到0x0为止
	{
		OLED_ShowChar(x,(line-1)*32,*chr,32,mode);
		x+=16;
		chr++;
	}
}

//显示32高字符串,一屏显示1层
//line:显示的目标行，1或者2
//*chr:字符串起始地址 
//mode:0,反色显示;1,正常显示
void OLED_Show32in1line(u8 *chr,u8 mode){
	u8 len = strlen((char*)chr);
	u8 x = (128-(len*16))/2;
	while(*chr!=0x00)//到0x0为止
	{
		OLED_ShowChar(x,16,*chr,32,mode);
		x+=16;
		chr++;
	}
}


//OK按钮的处理过程
void key_ok_handler(void){
	//printf("#按键事件：按钮OK触发\r\n");
	alarm_on=1;
	if(menu_layer == 0){
		//关屏状态
		menu_layer = 1;
	}
	else if(menu_layer == 1){
		//第一屏状态，跳转到一级菜单
		menu_layer = 2;
		if(menutimer != NULL){
			returnTimer(menutimer);
			menutimer = NULL;
		}
	}
	else if(menu_layer == 2){
		//第一级菜单，不同设置的菜单标记
		if(menu_layer_1 == 4){
			//4代表不限制充电限额，设置不限制
			EEPROM_Set_Usr_Limit_Mode(0);
			usr_sel_limit_mode = 0;
			menu_layer = 1;//回到主界面
		}
		else{
			//第一级菜单，跳入第二级
			menu_layer = 3;
		}
	}
	else if(menu_layer == 3){
		//第三级，根据不同菜单保存不同数据
		switch(menu_layer_1){
			case 0:{//0是电流可调
				EEPROM_Set_Current(menu_set_current);
				printf("#菜单事件：用户选择限制电流，电流为%0.1fA\r\n",menu_set_current/10.0);
				break;
			}
			case 1:{//1是选择电流
				switch(menu_select_current){
					//0  1  2   3   4   5
					//6，8，12，16，24，32 安
					case 0:{
						EEPROM_Set_Current(60);
						printf("#菜单事件：用户选择限制电流，电流为6A\r\n");
						break;
					}
					case 1:{
						EEPROM_Set_Current(80);
						printf("#菜单事件：用户选择限制电流，电流为8A\r\n");
						break;
					}
					case 2:{
						EEPROM_Set_Current(120);
						printf("#菜单事件：用户选择限制电流，电流为12A\r\n");
						break;
					}
					case 3:{
						EEPROM_Set_Current(160);
						printf("#菜单事件：用户选择限制电流，电流为16A\r\n");
						break;
					}
					case 4:{
						EEPROM_Set_Current(240);
						printf("#菜单事件：用户选择限制电流，电流为24A\r\n");
						break;
					}
					case 5:{
						EEPROM_Set_Current(320);
						printf("#菜单事件：用户选择限制电流，电流为32A\r\n");
						break;
					}
				}
				break;
			}
			case 2:{//2是时间限制
				EEPROM_Set_Usr_Limit_Mode(1);//1是限制时间
				EEPROM_Set_Time_Limit(menu_set_min + menu_set_hour*60);
				printf("#菜单事件：用户选择限制时间，长度为%dH%dM\r\n",menu_set_hour,menu_set_min);
				break;
			}
			case 3:{//3是能量限制
				EEPROM_Set_Usr_Limit_Mode(2);//2是限制能量
				EEPROM_Set_Energy_Limit(menu_set_energy);
				printf("#菜单事件：用户选择限制能量，能量为%dkWh\r\n",menu_set_energy);
				break;
			}
		}
		Get_Datas_From_EEPROM();
		menu_layer = 1;
	}	
}

//KEYA的处理过程
void key_a_handler(void){
	//printf("#按键事件：旋钮+触发\r\n");
	alarm_on=1;
	if(menu_layer == 0){
		//关屏状态
		menu_layer = 1;
	}
	else if(menu_layer == 2){
		//第二级菜单，切换数字
		if (menu_layer_1 == 4){
			menu_layer_1 = 0;
		} else {
			menu_layer_1 += 1;
		}
	}
	else if(menu_layer == 3){
		switch(menu_layer_1){
			case 0:{//0是电流可调
				if (menu_set_current >= 320){
				} else {
					menu_set_current += 1;
				}
				break;
			}
			case 1:{//1是选择电流
				if (menu_select_current >= 5){
					menu_select_current = 0;
				} else {
					menu_select_current += 1;
				}
				break;
			}
			case 2:{//2是时间限制
				if(menu_set_min >= 55){
					if(menu_set_hour == 99){
						//什么都不做
					}
					else{
						menu_set_min = 0;
						menu_set_hour += 1;
					}
				}
				else{
					menu_set_min += 5;
				}
				break;
			}
			case 3:{//3是能量限制
				if (menu_set_energy >= 5000){
				} else {
					menu_set_energy += 1;
				}
				break;
			}
		}
	}
}

//KEYB的处理过程
void key_b_handler(void){
	//printf("#按键事件：旋钮-触发\r\n");
	alarm_on=1;
	if(menu_layer == 0){
		//关屏状态
		menu_layer = 1;
	}
	else if(menu_layer == 2){
		//第二级菜单，切换数字
		if (menu_layer_1 == 0){
			menu_layer_1 = 4;
		} else {
			menu_layer_1 -= 1;
		}
	}
	else if(menu_layer == 3){
		switch(menu_layer_1){
			case 0:{//0是电流可调
				if (menu_set_current <= 60){
				} else {
					menu_set_current -= 1;
				}
				break;
			}
			case 1:{//1是选择电流
				if (menu_select_current == 0){
					menu_select_current = 5;
				} else {
					menu_select_current -= 1;
				}
				break;
			}
			case 2:{//2是时间限制
				if(menu_set_min == 0){
					if(menu_set_hour == 0){
						//什么都不做
					}
					else{
						menu_set_min = 55;
						menu_set_hour -= 1;
					}
				}
				else{
					menu_set_min -= 5;
				}
				break;
			}
			case 3:{//3是能量限制
				if (menu_set_energy <= 0){
				} else {
					menu_set_energy -= 1;
				}
				break;
			}
		}
	}
}

//扫描按键流程
void MAINKEYTASK(void){
	//长按标志位，代表当前周期已经是长按期间需要处理连按
	static u8 selalongpress = 0;
	static u8 selblongpress = 0;
	u8 key = 0;
	//---------------------------------------------sela长按处理
	if(selalongpress == 1)
	{
		//长按超过1000ms后置位，进入长按处理
		u8 ret = KEY_SELA(0);
		if(ret == 0){
			//按键抬起
			KEY_SELA(1);
			selalongpress = 0;
		}
		else if (ret > 30){
			//按键长按超过100ms
			KEY_SELA(1);
			key_a_handler();
		}

	}
	else if (KEY_SELA(0) > 1000)
	{
		//长按超过1000ms且长按标置位未置位
		KEY_SELA(1);
		selalongpress = 1;
	}
	//---------------------------------------------selb长按处理
	//长按处理
	if(selblongpress == 1)
	{
		//长按超过1000ms后置位，进入长按处理
		u8 ret = KEY_SELB(0);
		if(ret == 0)
		{
			//按键抬起
			KEY_SELB(1);
			selblongpress = 0;
		}
		else if (ret > 30)
		{
			//按键长按超过100ms
			KEY_SELB(1);
			key_b_handler();
		}

	}
	else if (KEY_SELB(0) > 1000)
	{
		KEY_SELB(1);
		selblongpress = 1;
	}
	//---------------------------------------------短按键
	key = KEY_Scan(0);	//得到键值
	if(key)
	{
		switch (key)
		{
		case KEY0_PRES:
			key_a_handler();
			break;
		case KEY1_PRES:
			key_b_handler();
			break;
		case KEY2_PRES:
			key_ok_handler();
			break;
		}
	}
}

//显示空闲状态的信息（电流设置和当前限制模式)(第一屏)
void Show_Idle_State(){
	uint8_t hours = 0;
	uint8_t mins = 0;
	char strings[9] = "";
	memset(strings,0,sizeof(strings));
	sprintf(strings,"%.1f A",usr_set_current/10.0);
	OLED_Show32in2line(1,(u8*)strings,1);
	memset(strings,0,sizeof(strings));
	switch(usr_sel_limit_mode){
		case 0:
			sprintf(strings,"NOLIMIT");	
			break;
		case 1:
			hours = time_limit / 60;
			mins = time_limit % 60;
			sprintf(strings,"%dH %dM", hours, mins);	
			break;
		case 2:
			sprintf(strings,"%.1fkWh",energy_limit/10.0);
			break;
		default:
			sprintf(strings,"LIMITERR");	
			break;
	}
	OLED_Show32in2line(2,(u8*)strings,1);
	//sprintf(strings,"%.1f A",usr_set_current/10.0);
}
//显示菜单页面，根据全局变量的位置来确定
void Show_Menus(void){
	char strings[9] = "";
	switch(menu_layer){
		case 0:{//0代表不显示
			//什么都不干，等按键流程控制跳出
			break;
		}
		case 1:{//1代表主界面，只显示目标数据
			if(menutimer == NULL){
				menutimer = obtainTimer(0);//申请一个计时器
			}
			if(IS_TIMEOUT_MS(menutimer,1000)){
				//10秒后超时
				menu_layer = 0;//进入关屏状态
				OLED_Clear();//清除屏幕
				returnTimer(menutimer);
				menutimer = NULL;
			}
			else{
				Show_Idle_State();
			}
			break;
		}
		case 2:{//2为设置页面，要考虑menu_layer_1的数值
			switch(menu_layer_1){
				case 0:{//0是电流可调
					memset(strings,0,sizeof(strings));
					sprintf(strings,"MAXIMUM");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"CURRENT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 1:{//1是档位电流
					memset(strings,0,sizeof(strings));
					sprintf(strings,"PREFER");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"CURRENT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 2:{//2是时间限制
					memset(strings,0,sizeof(strings));
					sprintf(strings,"TIME");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"LIMIT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 3:{//3是能量限制
					memset(strings,0,sizeof(strings));
					sprintf(strings,"ENERGY");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"LIMIT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 4:{//4是不限制
					memset(strings,0,sizeof(strings));
					sprintf(strings,"NO");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"LIMIT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
			}
			break;
		}
		case 3:{//3为设置的详情，要考虑menu_layer_1的数值
			switch(menu_layer_1){
				case 0:{//0是电流可调
					memset(strings,0,sizeof(strings));
					sprintf(strings,"MAX CUR");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"%.1f A", menu_set_current/10.0);
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 1:{//1是档位电流
					memset(strings,0,sizeof(strings));
					sprintf(strings,"MAX CUR");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					switch(menu_select_current){
						case 0:{
							sprintf(strings,"6.0 A");
							break;
						}
						case 1:{
							sprintf(strings,"8.0 A");
							break;
						}
						case 2:{
							sprintf(strings,"12.0 A");
							break;
						}
						case 3:{
							sprintf(strings,"16.0 A");
							break;
						}
						case 4:{
							sprintf(strings,"24.0 A");
							break;
						}
						case 5:{
							sprintf(strings,"32.0 A");
							break;
						}
					}
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 2:{//2是时间限制
					memset(strings,0,sizeof(strings));
					sprintf(strings,"TIME");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"%02dH %02dM", menu_set_hour, menu_set_min);
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 3:{//3是能量限制
					memset(strings,0,sizeof(strings));
					sprintf(strings,"ENERGY");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"%.1fkWh", menu_set_energy/10.0);
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
			}
			break;
		}
	}
}
//显示充电结果页面
void Show_Summary(void)
{
	char strings[9] = "";
	memset(strings,0,sizeof(strings));
	sprintf(strings,"%.1fkWh",energy_charged*0.0001);
	OLED_Show32in2line(1,(u8*)strings,1);
	memset(strings,0,sizeof(strings));
	sprintf(strings,"%02dH %02dM", time_charged/60, time_charged%60);
	OLED_Show32in2line(2,(u8*)strings,1);
	
}
//插枪后的ui流程
void Ui_Gun(){
	switch(charge_state){
	case charge_state_idle:
		{
			break;
		}
	case charge_state_insert_gun:
		{
			if(chargestart == 1){
				//此时已经进行过跳出该状态，其实就是充电完成后返回插枪状态
				Show_Summary();
			}
			else{
				OLED_Show32in2line(1,(u8*)"WAIT",1);
				OLED_Show32in2line(2,(u8*)"UNLOCK",1);
			}
			break;
		}
	case charge_state_gun_pwm:
		{
			if(stop_reason == 3){
				//此时是车子主动断开的，就不再显示wait
				Show_Summary();
			}
			else{
				OLED_Show32in2line(1,(u8*)"WAIT",1);
				OLED_Show32in2line(2,(u8*)"EV",1);
			}
			break;
		}
	case charge_state_charging:
		{
			break;
		}
	case charge_state_stopping:
		{	
			OLED_Show32in2line(1,(u8*)"STOP",1);
			break;
		}
	case charge_state_nogun_pwm:
		{
			break;
		}
	case charge_state_err:
		{
		    OLED_ShowString(0,0,(u8*)"ERR!",32,1);
			break;
		}

	}
}

void Ui_Task(void){
	OLED_ClearBuffer();
	if (chargegun){
		Ui_Gun();
	} else {
		Show_Menus();
		MAINKEYTASK();
	}
	OLED_Refresh();
}
