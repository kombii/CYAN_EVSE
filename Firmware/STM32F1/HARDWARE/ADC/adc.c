#include "adc.h"
#include "delay.h"

#define    ADC1_DR_Address ((u32)0x40012400+0x4c)

#define    ADC_ACQ_TIMES    20                             //��������
#define    ADC_ACQ_CHAS    2                              //����ͨ������
volatile uint16_t   ADC_ConvertedValue[ADC_ACQ_TIMES][ADC_ACQ_CHAS];           //�ɼ����ݵĴ洢��ַ

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
		DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//����-->�ڴ�

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
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);	  		//ʹ��ADC1ͨ��ʱ��
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);   						//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
		ADC_DeInit(ADC1);
		ADC_InitStructure.ADC_Mode=ADC_Mode_Independent; 			//���óɶ�������ģ
		ADC_InitStructure.ADC_ScanConvMode=ENABLE;      			//��ͨ��ѭ��ɨ��
		ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;  			//ʹ���Զ�����ת��
		ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//ת���������������
		ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;		//����ѡ���Ҷ���
		ADC_InitStructure.ADC_NbrOfChannel=ADC_ACQ_CHAS;			//ת��ͨ����
		ADC_Init(ADC1,&ADC_InitStructure);							//��ʼ��adc
		//����adcͨ��ת��˳��Ͳ�������,(55.5+12.5)/12M=5.6us
		ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5); //9ͨ��
		ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 2, ADC_SampleTime_55Cycles5); //��׼
    
		ADC_DMACmd(ADC1, ENABLE);											//adc����ѡ��dma1����ģʽ��
		ADC_Cmd(ADC1,ENABLE);												//ʹ��ADC1
  
		ADC_ResetCalibration(ADC1);											//��λУ׼�Ĵ���
		ADC_TempSensorVrefintCmd(ENABLE);									//�����ڲ��¶ȴ������ͻ�׼��ѹԴ
		delay_ms(10);														//��˵GD�ü����
		while(ADC_GetResetCalibrationStatus(ADC1)){}		    			//�ȴ�У׼�Ĵ�����λ���
		ADC_StartCalibration(ADC1);											//ADУ׼
		while(ADC_GetCalibrationStatus(ADC1)){}								//�ȴ�����
		DMA_Cmd(DMA1_Channel1,ENABLE);	 									//ʹ��DMA																				// ����û�в����ⲿ����������ʹ���������ADCת��
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}
}				  

//�޸Ĺ��Ļ��ƽ��ֵ��chֻ��0��1��0����cpͨ����1�����׼ͨ��
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
