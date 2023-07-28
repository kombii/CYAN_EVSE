#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART1_REC_LEN  			40  		//�����������ֽ��� 200
#define USART2_REC_LEN  			200  		//�����������ֽ��� 200
#define USART3_REC_LEN  			200  		//�����������ֽ��� 200

extern u8  USART1_RX_BUF[USART1_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern volatile u16 USART1_RX_STA;         	//����״̬���

extern u8  USART2_RX_BUF[USART2_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern volatile u16 USART2_RX_STA;         	//����״̬���

extern u8  USART3_RX_BUF[USART3_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern volatile u16 USART3_RX_STA;         	//����״̬���

void PrintString1(char *puts);
u8 WriteData1(u8 *puts, u8 len);
	
void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
#endif


