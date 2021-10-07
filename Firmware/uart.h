#ifndef __UART_H
#define __UART_H	 

#include	"config.h"

#define	UART1	1
#define	UART2	2

#ifdef UART1
#define	COM_TX1_Lenth	128
#define	COM_RX1_Lenth	128
#endif
#ifdef UART2
#define	COM_TX2_Lenth	128
#define	COM_RX2_Lenth	128
#endif

#define	UART_ShiftRight	0		//ͬ����λ���
#define	UART_8bit_BRTx	(1<<6)	//8λ����,�ɱ䲨����
#define	UART_9bit		(2<<6)	//9λ����,�̶�������
#define	UART_9bit_BRTx	(3<<6)	//9λ����,�ɱ䲨����

#define	UART1_SW_P30_P31	0
#define	UART1_SW_P36_P37	(1<<6)
#define	UART1_SW_P16_P17	(2<<6)
#define	UART1_SW_P43_P44	(3<<6)
#define	UART2_SW_P10_P11	0
#define	UART2_SW_P46_P47	1


#define	TimeOutSet1		5
#define	TimeOutSet2		5

#define	BRT_Timer1	1
#define	BRT_Timer2	2

typedef struct
{ 
	u8	id;				//���ں�

	u8	TX_read;		//���Ͷ�ָ��
	u8	TX_write;		//����дָ��
	u8	B_TX_busy;		//æ��־

	u8 	RX_Cnt;			//�����ֽڼ���
	u8	RX_TimeOut;		//���ճ�ʱ
	u8	B_RX_OK;		//���տ����
} COMx_Define; 

typedef struct
{ 
	u8	UART_Mode;			//ģʽ,         UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	u8	UART_BRT_Use;		//ʹ�ò�����,   BRT_Timer1,BRT_Timer2
	u32	UART_BaudRate;		//������,       ENABLE,DISABLE
	u8	Morecommunicate;	//���ͨѶ����, ENABLE,DISABLE
	u8	UART_RxEnable;		//�������,   ENABLE,DISABLE
	u8	BaudRateDouble;		//�����ʼӱ�, ENABLE,DISABLE
	u8	UART_Interrupt;		//�жϿ���,   ENABLE,DISABLE
	u8	UART_Priority;		//���ȼ�,     Priority_0,Priority_1,Priority_2,Priority_3
	u8	UART_P_SW;			//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
} COMx_InitDefine; 

#ifdef UART1
extern	COMx_Define	COM1;
extern	u8	xdata TX1_Buffer[COM_TX1_Lenth];	//���ͻ���
extern	u8 	xdata RX1_Buffer[COM_RX1_Lenth];	//���ջ���
#endif
#ifdef UART2
extern	COMx_Define	COM2;
extern	u8	xdata TX2_Buffer[COM_TX2_Lenth];	//���ͻ���
extern	u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//���ջ���
#endif

u8	UART_Configuration(u8 UARTx, COMx_InitDefine *COMx);
#ifdef UART1
void TX1_write2buff(u8 dat);	//д�뷢�ͻ��壬ָ��+1
u8 WriteData1(u8 *puts, u8 len);
void PrintString1(u8 *puts);
#endif
#ifdef UART2
void TX2_write2buff(u8 dat);	//д�뷢�ͻ��壬ָ��+1
u8 WriteData2(u8 *puts, u8 len);
void PrintString2(u8 *puts);
#endif

void UART_config(void);
//void COMx_write2buff(COMx_Define *COMx, u8 dat);	//д�뷢�ͻ��壬ָ��+1
//void PrintString(COMx_Define *COMx, u8 *puts);

#endif

