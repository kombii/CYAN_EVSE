#include	"timer.h"
#include    "BspTime.h"

void timer0_int (void) interrupt TIMER0_VECTOR
{
	tickingTimer();
}
void timer1_int (void) interrupt TIMER1_VECTOR
{
}
void timer2_int (void) interrupt TIMER2_VECTOR
{
}
void timer3_int (void) interrupt TIMER3_VECTOR
{
}
void timer4_int (void) interrupt TIMER4_VECTOR
{
}


//========================================================================
// ����: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// ����: ��ʱ����ʼ������.
// ����: TIMx: �ṹ����,��ο�timer.h��Ķ���.
// ����: �ɹ�����0, �ղ�������1,���󷵻�2.
// �汾: V1.0, 2012-10-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	if(TIM > Timer4)	return 1;	//�ղ���

	if(TIM == Timer0)
	{
		Timer0_Stop();		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer0_InterruptEnable();	//�����ж�
		else		Timer0_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_Priority > Priority_3)	return 2;	//����
		Timer0_Priority(TIMx->TIM_Priority);	//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer0_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer0_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer0_AsCounter();	//����������Ƶ
		else		Timer0_AsTimer();	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer0_CLKO_Enable();	//���ʱ��
		else		Timer0_CLKO_Disable();	//�����ʱ��
		
		T0_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer0_Run();	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer1)
	{
		Timer1_Stop();		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer1_InterruptEnable();	//�����ж�
		else		Timer1_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_Priority > Priority_3)	return 2;	//����
		Timer1_Priority(TIMx->TIM_Priority);	//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer1_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer1_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer1_AsCounter();	//����������Ƶ
		else		Timer1_AsTimer();	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer1_CLKO_Enable();	//���ʱ��
		else		Timer1_CLKO_Disable();	//�����ʱ��
		
		T1_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer1_Run();	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer2)		//Timer2,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		Timer2_Stop();	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer2_InterruptEnable();	//�����ж�
		else		Timer2_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer2_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer2_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer2_AsCounter();	//����������Ƶ
		else		Timer2_AsTimer();		//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer2_CLKO_Enable();		//���ʱ��
		else		Timer2_CLKO_Disable();	//�����ʱ��

		T2_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer2_Run();	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer3)		//Timer3,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		Timer3_Stop();	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer3_InterruptEnable();	//�����ж�
		else		Timer3_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer3_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer3_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer3_AsCounter();	//����������Ƶ
		else		Timer3_AsTimer();		//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer3_CLKO_Enable();		//���ʱ��
		else		Timer3_CLKO_Disable();	//�����ʱ��

		T3_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer3_Run();	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer4)		//Timer3,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		Timer4_Stop();	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer4_InterruptEnable();	//�����ж�
		else		Timer4_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer4_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer4_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer4_AsCounter();	//����������Ƶ
		else		Timer4_AsTimer();		//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer4_CLKO_Enable();		//���ʱ��
		else		Timer4_CLKO_Disable();	//�����ʱ��

		T4_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer4_Run();	//��ʼ����
		return	0;		//�ɹ�
	}
	return 2;	//����
}

void Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;						//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority    = Priority_0;			//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000);		//�ж�Ƶ��, 1000��/��
	TIM_InitStructure.TIM_Run       = ENABLE;					//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);					//��ʼ��Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
}