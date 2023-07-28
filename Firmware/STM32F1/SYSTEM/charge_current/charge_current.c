#include "charge_current.h"
#include "state_mech.h"
#include "timer.h"

//����Ŀ����������������Ϊ����x10ȡ���������ֵ
//���Ϊ50a����,Ҳ���Ǵ���������500������51a����㹫ʽ��ͬ,�淶��A.2��
//���ݹ淶��A.2��������⵽8-10%��pwm����6A��磬���������������60��6A��
//��������������ϳ���32a�ĵ���
//����ֵΪ32a(7KW) 16a(3.5KW) 8a(1.75KW),���Ϊ6a(1.32KW)
//10%��85%ռ�ձ�֮�䣬i = Dx100x0.6 Ҳ����D = i/60 
//D = 6/60 -> 10% 
//D = 32/60 -> 53.33%
void Set_Charge_Current(uint16_t current){
	if(current>500)current = 500;
	if(current<60)current = 60;
	avaliable_current = current;
	//D = 60/600 -> 10% 
	//D = 320/600 -> 53.33%
	//PWM����趨��PWM2ģʽ��Ҳ���Ǽ�����С��compareʱ�������Ч��ƽ������ƽ��������ЧΪ��
	//Ҳ����С��compareʱ����ͣ�����compareʱ����ߣ�ռ�ձ��Ǹߵ�ƽ���ȣ������Ƿ��ģ�Ҫ��һ��
	TIM_SetCompare1(TIM2,(uint16_t)(7200.0-(7200.0*current/600.0)));
}
