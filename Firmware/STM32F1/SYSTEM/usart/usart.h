#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART1_REC_LEN  			40  		//定义最大接收字节数 200
#define USART2_REC_LEN  			200  		//定义最大接收字节数 200
#define USART3_REC_LEN  			200  		//定义最大接收字节数 200

extern u8  USART1_RX_BUF[USART1_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern volatile u16 USART1_RX_STA;         	//接收状态标记

extern u8  USART2_RX_BUF[USART2_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern volatile u16 USART2_RX_STA;         	//接收状态标记

extern u8  USART3_RX_BUF[USART3_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern volatile u16 USART3_RX_STA;         	//接收状态标记

void PrintString1(char *puts);
u8 WriteData1(u8 *puts, u8 len);
	
void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
#endif


