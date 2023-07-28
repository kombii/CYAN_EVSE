#include "BspTime.h"
autotimer bsptimers[TIMER_COUNT+1]= {0,0};
//+1的这个1，是用于delay的，不参与系统分配

autotimer* delaytimer = &bsptimers[TIMER_COUNT];

#ifdef BSPTIME_USE_SYSTICK

//初始化相关定时器和中断
void init_bsptimer(void){
	SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8; 
	SysTick->LOAD=SystemCoreClock/8000; 		//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk ;	//启用中断
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
	if(delaytimer != NULL){
		delaytimer->busy = 1;
	}
}

void delay_ms(uint16_t nms){
	setTimer(delaytimer,0);
	while(IS_INTIME_MS(delaytimer,nms));
}

#endif
#ifndef BSPTIME_USE_SYSTICK
/*
	定时器中断的定时时间设定
	定时器只需要配置时钟、TIM和NVIC即可
*/
void init_bsptimer(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		//使能TIM4时钟
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;				//TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);								//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_TimeBaseStructure.TIM_Period = 7199;				    //总的值设置为0xFFFF，设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = 9;					//预分频器
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM_Cmd(TIM4, ENABLE);										//开启定时器
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);					//开启定时器更新中断
}

//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) 			//溢出中断
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);				//清除中断标志位
		tickingTimer();
	}
}
#endif

/*
* 获取一个空闲的定时器，这个定时器
* 1、会自动跑滴答
* 2、是空闲的
*/
autotimer* obtainTimer(uint16_t start_value) {
    uint8_t i;
    for (i = 0; i < TIMER_COUNT; i++) {
        if (bsptimers[i].busy == 0) {
            bsptimers[i].time = start_value;
            bsptimers[i].busy = 1;
            break;
        }
    }
    return &bsptimers[i];
}
/*
* 设置定时器，该操作会操作目标定时器，修改当前值
*/
uint8_t setTimer(autotimer* timer,uint16_t start_value) {
    if(timer->busy == 0)return 0;
    timer->time = start_value;
    return 1;
}
/*
* 归还定时器，该操作会自动释放定时器
* 并且对定时器归零
*/
void returnTimer(autotimer* timer) {
    timer->busy = 0;
    timer->time = 0;
}

/*
* 获取定时器滴答数量
*/
uint16_t getTimer(autotimer* timer) {
    return timer->time;
}

/*
* 让所有被使用的定时器滴答一下
*/
void tickingTimer(void) {
    uint8_t i;
    for (i = 0; i <= TIMER_COUNT; i++) {
        if (bsptimers[i].busy == 1) {
            bsptimers[i].time += 1;
        }
    }
}
