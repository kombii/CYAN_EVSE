#ifndef _I2C_h_
#define _I2C_h_
#include "sys.h"

void IIC1_Init(void);
void HWIIC_WriteByte(u8 devaddr,u8 addr,u8 data);
#endif

