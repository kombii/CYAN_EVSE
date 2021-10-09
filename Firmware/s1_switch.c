#include "s1_switch.h"
#include "pwm.h"
#include "config.h"

void S1_Switch_Init()
{
	PWM_config();
	S1_Switch_Off();
	P14 = 1;
}

void S1_Switch_On()
{
	PWMA_ENO3P_ENABLE();
}
void S1_Switch_Off()
{
	PWMA_ENO3P_DISABLE();
	//�����и����⣬���л���˲�䣬ʵ���ϻ��������½��͹ر�pwm���������ʱ����һ���޴��ռ�ձȣ�������ڳ���˵����������
	//�����淶���S1Ҳ��ֱ��Ӳ�л��ģ��л��ڼ�Ҳ���е�ѹ���䣬����Ӧ�ö����������жϣ���ʱ����ôʵ�֡�
	P14 = 1;
}