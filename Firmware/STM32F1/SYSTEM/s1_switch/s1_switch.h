#ifndef __S1_SWITCH_H
#define __S1_SWITCH_H	 
#include "sys.h"

extern volatile uint32_t S1_Switch_State;

void S1_Switch_Init(void);
void S1_Switch_On(void);
void S1_Switch_Off(void);
	
#endif

