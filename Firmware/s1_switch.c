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
	//这里有个问题，在切换的瞬间，实际上会遇到刚下降就关闭pwm的情况，此时会获得一个巨大的占空比，这个对于车来说可能有问题
	//不过规范里的S1也是直接硬切换的，切换期间也会有电压下落，车端应该对这个情况做判断，暂时先这么实现。
	P14 = 1;
}