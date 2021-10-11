#include "charge_current.h"
#include "pwm.h"

//����Ŀ����������������Ϊ����x10ȡ���������ֵ
//���Ϊ50a����,Ҳ���Ǵ���������500������51a����㹫ʽ��ͬ,�淶��A.2��
//���ݹ淶��A.2��������⵽8-10%��pwm����6A��磬���������������60��6A��
//��������������ϳ���32a�ĵ���
//����ֵΪ32a(7KW) 16a(3.5KW) 8a(1.75KW),���Ϊ6a(1.32KW)
void Set_Charge_Current(uint16_t current){
	if(current>500L)current = 500L;
	if(current<60L)current = 60L;
	avaliable_current = current;
	PWMA_Duty.PWM3_Duty = (MAIN_Fosc/1000L)*(current/6L)/100L;
	UpdatePwm(PWMA,&PWMA_Duty);
}	