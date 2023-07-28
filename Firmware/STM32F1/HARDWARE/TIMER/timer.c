#include "timer.h"

//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_DeInit(TIM2);//将外设 TIMx 寄存器重设为缺省值
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);       //TIM2 Remap CH1->PA15 
 
	//设置该引脚为复用输出功能,输出TIM2_CH1的PWM脉冲波形	GPIOA.15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //TIM2_CH1_ETR
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
	//初始化TIM2
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM2 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	//TIM_OCInitStructure.TIM_Pulse = 1;//设置了待装入捕获比较寄存器的脉冲值
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据指定的参数初始化外设

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR2上的预装载寄存器
 
	TIM_Cmd(TIM2, ENABLE);  //使能TIM2
}

void PwmControl(bool bCon){
    if(bCon){
		//任何情况下都要认为前面调用过ForcedOC1Config，所以需要重新启用pwm
		TIM_SelectOCxM(TIM2, TIM_Channel_1, TIM_OCMode_PWM2);
		TIM_CCxCmd(TIM2, TIM_Channel_1, TIM_CCx_Enable);
		TIM_Cmd(TIM2, ENABLE);
    }else{
		TIM_Cmd(TIM2, DISABLE);
		TIM_ForcedOC1Config(TIM2, TIM_ForcedAction_Active);   //设置为非活动状态，低电平
    }
}
