#include	"timer.h"
#include    "BspTime.h"

void timer0_int (void) interrupt TIMER0_VECTOR
{
	tickingTimer();
}
void timer1_int (void) interrupt TIMER1_VECTOR
{
}
void timer2_int (void) interrupt TIMER2_VECTOR
{
}
void timer3_int (void) interrupt TIMER3_VECTOR
{
}
void timer4_int (void) interrupt TIMER4_VECTOR
{
}


//========================================================================
// 函数: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	if(TIM > Timer4)	return 1;	//空操作

	if(TIM == Timer0)
	{
		Timer0_Stop();		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer0_InterruptEnable();	//允许中断
		else		Timer0_InterruptDisable();	//禁止中断
		if(TIMx->TIM_Priority > Priority_3)	return 2;	//错误
		Timer0_Priority(TIMx->TIM_Priority);	//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer0_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer0_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer0_AsCounter();	//对外计数或分频
		else		Timer0_AsTimer();	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer0_CLKO_Enable();	//输出时钟
		else		Timer0_CLKO_Disable();	//不输出时钟
		
		T0_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer0_Run();	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer1)
	{
		Timer1_Stop();		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer1_InterruptEnable();	//允许中断
		else		Timer1_InterruptDisable();	//禁止中断
		if(TIMx->TIM_Priority > Priority_3)	return 2;	//错误
		Timer1_Priority(TIMx->TIM_Priority);	//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer1_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer1_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer1_AsCounter();	//对外计数或分频
		else		Timer1_AsTimer();	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer1_CLKO_Enable();	//输出时钟
		else		Timer1_CLKO_Disable();	//不输出时钟
		
		T1_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer1_Run();	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer2)		//Timer2,固定为16位自动重装, 中断无优先级
	{
		Timer2_Stop();	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer2_InterruptEnable();	//允许中断
		else		Timer2_InterruptDisable();	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer2_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer2_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer2_AsCounter();	//对外计数或分频
		else		Timer2_AsTimer();		//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer2_CLKO_Enable();		//输出时钟
		else		Timer2_CLKO_Disable();	//不输出时钟

		T2_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer2_Run();	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer3)		//Timer3,固定为16位自动重装, 中断无优先级
	{
		Timer3_Stop();	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer3_InterruptEnable();	//允许中断
		else		Timer3_InterruptDisable();	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer3_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer3_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer3_AsCounter();	//对外计数或分频
		else		Timer3_AsTimer();		//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer3_CLKO_Enable();		//输出时钟
		else		Timer3_CLKO_Disable();	//不输出时钟

		T3_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer3_Run();	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer4)		//Timer3,固定为16位自动重装, 中断无优先级
	{
		Timer4_Stop();	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer4_InterruptEnable();	//允许中断
		else		Timer4_InterruptDisable();	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer4_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer4_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer4_AsCounter();	//对外计数或分频
		else		Timer4_AsTimer();		//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer4_CLKO_Enable();		//输出时钟
		else		Timer4_CLKO_Disable();	//不输出时钟

		T4_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer4_Run();	//开始运行
		return	0;		//成功
	}
	return 2;	//错误
}

void Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;						//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority    = Priority_0;			//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000);		//中断频率, 1000次/秒
	TIM_InitStructure.TIM_Run       = ENABLE;					//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);					//初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
}