#include "timer.h"

//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_DeInit(TIM2);//������ TIMx �Ĵ�������Ϊȱʡֵ
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);       //TIM2 Remap CH1->PA15 
 
	//���ø�����Ϊ�����������,���TIM2_CH1��PWM���岨��	GPIOA.15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //TIM2_CH1_ETR
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
	//��ʼ��TIM2
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM2 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	//TIM_OCInitStructure.TIM_Pulse = 1;//�����˴�װ�벶��ȽϼĴ���������ֵ
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //����ָ���Ĳ�����ʼ������

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR2�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
}

void PwmControl(bool bCon){
    if(bCon){
		//�κ�����¶�Ҫ��Ϊǰ����ù�ForcedOC1Config��������Ҫ��������pwm
		TIM_SelectOCxM(TIM2, TIM_Channel_1, TIM_OCMode_PWM2);
		TIM_CCxCmd(TIM2, TIM_Channel_1, TIM_CCx_Enable);
		TIM_Cmd(TIM2, ENABLE);
    }else{
		TIM_Cmd(TIM2, DISABLE);
		TIM_ForcedOC1Config(TIM2, TIM_ForcedAction_Active);   //����Ϊ�ǻ״̬���͵�ƽ
    }
}
