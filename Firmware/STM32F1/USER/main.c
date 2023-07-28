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

//����
void soft_reset(void)
{
	printf("#����ϵͳ��ϵͳ�������ɿأ����ã����עԭ��\r\n");
	printf("#---------------------------------------------------\r\n");
	// �ر������ж�
    __set_FAULTMASK(1); 
	// ��λ
	NVIC_SystemReset(); 
}

//����¶ȴ�����״̬
void Check_DS(void){
	int32_t temperature;
	u8 i;
	u8 temp_checked = 0;
	Ds_RomSearch((GuidSearchTypdef *)DsGuid,8);
	if(Ds_Device_Searched_Num!=3){
		printf("#����ϵͳ�����´������������㣬�ܾ�����:\r\n");
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
		printf("#����ϵͳ�����´�����������������ʼ�����¶�:\r\n");
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
				printf("#����ϵͳ��������%d���¶ȶ�����:%d",i,temperature);
				if(temperature>550){
					printf("!!!�¶�̫��!!!\r\n");
					temp_checked = 0;
					delay_ms(500);
					break;
				}
				if(temperature==0){
					printf("!!!�¶�̫��!!!\r\n");
					temp_checked = 0;
					delay_ms(500);
					break;
				}
				printf("\r\n");
				temp_checked = 1;
			}
		}
		printf("#����ϵͳ���¶Ȳ��Խ��ͨ��\r\n");
	}
}

//������ʱ����Ӧ����ť
void Power_On_Emg_Button(void){
	if(EMG_STOP_GPIO_IN){
		printf("#����ϵͳ������ʱEMG��ť���£�\r\n");
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	State_Task_Init();
	
	uart1_init(4800);	 //���ɼ���
	uart2_init(115200);	 //485
	uart3_init(115200);	 //debug��
	
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
	
	printf("#����ϵͳ��ϵͳ�Լ�ͨ������ʼ����\r\n");
	
	IWDG_Init(4,625);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s	
	
	while(1){
		main_task();
		IWDG_Feed();//ι��
		if(EMG_STOP_GPIO_IN){
			soft_reset();
		}
	}
	
	
	
	
	
//	while(1){
//		if (USART2_RX_STA & 0x8000)
//		{
//			u16 len = USART2_RX_STA & 0x3fff; //�õ��˴ν��յ������ݳ���
//			printf("�յ���β:\r\n");
//			uint16_t comp = (USART2_RX_BUF[0]-0x30)*100+(USART2_RX_BUF[1]-0x30)*10+(USART2_RX_BUF[2]-0x30);
//			printf("comp:%d\r\n",comp);
//			TIM_SetCompare1(TIM2,comp);
//			for (t = 0; t < len; t++)
//			{
//				USART_SendData(USART2, USART2_RX_BUF[t]); //�򴮿�2��������
//				while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET)
//					; //�ȴ����ͽ���
//			}
//			printf("\r\n\r\n"); //���뻻��
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

