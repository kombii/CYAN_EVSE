#include "uart.h"
#include "TYPESS.H"
#include "sys_queue.h"
#include "dlt645.h"
#include "gpio.h"

extern RX_BUFF_TYPE         cli_rx_buff;

#ifdef UART1
COMx_Define	COM1;
u8	xdata TX1_Buffer[COM_TX1_Lenth];	//发送缓冲
u8 	xdata RX1_Buffer[COM_RX1_Lenth];	//接收缓冲
#endif
#ifdef UART2
COMx_Define	COM2;
u8	xdata TX2_Buffer[COM_TX2_Lenth];	//发送缓冲
u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//接收缓冲
#endif

char putchar (char c)
{
	ES=0;
	SBUF = c;
	while(TI==0);
	TI=0;
	ES=1;
	return 0;
}

u8 UART_Configuration(u8 UARTx, COMx_InitDefine *COMx)
{
	u8	i;
	u32	j;
	
#ifdef UART1
	if(UARTx == UART1)
	{
		COM1.id = 1;
		COM1.TX_read    = 0;
		COM1.TX_write   = 0;
		COM1.B_TX_busy  = 0;
		COM1.RX_Cnt     = 0;
		COM1.RX_TimeOut = 0;
		COM1.B_RX_OK    = 0;
		for(i=0; i<COM_TX1_Lenth; i++)	TX1_Buffer[i] = 0;
		for(i=0; i<COM_RX1_Lenth; i++)	RX1_Buffer[i] = 0;

		if(COMx->UART_Priority > Priority_3)	return 2;	//错误
		UART1_Priority(COMx->UART_Priority);	//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
		if(COMx->UART_Mode > UART_9bit_BRTx)	return 2;	//模式错误
		SCON = (SCON & 0x3f) | COMx->UART_Mode;
		if((COMx->UART_Mode == UART_9bit_BRTx) || (COMx->UART_Mode == UART_8bit_BRTx))	//可变波特率
		{
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//按1T计算
			if(j >= 65536UL)	return 2;	//错误
			j = 65536UL - j;
			if(COMx->UART_BRT_Use == BRT_Timer1)
			{
				TR1 = 0;
				AUXR &= ~0x01;		//S1 BRT Use Timer1;
				TMOD &= ~(1<<6);	//Timer1 set As Timer
				TMOD &= ~0x30;		//Timer1_16bitAutoReload;
				AUXR |=  (1<<6);	//Timer1 set as 1T mode
				TH1 = (u8)(j>>8);
				TL1 = (u8)j;
				ET1 = 0;	//禁止中断
				TMOD &= ~0x40;	//定时
				INT_CLKO &= ~0x02;	//不输出时钟
				TR1  = 1;
			}
			else if(COMx->UART_BRT_Use == BRT_Timer2)
			{
				AUXR &= ~(1<<4);	//Timer stop
				AUXR |= 0x01;		//S1 BRT Use Timer2;
				AUXR &= ~(1<<3);	//Timer2 set As Timer
				AUXR |=  (1<<2);	//Timer2 set as 1T mode
				TH2 = (u8)(j>>8);
				TL2 = (u8)j;
				IE2  &= ~(1<<2);	//禁止中断
				AUXR |=  (1<<4);	//Timer run enable
			}
			else return 2;	//错误
		}
		else if(COMx->UART_Mode == UART_ShiftRight)
		{
			if(COMx->BaudRateDouble == ENABLE)	AUXR |=  (1<<5);	//固定波特率SysClk/2
			else								AUXR &= ~(1<<5);	//固定波特率SysClk/12
		}
		else if(COMx->UART_Mode == UART_9bit)	//固定波特率SysClk*2^SMOD/64
		{
			if(COMx->BaudRateDouble == ENABLE)	PCON |=  (1<<7);	//固定波特率SysClk/32
			else								PCON &= ~(1<<7);	//固定波特率SysClk/64
		}
		if(COMx->UART_Interrupt == ENABLE)	ES = 1;	//允许中断
		else								ES = 0;	//禁止中断
		if(COMx->UART_RxEnable == ENABLE)	REN = 1;	//允许接收
		else								REN = 0;	//禁止接收
		P_SW1 = (P_SW1 & 0x3f) | (COMx->UART_P_SW & 0xc0);	//切换IO
		return	0;
	}
#endif
#ifdef UART2
	if(UARTx == UART2)
	{
		COM2.id = 2;
		COM2.TX_read    = 0;
		COM2.TX_write   = 0;
		COM2.B_TX_busy  = 0;
		COM2.RX_Cnt     = 0;
		COM2.RX_TimeOut = 0;
		COM2.B_RX_OK    = 0;
		for(i=0; i<COM_TX2_Lenth; i++)	TX2_Buffer[i] = 0;
		for(i=0; i<COM_RX2_Lenth; i++)	RX2_Buffer[i] = 0;

		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//可变波特率
		{
			if(COMx->UART_Priority > Priority_3)	return 2;	//错误
			UART2_Priority(COMx->UART_Priority);	//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
			if(COMx->UART_Mode == UART_9bit_BRTx)	S2CON |=  (1<<7);	//9bit
			else									S2CON &= ~(1<<7);	//8bit
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//按1T计算
			if(j >= 65536UL)	return 2;	//错误
			j = 65536UL - j;
			AUXR &= ~(1<<4);	//Timer stop
			AUXR &= ~(1<<3);	//Timer2 set As Timer
			AUXR |=  (1<<2);	//Timer2 set as 1T mode
			TH2 = (u8)(j>>8);
			TL2 = (u8)j;
			IE2  &= ~(1<<2);	//禁止中断
			AUXR |=  (1<<4);	//Timer run enable
		}
		else	return 2;	//模式错误
		if(COMx->UART_Interrupt == ENABLE)	IE2   |=  1;		//允许中断
		else								IE2   &= ~1;		//禁止中断
		if(COMx->UART_RxEnable == ENABLE)	S2CON |=  (1<<4);	//允许接收
		else								S2CON &= ~(1<<4);	//禁止接收
		P_SW2 = (P_SW2 & ~1) | (COMx->UART_P_SW & 0x01);	//切换IO
		return	0;
	}
#endif
	return	2;	//错误
}

