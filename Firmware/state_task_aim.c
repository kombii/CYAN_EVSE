#include "state_mech.h"
#include "gpio.h"
#include "dlt645.h"
#include "charge_current.h"
#include "BspTime.h"
#include <stdio.h>

bit emgstate = 0;//紧急状态输入 1紧急 0正常
bit outputen = 0;//输出允许 1允许 0不允许
bit lock = 0;//外部锁定系统 1锁定 0解锁

bit output = 0;//输出 1输出 0关闭输出

bit overtemp = 0;//过温 1紧急 0正常
bit overvoltage = 0;//过压 1紧急 0正常
bit undervoltage = 0;//欠压 1紧急 0正常
bit overcurrent = 0;//过流 1紧急 0正常
bit emgstop = 0;//紧急停止按钮 1紧急 0正常

uint32_t cp_voltage = 0;//cp线电压，以0.01v单位
uint8_t charge_state = 0;//充电状态
uint8_t cp_state = 0;//cp线档位

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
	P34 = 0;
}

void HW_EMG_Task(void){
	emgstate = !P33; 	//NOR输出，低有效
	emgstop = P35; 		//光偶上拉输出 高有效
	overtemp = P36;		//光偶上拉输出 高有效
	if(emgstate == 0 && (emgstop|overtemp))emgstate = 1;//如果在采集期间出现问题，可以直接处理掉
}

void Out_Task(void){
	if(	(outputen) 	 && 
		!(emgstate) && 
		!(emgstop) &&
	    !(overtemp)
	) output = 1;
	else output = 0;
	P34 = output;
}

void Elec_Vol_Task(void){
	//判断电压状态
	if(elec_condition.Voltage > ac_voltage_max)
	{
	overvoltage = 1;
	}
	else {
		overvoltage = 0;
		if (elec_condition.Voltage < ac_voltage_min && !((elec_condition.Voltage>=-EPSINON) && (elec_condition.Voltage<=EPSINON)))
		{
		undervoltage = 1;
		}
		else 
		{
		undervoltage = 0;
		}	
	}
}

void Elec_Curr_Task(void){
	//判断电流状态
	static autotimer *Overcurrenttimer = NULL;
	if(avaliable_current > 20)
	{//20a以上用ac_current_size_20判断，1.1倍
		if(elec_condition.Current > (ac_current_size_20*avaliable_current/10))
		{//大于值，申请定时器，开始计时
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
				setTimer(Overcurrenttimer,0);
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
				setTimer(Overcurrenttimer,0);
			}
			overcurrent = 0;
		}	
	}
	else
	{//20a以下用ac_current_size_0判断 +2a
		if(elec_condition.Current > (ac_current_size_0+(avaliable_current/10)))
		{//大于值，申请定时器，开始计时
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
				setTimer(Overcurrenttimer,0);
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

void main_tast(void){
	HW_EMG_Task();
	Out_Task();
//	outputen?printf("1"):printf("0");
//	emgstate?printf("1"):printf("0");
//	emgstop?printf("1"):printf("0");
//	overtemp?printf("1"):printf("0");
//	output?printf("1"):printf("0");
//	printf("\r\n");
	Elec_Vol_Task();
	Elec_Curr_Task();
}
