#include "config.h"
#include "gpio.h"
#include "delay.h"
#include "timer.h"
#include "pwm.h"
#include "uart.h"
#include "dlt645.h"
#include "BspTime.h"
#include "adc.h"
#include "state_mech.h"
#include <stdio.h>

void GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//14 cp输出 

	
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	//02 03 adc输入高阻
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure); //30 31 串口
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure); //10 11 串口
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure); //12 485控制脚
}

//获取充电系的CP线路工作状态
void Conclude_CP_State(void){
#define v_0v 4500
#define v_6v 7500
#define v_9v 10500
#define v_12v 13500

if(cp_voltage < v_0v)								cp_state = 0;//电压0v(超低)档
else if(cp_voltage >= v_0v && cp_voltage < v_6v)	cp_state = 6;//电压6v档
else if(cp_voltage >= v_6v && cp_voltage < v_9v)	cp_state = 9;//电压9v档
else if(cp_voltage >= v_9v && cp_voltage < v_12v)	cp_state = 12;//电压12v档
else												cp_state = 255;//电压255v(超高)档

}
//从adc获取cp信号的电压
void get_cp_vol_task(void)
{
	uint32_t adcval = 0;
	uint32_t intnalref1v9=0;
	uint8_t i;
	
	for(i=0;i<3;i++){
		adcval += Get_ADCResult(15);
	}
	intnalref1v9 = adcval /= 3;
	adcval = 0;
	for(i=0;i<10;i++){
		adcval += Get_ADCResult(10);
	}
	//累计10次
	adcval = (adcval*476L)/intnalref1v9;
	
	//外部5v电压用内部校准：(119/intnalref1v9)*1024(换算到0.01v)
	//1218560/intnalref1v9
	//分压电阻比例4倍，基准电压 121856/intnalref1v9
	//(4*(1218560/intnalref1v9))/1024
	//476/intnalref1v9
	
	adcval += 900L;
	//整流二极管吃掉了0.6v，车端二极管也吃掉了0.6v
	cp_voltage = adcval;
	Conclude_CP_State();
	printf("sw vol:%lu\r\n",cp_voltage);
	printf("cp state:%bu\r\n",cp_state);
}

void main(void)
{
	GPIO_config();
	PWMA_Duty.PWM3_Duty = MAIN_Fosc/1000/2;
	Timer_config();
	PWM_config();
	UART_config();
	ADC_config();
	cli_init();
	EA = 1;
	P12 = 0;
	
	while(1)
	{
		
		if(dlt645_2007_read_data(0x2010100) == 0){
			printf("vol:%f\r\n",*(float *)elec_condition.Voltage);
		}
		else{
			printf("volerr\r\n");
		}	
		get_cp_vol_task();
		
		//cli_run();
		//PWMA_BrakeOutputEnable();
		//delay_ms(100);
		//PWMA_BrakeOutputDisable();
		//delay_ms(100);
	}
}