/*********************************************************/

/********************* UART1 函数 ************************/
#ifdef UART1
void TX1_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX1_Buffer[COM1.TX_write] = dat;	//装发送缓冲
	if(++COM1.TX_write >= COM_TX1_Lenth)	COM1.TX_write = 0;

	if(COM1.B_TX_busy == 0)		//空闲
	{  
		COM1.B_TX_busy = 1;		//标志忙
		TI = 1;					//触发发送中断
	}
}

void PrintString1(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX1_write2buff(*puts); 	//遇到停止符0结束
}

u8 WriteData1(u8 *puts, u8 len)
{
	u8 i;
    for (i=0;i<len;i++)  TX1_write2buff(puts[i]);
	return len;
}

void UART1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		if(COM1.B_RX_OK == 0)
		{
			if(COM1.RX_Cnt >= COM_RX1_Lenth)	COM1.RX_Cnt = 0;
			RX1_Buffer[COM1.RX_Cnt++] = SBUF;
			COM1.RX_TimeOut = TimeOutSet1;
		}
	}

	if(TI)
	{
		TI = 0;
		if(COM1.TX_read != COM1.TX_write)
		{
		 	SBUF = TX1_Buffer[COM1.TX_read];
			if(++COM1.TX_read >= COM_TX1_Lenth)		COM1.TX_read = 0;
		}
		else	COM1.B_TX_busy = 0;
	}
}
#endif

/********************* UART2 函数 ************************/
#ifdef UART2
void TX2_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX2_Buffer[COM2.TX_write] = dat;	//装发送缓冲
	if(++COM2.TX_write >= COM_TX2_Lenth)	COM2.TX_write = 0;

	if(COM2.B_TX_busy == 0)		//空闲
	{  
		P12 = 1;
		COM2.B_TX_busy = 1;		//标志忙
		SET_TI2();				//触发发送中断
	}
}

void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX2_write2buff(*puts); 	//遇到停止符0结束
}

u8 WriteData2(u8 *puts, u8 len)
{
	u8 i;
    for (i=0;i<len;i++)  TX2_write2buff(puts[i]);
	return len;
}

void UART2_int (void) interrupt UART2_VECTOR
{	
	uint8_t ucData;
	if(RI2)
	{
		if(COM2.B_RX_OK == 0)
		{		
		CLR_RI2();
		ucData = S2BUF;
		QUEUE_IN(cli_rx_buff, ucData);	
			//if(COM2.RX_Cnt >= COM_RX2_Lenth)	COM2.RX_Cnt = 0;
			//RX2_Buffer[COM2.RX_Cnt++] = S2BUF;
			//COM2.RX_TimeOut = TimeOutSet2;
		}
	}

	if(TI2)
	{
		CLR_TI2();
		if(COM2.TX_read != COM2.TX_write)
		{
		 	S2BUF = TX2_Buffer[COM2.TX_read];
			if(++COM2.TX_read >= COM_TX2_Lenth)		COM2.TX_read = 0;
		}
		else
		{
			COM2.B_TX_busy = 0;
			P12 = 0;
		}
	}
}
#endif

