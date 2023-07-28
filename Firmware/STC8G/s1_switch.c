#include "s1_switch.h"
#include "pwm.h"
#include "config.h"
#include "state_mech.h"

void S1_Switch_Init(void)
{
	PWM_config();
	S1_Switch_Off();
	P14 = 1;
}

void S1_Switch_On(void)
{
	if(S1_Switch_State == 0)
	{
		PWMA_ENO3P_ENABLE();
		S1_Switch_State = 1;
	}
}
void S1_Switch_Off(void)
{
	if(S1_Switch_State == 1)
	{
		PWMA_ENO3P_DISABLE();
		//�����и����⣬���л���˲�䣬ʵ���ϻ��������½��͹ر�pwm���������ʱ����һ���޴��ռ�ձȣ�������ڳ���˵����������
		//�����淶���S1Ҳ��ֱ��Ӳ�л��ģ��л��ڼ�Ҳ���е�ѹ���䣬����Ӧ�ö����������жϣ���ʱ����ôʵ�֡�
		P14 = 1;
		S1_Switch_State = 0;
	}
}