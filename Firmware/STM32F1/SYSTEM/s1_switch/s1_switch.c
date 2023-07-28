#include "s1_switch.h"
#include "timer.h"
#include "state_mech.h"

void S1_Switch_Init(void)
{
	TIM2_PWM_Init(7199,9);	 //PWM频率=72000000/72000=1Khz
	PwmControl(0);
	S1_Switch_Off();
}

//开启S1，也就是开始输出pwm
void S1_Switch_On(void)
{
	if(S1_Switch_State == 0)
	{
		PwmControl(1);
		S1_Switch_State = 1;
	}
}
//关闭S1，关闭pwm同时电平置为低
void S1_Switch_Off(void)
{
	if(S1_Switch_State == 1)
	{
		PwmControl(0);
		//这里有个问题，在切换的瞬间，实际上会遇到刚下降就关闭pwm的情况，此时会获得一个巨大的占空比，这个对于车来说可能有问题
		//不过规范里的S1也是直接硬切换的，切换期间也会有电压下落，车端应该对这个情况做判断，暂时先这么实现。
		S1_Switch_State = 0;
	}
}