/********************* UART3 函数 ************************/
#ifdef UART3
void TX3_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX3_Buffer[COM3.TX_write] = dat;	//装发送缓冲
	if(++COM3.TX_write >= COM_TX3_Lenth)	COM3.TX_write = 0;

	if(COM3.B_TX_busy == 0)		//空闲
	{  
		COM3.B_TX_busy = 1;		//标志忙
		SET_TI3();				//触发发送中断
	}
}

void PrintString3(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX3_write2buff(*puts); 	//遇到停止符0结束
}

void UART3_int (void) interrupt UART3_VECTOR
{
	if(RI3)
	{
		CLR_RI3();
		if(COM3.B_RX_OK == 0)
		{
			if(COM3.RX_Cnt >= COM_RX3_Lenth)	COM3.RX_Cnt = 0;
			RX3_Buffer[COM3.RX_Cnt++] = S3BUF;
			COM3.RX_TimeOut = TimeOutSet3;
		}
	}

	if(TI3)
	{
		CLR_TI3();
		if(COM3.TX_read != COM3.TX_write)
		{
		 	S3BUF = TX3_Buffer[COM3.TX_read];
			if(++COM3.TX_read >= COM_TX3_Lenth)		COM3.TX_read = 0;
		}
		else	COM3.B_TX_busy = 0;
	}
}
#endif

/********************* UART4 函数 ************************/
#ifdef UART4
void TX4_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX4_Buffer[COM4.TX_write] = dat;	//装发送缓冲
	if(++COM4.TX_write >= COM_TX4_Lenth)	COM4.TX_write = 0;

	if(COM4.B_TX_busy == 0)		//空闲
	{  
		COM4.B_TX_busy = 1;		//标志忙
		SET_TI4();				//触发发送中断
	}
}

void PrintString4(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX4_write2buff(*puts); 	//遇到停止符0结束
}

void UART4_int (void) interrupt UART4_VECTOR
{
	if(RI4)
	{
		CLR_RI4();
		if(COM4.B_RX_OK == 0)
		{
			if(COM4.RX_Cnt >= COM_RX4_Lenth)	COM4.RX_Cnt = 0;
			RX4_Buffer[COM4.RX_Cnt++] = S4BUF;
			COM4.RX_TimeOut = TimeOutSet4;
		}
	}

	if(TI4)
	{
		CLR_TI4();
		if(COM4.TX_read != COM4.TX_write)
		{
		 	S4BUF = TX4_Buffer[COM4.TX_read];
			if(++COM4.TX_read >= COM_TX4_Lenth)		COM4.TX_read = 0;
		}
		else	COM4.B_TX_busy = 0;
	}
}
#endif

/*********************************************************/
/*
void COMx_write2buff(COMx_Define *COMx, u8 dat)	//写入发送缓冲，指针+1
{
	if(COMx->id == 1)	TX1_write2buff(dat);
	if(COMx->id == 2)	TX2_write2buff(dat);
	if(COMx->id == 3)	TX3_write2buff(dat);
	if(COMx->id == 4)	TX4_write2buff(dat);
}

void PrintString(COMx_Define *COMx, u8 *puts)
{
    for (; *puts != 0;	puts++)  COMx_write2buff(COMx,*puts); 	//遇到停止符0结束
}
*/

void UART_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	COMx_InitDefine		COMx_InitStructure;
	
	GPIO_InitStructure.Pin  = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure); //16 17 串口
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure); //10 11 串口
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure); //12 485控制脚
	P12 = 0;
	
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;
	COMx_InitStructure.UART_BaudRate  = 19200ul;
	COMx_InitStructure.UART_RxEnable  = ENABLE;
	COMx_InitStructure.UART_Interrupt = ENABLE;
	COMx_InitStructure.UART_Priority    = Priority_0;
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;
	UART_Configuration(UART2, &COMx_InitStructure);
	
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;
	COMx_InitStructure.UART_BaudRate  = 57600ul;
	COMx_InitStructure.UART_RxEnable  = ENABLE;
	COMx_InitStructure.BaudRateDouble = DISABLE;
	COMx_InitStructure.UART_Interrupt = ENABLE;
	COMx_InitStructure.UART_Priority    = Priority_0;	
	COMx_InitStructure.UART_P_SW      = UART1_SW_P16_P17;	
	UART_Configuration(UART1, &COMx_InitStructure);	
}

