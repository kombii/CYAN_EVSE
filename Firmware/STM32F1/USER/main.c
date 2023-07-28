#include "sys.h"
#include "usart.h"
#include "BspTime.h"
#include "delay.h"
#include "timer.h"
#include "state_mech.h"
#include "cp_signal.h"
#include "s1_switch.h"
#include "charge_current.h"
#include "ui.h"
#include "rtc.h"
#include "Ds64RomSearch.h"
#include "ds18b20.h"
#include "wdg.h"

//重启
void soft_reset(void)
{
	printf("#主控系统：系统即将（可控）重置，请关注原因！\r\n");
	printf("#---------------------------------------------------\r\n");
	// 关闭所有中断
    __set_FAULTMASK(1); 
	// 复位
	NVIC_SystemReset(); 
}

//检测温度传感器状态
void Check_DS(void){
	int32_t temperature;
	u8 i;
	u8 temp_checked = 0;
	Ds_RomSearch((GuidSearchTypdef *)DsGuid,8);
	if(Ds_Device_Searched_Num!=3){
		printf("#测温系统：测温传感器数量不足，拒绝启动:\r\n");
		while(1){
				EMG_OUT_GPIO_OUT = 1;
				delay_ms(100);
				EMG_OUT_GPIO_OUT = 0;
				delay_ms(100);
				if(EMG_STOP_GPIO_IN){
					soft_reset();
				}
		}
	}
	else{
		printf("#测温系统：测温传感器搜索结束，开始测试温度:\r\n");
		EMG_OUT_GPIO_OUT = 1;
		delay_ms(20);
		EMG_OUT_GPIO_OUT = 0;
		delay_ms(50);
		EMG_OUT_GPIO_OUT = 1;
		delay_ms(20);
		EMG_OUT_GPIO_OUT = 0;
		while(temp_checked==0){
			for(i=0;i<3;i++){
				DS18B20_Start(DsGuid[i].guid.u64Bits);
			}
			//delay_ms(1000);
			for(i=0;i<3;i++){
				temperature=DS18B20_Get_Temp_After_Start(DsGuid[i].guid.u64Bits);
				printf("#测温系统：传感器%d的温度读数是:%d",i,temperature);
				if(temperature>550){
					printf("!!!温度太高!!!\r\n");
					temp_checked = 0;
					delay_ms(500);
					break;
				}
				if(temperature==0){
					printf("!!!温度太低!!!\r\n");
					temp_checked = 0;
					delay_ms(500);
					break;
				}
				printf("\r\n");
				temp_checked = 1;
			}
		}
		printf("#测温系统：温度测试结果通过\r\n");
	}
}

//开机的时候检测应急按钮
void Power_On_Emg_Button(void){
	if(EMG_STOP_GPIO_IN){
		printf("#主控系统：开机时EMG按钮按下！\r\n");
	}
	while(EMG_STOP_GPIO_IN){
		EMG_OUT_GPIO_OUT = 1;
		delay_ms(100);
		EMG_OUT_GPIO_OUT = 0;
		delay_ms(500);
	}
}

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	State_Task_Init();
	
	uart1_init(4800);	 //到采集器
	uart2_init(115200);	 //485
	uart3_init(115200);	 //debug用
	
	init_bsptimer();
	delay_init();
	RTC_Init();
	DS18B20_Init();
	
	CP_Init();
	S1_Switch_Init();
	Get_Datas_From_EEPROM();
	Ui_System_Init();
	
	Power_On_Emg_Button();
	
	Check_DS();
	
	printf("#主控系统：系统自检通过，开始运行\r\n");
	
	IWDG_Init(4,625);    //与分频数为64,重载值为625,溢出时间为1s	
	
	while(1){
		main_task();
		IWDG_Feed();//喂狗
		if(EMG_STOP_GPIO_IN){
			soft_reset();
		}
	}
	
	
	
	
	
//	while(1){
//		if (USART2_RX_STA & 0x8000)
//		{
//			u16 len = USART2_RX_STA & 0x3fff; //得到此次接收到的数据长度
//			printf("收到包尾:\r\n");
//			uint16_t comp = (USART2_RX_BUF[0]-0x30)*100+(USART2_RX_BUF[1]-0x30)*10+(USART2_RX_BUF[2]-0x30);
//			printf("comp:%d\r\n",comp);
//			TIM_SetCompare1(TIM2,comp);
//			for (t = 0; t < len; t++)
//			{
//				USART_SendData(USART2, USART2_RX_BUF[t]); //向串口2发送数据
//				while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET)
//					; //等待发送结束
//			}
//			printf("\r\n\r\n"); //插入换行
//			
//			
//			USART2_RX_STA = 0;
//		}
//		else
//		{
//			delay_ms(10);
//		}
//	}
	//		TIM_SetCompare2(TIM3,led0pwmval);
}

