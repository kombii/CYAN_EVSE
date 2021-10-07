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
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//14 cp��� 

	
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	//02 03 adc�������
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure); //30 31 ����
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure); //10 11 ����
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure); //12 485���ƽ�
}

//��ȡ���ϵ��CP��·����״̬
void Conclude_CP_State(void){
#define v_0v 4500
#define v_6v 7500
#define v_9v 10500
#define v_12v 13500

if(cp_voltage < v_0v)								cp_state = 0;//��ѹ0v(����)��
else if(cp_voltage >= v_0v && cp_voltage < v_6v)	cp_state = 6;//��ѹ6v��
else if(cp_voltage >= v_6v && cp_voltage < v_9v)	cp_state = 9;//��ѹ9v��
else if(cp_voltage >= v_9v && cp_voltage < v_12v)	cp_state = 12;//��ѹ12v��
else												cp_state = 255;//��ѹ255v(����)��

}
//��adc��ȡcp�źŵĵ�ѹ
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
	//�ۼ�10��
	adcval = (adcval*476L)/intnalref1v9;
	
	//�ⲿ5v��ѹ���ڲ�У׼��(119/intnalref1v9)*1024(���㵽0.01v)
	//1218560/intnalref1v9
	//��ѹ�������4������׼��ѹ 121856/intnalref1v9
	//(4*(1218560/intnalref1v9))/1024
	//476/intnalref1v9
	
	adcval += 900L;
	//���������ܳԵ���0.6v�����˶�����Ҳ�Ե���0.6v
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




