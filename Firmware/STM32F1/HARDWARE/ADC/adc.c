#include "adc.h"
#include "delay.h"

#define    ADC1_DR_Address ((u32)0x40012400+0x4c)

#define    ADC_ACQ_TIMES    20                             //采样次数
#define    ADC_ACQ_CHAS    2                              //采样通道个数
volatile uint16_t   ADC_ConvertedValue[ADC_ACQ_TIMES][ADC_ACQ_CHAS];           //采集数据的存储地址

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	{     
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	}
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStructure.DMA_PeripheralBaseAddr=ADC1_DR_Address;
		DMA_InitStructure.DMA_MemoryBaseAddr=(u32)&ADC_ConvertedValue;
		DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//外设-->内存

		DMA_InitStructure.DMA_BufferSize=ADC_ACQ_TIMES*ADC_ACQ_CHAS;
		DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;

		DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;

		DMA_InitStructure.DMA_Priority=DMA_Priority_High;
		DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
		DMA_Init(DMA1_Channel1,&DMA_InitStructure);	
	}
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);	  		//使能ADC1通道时钟
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);   						//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
		ADC_DeInit(ADC1);
		ADC_InitStructure.ADC_Mode=ADC_Mode_Independent; 			//配置成独立工作模
		ADC_InitStructure.ADC_ScanConvMode=ENABLE;      			//多通道循环扫描
		ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;  			//使能自动连续转换
		ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//转换由软件触发启动
		ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;		//数据选择右对其
		ADC_InitStructure.ADC_NbrOfChannel=ADC_ACQ_CHAS;			//转换通道数
		ADC_Init(ADC1,&ADC_InitStructure);							//初始化adc
		//配置adc通道转换顺序和采样周期,(55.5+12.5)/12M=5.6us
		ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5); //9通道
		ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 2, ADC_SampleTime_55Cycles5); //基准
    
		ADC_DMACmd(ADC1, ENABLE);											//adc传输选用dma1工作模式，
		ADC_Cmd(ADC1,ENABLE);												//使能ADC1
  
		ADC_ResetCalibration(ADC1);											//复位校准寄存器
		ADC_TempSensorVrefintCmd(ENABLE);									//启用内部温度传感器和基准电压源
		delay_ms(10);														//听说GD得加这个
		while(ADC_GetResetCalibrationStatus(ADC1)){}		    			//等待校准寄存器复位完成
		ADC_StartCalibration(ADC1);											//AD校准
		while(ADC_GetCalibrationStatus(ADC1)){}								//等待结束
		DMA_Cmd(DMA1_Channel1,ENABLE);	 									//使能DMA																				// 由于没有采用外部触发，所以使用软件触发ADC转换
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}
}				  

//修改过的获得平均值，ch只有0和1，0代表cp通道，1代表基准通道
u16 Get_Adc_Average(u8 ch)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<ADC_ACQ_TIMES;t++)
	{
		temp_val+=ADC_ConvertedValue[t][ch];
	}
	return temp_val/ADC_ACQ_TIMES;
}
