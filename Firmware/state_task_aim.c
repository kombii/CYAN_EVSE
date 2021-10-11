#include "state_mech.h"
#include "gpio.h"
#include "dlt645.h"
#include "charge_current.h"
#include "BspTime.h"
#include <stdio.h>

bit emgstate = 0;//����״̬���� 1���� 0����
bit outputen = 0;//������� 1���� 0������
bit lock = 0;//�ⲿ����ϵͳ 1���� 0����

bit output = 0;//��� 1��� 0�ر����

bit overtemp = 0;//���� 1���� 0����
bit overvoltage = 0;//��ѹ 1���� 0����
bit undervoltage = 0;//Ƿѹ 1���� 0����
bit overcurrent = 0;//���� 1���� 0����
bit emgstop = 0;//����ֹͣ��ť 1���� 0����

uint32_t cp_voltage = 0;//cp�ߵ�ѹ����0.01v��λ
uint8_t charge_state = 0;//���״̬
uint8_t cp_state = 0;//cp�ߵ�λ

void State_Task_Init(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//33 EMG������ 
	GPIO_InitStructure.Pin  = GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	//23 �����ѹ��� 22 21 20 �ⲿ�����ź�
	GPIO_InitStructure.Pin  = GPIO_Pin_6|GPIO_Pin_5;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//36 �¶ȴ��� 35 Ӧ��ֹͣ
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//34 �Ӵ������
	P34 = 0;
}

void HW_EMG_Task(void){
	emgstate = !P33; 	//NOR���������Ч
	emgstop = P35; 		//��ż������� ����Ч
	overtemp = P36;		//��ż������� ����Ч
	if(emgstate == 0 && (emgstop|overtemp))emgstate = 1;//����ڲɼ��ڼ�������⣬����ֱ�Ӵ����
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
	//�жϵ�ѹ״̬
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
	//�жϵ���״̬
	static autotimer *Overcurrenttimer = NULL;
	if(avaliable_current > 20)
	{//20a������ac_current_size_20�жϣ�1.1��
		if(elec_condition.Current > (ac_current_size_20*avaliable_current/10))
		{//����ֵ�����붨ʱ������ʼ��ʱ
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
	{//20a������ac_current_size_0�ж� +2a
		if(elec_condition.Current > (ac_current_size_0+(avaliable_current/10)))
		{//����ֵ�����붨ʱ������ʼ��ʱ
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
			{//�մӹ����ָ�
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
