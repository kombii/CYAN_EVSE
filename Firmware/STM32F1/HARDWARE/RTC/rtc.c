#include "delay.h"
#include "rtc.h" 		    

u8 RTC_Init(void)
{
	//检查是不是第一次配置时钟
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  		
	BKP_DeInit();	//复位备份区域 	
	RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
		temp++;
		delay_ms(10);
		}
	if(temp>=250)return 1;//初始化时钟失败,晶振有问题	    
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
	RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	RTC_WaitForSynchro();		//等待RTC寄存器同步  
	//RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	RTC_EnterConfigMode();/// 允许配置	
	RTC_SetPrescaler(32767); //设置RTC预分频的值
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	RTC_Set(0);  //设置时间	
	RTC_ExitConfigMode(); //退出配置模式  
	//RTC_NVIC_Config();//RCT中断分组设置	
	return 0; //ok
}

u32 RTC_Get(void)
{
	return RTC_GetCounter();	    
}


void RTC_Set(u32 sec)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
	RTC_SetCounter(sec);	//设置RTC计数器的值
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
}
















