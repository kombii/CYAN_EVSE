#include "s1_switch.h"
#include "pwm.h"
#include "config.h"

void s1_switch_init()
{
	PWM_config();
	s1_switch_off();
	P14 = 1;
}

void s1_switch_on()
{
	PWMA_ENO3P_ENABLE();
}
void s1_switch_off()
{
	PWMA_ENO3P_DISABLE();
	P14 = 1;
}