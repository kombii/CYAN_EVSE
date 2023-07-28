#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include <stdbool.h>
void TIM2_PWM_Init(u16 arr,u16 psc);
void PwmControl(bool bCon);
#endif
