#ifndef __DLT645_2007_H
#define __DLT645_2007_H
#include "TYPESS.H"
#include "sys_queue.h"

#define RX_BUFF_TYPE        QUEUE64_S
extern RX_BUFF_TYPE         cli_rx_buff;

typedef struct {
    uint8_t Voltage[4];
    uint8_t Current[4];
	uint8_t ActivePower[4];
	uint8_t ReactivePower[4];
	uint8_t PowerFactor[4];
	uint8_t Energy[4];
	uint8_t Freq[4];
} ELEC_TYPE_S;

extern ELEC_TYPE_S elec_condition;
extern void Cli_Init(void);
int Dlt645_Read_Data(uint32_t cmdcode);
#endif /* __DLT645_2007_H */
