#include "s1_switch.h"
#include "timer.h"
#include "state_mech.h"

void S1_Switch_Init(void)
{
	TIM2_PWM_Init(7199,9);	 //PWMƵ��=72000000/72000=1Khz
	PwmControl(0);
	S1_Switch_Off();
}

//����S1��Ҳ���ǿ�ʼ���pwm
void S1_Switch_On(void)
{
	if(S1_Switch_State == 0)
	{
		PwmControl(1);
		S1_Switch_State = 1;
	}
}
//�ر�S1���ر�pwmͬʱ��ƽ��Ϊ��
void S1_Switch_Off(void)
{
	if(S1_Switch_State == 1)
	{
		PwmControl(0);
		//�����и����⣬���л���˲�䣬ʵ���ϻ��������½��͹ر�pwm���������ʱ����һ���޴��ռ�ձȣ�������ڳ���˵����������
		//�����淶���S1Ҳ��ֱ��Ӳ�л��ģ��л��ڼ�Ҳ���е�ѹ���䣬����Ӧ�ö����������жϣ���ʱ����ôʵ�֡�
		S1_Switch_State = 0;
	}
}

