#include "delay.h"
#include "rtc.h" 		    

u8 RTC_Init(void)
{
	//����ǲ��ǵ�һ������ʱ��
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  		
	BKP_DeInit();	//��λ�������� 	
	RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//���ָ����RCC��־λ�������,�ȴ����پ������
		{
		temp++;
		delay_ms(10);
		}
	if(temp>=250)return 1;//��ʼ��ʱ��ʧ��,����������	    
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
	RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��  
	//RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_EnterConfigMode();/// ��������	
	RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_Set(0);  //����ʱ��	
	RTC_ExitConfigMode(); //�˳�����ģʽ  
	//RTC_NVIC_Config();//RCT�жϷ�������	
	return 0; //ok
}

u32 RTC_Get(void)
{
	return RTC_GetCounter();	    
}


void RTC_Set(u32 sec)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
	PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
	RTC_SetCounter(sec);	//����RTC��������ֵ
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
}
















