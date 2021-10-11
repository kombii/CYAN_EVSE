#include "charge_current.h"
#include "pwm.h"

//设置目标充电电流，传入参数为电流x10取整数后的数值
//设计为50a以下,也就是传入参数最大500（超过51a后计算公式不同,规范表A.2）
//根据规范表A.2，车辆检测到8-10%的pwm会以6A充电，所以输入参数下限60（6A）
//大多数车根本不认超过32a的电流
//常用值为32a(7KW) 16a(3.5KW) 8a(1.75KW),最低为6a(1.32KW)
void Set_Charge_Current(uint16_t current){
	if(current>500L)current = 500L;
	if(current<60L)current = 60L;
	avaliable_current = current;
	PWMA_Duty.PWM3_Duty = (MAIN_Fosc/1000L)*(current/6L)/100L;
	UpdatePwm(PWMA,&PWMA_Duty);
}	