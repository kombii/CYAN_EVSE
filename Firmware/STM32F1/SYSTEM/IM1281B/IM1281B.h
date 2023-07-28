#ifndef __IM1281B_H
#define __IM1281B_H
#include "sys.h"

typedef struct {
    uint32_t Voltage; //Im_Voltage_data
    uint32_t Current; //Im_Current_data
	uint32_t ActivePower; //Im_Power_data
	uint32_t Temperature; //Im_Temp_data
	uint32_t PowerFactor; // Im_Pf_data
	uint32_t Energy; // Im_Energy_data
	uint32_t Freq; //Im_Freq_data
} ELEC_TYPE_S;

extern ELEC_TYPE_S elec_condition;
extern uint8_t IM1281B_TIMEOUT;
extern uint8_t IM1281B_COMMERR;
void IM1281B_Update_Task(void);
#endif
