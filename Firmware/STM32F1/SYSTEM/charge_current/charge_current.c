#include "charge_current.h"
#include "state_mech.h"
#include "timer.h"

//设置目标充电电流，传入参数为电流x10取整数后的数值
//设计为50a以下,也就是传入参数最大500（超过51a后计算公式不同,规范表A.2）
//根据规范表A.2，车辆检测到8-10%的pwm会以6A充电，所以输入参数下限60（6A）
//大多数车根本不认超过32a的电流
//常用值为32a(7KW) 16a(3.5KW) 8a(1.75KW),最低为6a(1.32KW)
//10%到85%占空比之间，i = Dx100x0.6 也就是D = i/60 
//D = 6/60 -> 10% 
//D = 32/60 -> 53.33%
void Set_Charge_Current(uint16_t current){
	if(current>500)current = 500;
	if(current<60)current = 60;
	avaliable_current = current;
	//D = 60/600 -> 10% 
	//D = 320/600 -> 53.33%
	//PWM输出设定是PWM2模式，也就是计数器小于compare时输出非有效电平，而电平配置是有效为高
	//也就是小于compare时输出低，高于compare时输出高，占空比是高电平长度，所以是反的，要减一次
	TIM_SetCompare1(TIM2,(uint16_t)(7200.0-(7200.0*current/600.0)));
}
