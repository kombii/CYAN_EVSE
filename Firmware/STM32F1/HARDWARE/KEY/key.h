#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"


#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)//��ȡ����2 

#define KEY0_PRES 	1	//KEY0����
#define KEY1_PRES	2	//KEY1����
#define KEY2_PRES	3	//KEY2����

u8 KEY_Scan(u8);  	//����ɨ�躯��	
u16 KEY_SELA(u8 clean);
u16 KEY_SELB(u8 clean);
#endif
