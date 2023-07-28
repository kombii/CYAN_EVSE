#include	"adc.h"

void ADC_Inilize(ADC_InitTypeDef *ADCx)
{
	ADCCFG = (ADCCFG & ~ADC_SPEED_2X16T) | ADCx->ADC_Speed;
	if(ADCx->ADC_Power == ENABLE)	ADC_CONTR |= 0x80;
	else							ADC_CONTR &= 0x7F;
	if(ADCx->ADC_AdjResult == ADC_RIGHT_JUSTIFIED)	ADCCFG |=  (1<<5);	//AD转换结果右对齐。
	else									ADCCFG &= ~(1<<5);	//AD转换结果左对齐。 
	if(ADCx->ADC_Interrupt == ENABLE)	EADC = 1;			//中断允许		ENABLE,DISABLE
	else								EADC = 0;
	if(ADCx->ADC_Priority > Priority_3)	return;	//错误
	ADC_Priority(ADCx->ADC_Priority);	//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	if(ADCx->ADC_SMPduty > 31)	return;	//错误
	if(ADCx->ADC_CsSetup > 1)	return;	//错误
	if(ADCx->ADC_CsHold > 3)	return;	//错误
	P_SW2 |= 0x80;
	ADCTIM = (ADCx->ADC_CsSetup << 7) | (ADCx->ADC_CsHold << 5) | ADCx->ADC_SMPduty ;		//设置 ADC 内部时序，ADC采样时间建议设最大值
	P_SW2 &= 0x7f;
}

void	ADC_PowerControl(u8 pwr)
{
	if(pwr == ENABLE)	ADC_CONTR |= 0x80;
	else				ADC_CONTR &= 0x7f;
}

u16	Get_ADCResult(u8 channel)	//channel = 0~15
{
	u16	adc;
	u8	i;

	if(channel > ADC_CH15)	return	4096;	//错误,返回4096,调用的程序判断	
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xf0) | ADC_START | channel; 
	NOP(4);			//对ADC_CONTR操作后要4T之后才能访问

	for(i=0; i<250; i++)		//超时
	{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			if(ADCCFG &  (1<<5))		//转换结果右对齐。 
			{
				adc = ((u16)ADC_RES << 8) | ADC_RESL;
			}
			else		//转换结果左对齐。 
			{
				#if ADC_RES_12BIT==1
					adc = (u16)ADC_RES;
					adc = (adc << 4) | ((ADC_RESL >> 4)&0x0f);
				#else
					adc = (u16)ADC_RES;
					adc = (adc << 2) | ((ADC_RESL >> 6)&0x03);
				#endif
			}
			return	adc;
		}
	}
	return	4096;	//错误,返回4096,调用的程序判断
}

void ADC_int (void) interrupt ADC_VECTOR
{
	ADC_CONTR &= ~ADC_FLAG;
}

void ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//结构定义
	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC 模拟信号采样时间控制, 0~31（注意： SMPDUTY 一定不能设置小于 10）
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC 通道选择时间控制 0(默认),1
	ADC_InitStructure.ADC_CsHold    = 1;		//ADC 通道选择保持时间控制 0,1(默认),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X1T;		//设置 ADC 工作时钟频率	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_Power     = ENABLE;				//ADC功率允许/关闭	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC结果调整,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_InitStructure.ADC_Priority    = Priority_0;			//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//中断允许	ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//初始化
	ADC_PowerControl(ENABLE);							//单独的ADC电源操作函数, ENABLE或DISABLE
}

