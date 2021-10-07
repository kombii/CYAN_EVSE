#include "pwm.h"

PWMx_Duty PWMA_Duty;

void PWM_config(void)
{
	PWMx_InitDefine		PWMx_InitStructure;
	
	PWMx_InitStructure.PWM3_Mode    =	CCMRn_PWM_MODE1;	//ģʽ,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	PWMx_InitStructure.PWM3_SetPriority  = Priority_0;			//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	
	PWMx_InitStructure.PWM_Period   = MAIN_Fosc/1000;							//����ʱ��,   0~65535
	PWMx_InitStructure.PWM3_Duty    = PWMA_Duty.PWM3_Duty;	//PWM3ռ�ձ�ʱ��, 0~Period
	PWMx_InitStructure.PWM_DeadTime = 0;								//��������������, 0~255
	
	PWMx_InitStructure.PWM_EnoSelect   = ENO3P;	//���ͨ��ѡ��,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
	PWMx_InitStructure.PWM_PS_SW       = PWM3_SW_P14_P15;	//�л��˿�,		PWM1_SW_P10_P11,PWM1_SW_P20_P21,PWM1_SW_P60_P61

	PWMx_InitStructure.PWM_CC3Enable   = ENABLE;				//����PWM3P���벶��/�Ƚ����,  ENABLE,DISABLE
	
	PWMx_InitStructure.PWM_MainOutEnable= ENABLE;				//�����ʹ��, ENABLE,DISABLE
	PWMx_InitStructure.PWM_CEN_Enable   = ENABLE;				//ʹ�ܼ�����, ENABLE,DISABLE
	PWM_Configuration(PWMA, &PWMx_InitStructure);				//��ʼ��PWM,  PWMA,PWMB
}

u8 PWM_Configuration(u8 PWM, PWMx_InitDefine *PWMx)
{
	if(PWM == PWMA)
	{
		EAXSFR();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չSFR(XSFR) */
		
		if(PWMx->PWM1_SetPriority <= Priority_3)	PWM1_Priority(PWMx->PWM1_SetPriority);	//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		if(PWMx->PWM2_SetPriority <= Priority_3)	PWM2_Priority(PWMx->PWM2_SetPriority);	//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		if(PWMx->PWM3_SetPriority <= Priority_3)	PWM3_Priority(PWMx->PWM3_SetPriority);	//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		if(PWMx->PWM4_SetPriority <= Priority_3)	PWM4_Priority(PWMx->PWM4_SetPriority);	//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		
		PWMA_CCER1_Disable();		//�ر��������벶��/�Ƚ����
		PWMA_CCER2_Disable();		//�ر��������벶��/�Ƚ����
		PWMA_OC1ModeSet(PWMx->PWM1_Mode);		//��������Ƚ�ģʽ
		PWMA_OC2ModeSet(PWMx->PWM2_Mode);		//��������Ƚ�ģʽ
		PWMA_OC3ModeSet(PWMx->PWM3_Mode);		//��������Ƚ�ģʽ
		PWMA_OC4ModeSet(PWMx->PWM4_Mode);		//��������Ƚ�ģʽ
		if(PWMx->PWM_Reload == ENABLE)	PWMA_OC1_ReloadEnable();	//����Ƚϵ�Ԥװ��ʹ��
		else		PWMA_OC1_RelosdDisable();	//��ֹ����Ƚϵ�Ԥװ��
		if(PWMx->PWM_Fast == ENABLE)	PWMA_OC1_FastEnable();		//����ȽϿ��ٹ���ʹ��
		else		PWMA_OC1_FastDisable();	//��ֹ����ȽϿ��ٹ���
		
		if(PWMx->PWM_CC1Enable == ENABLE)	PWMA_CC1E_Enable();			//�������벶��/�Ƚ����
		else		PWMA_CC1E_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC1NEnable == ENABLE)	PWMA_CC1NE_Enable();	//�������벶��/�Ƚ����
		else		PWMA_CC1NE_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC2Enable == ENABLE)	PWMA_CC2E_Enable();			//�������벶��/�Ƚ����
		else		PWMA_CC2E_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC2NEnable == ENABLE)	PWMA_CC2NE_Enable();	//�������벶��/�Ƚ����
		else		PWMA_CC2NE_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC3Enable == ENABLE)	PWMA_CC3E_Enable();			//�������벶��/�Ƚ����
		else		PWMA_CC3E_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC3NEnable == ENABLE)	PWMA_CC3NE_Enable();	//�������벶��/�Ƚ����
		else		PWMA_CC3NE_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC4Enable == ENABLE)	PWMA_CC4E_Enable();			//�������벶��/�Ƚ����
		else		PWMA_CC4E_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC4NEnable == ENABLE)	PWMA_CC4NE_Enable();	//�������벶��/�Ƚ����
		else		PWMA_CC4NE_Disable();	//�ر����벶��/�Ƚ����
		
		PWMA_AutoReload(PWMx->PWM_Period);
		PWMA_Duty1(PWMx->PWM1_Duty);
		PWMA_Duty2(PWMx->PWM2_Duty);
		PWMA_Duty3(PWMx->PWM3_Duty);
		PWMA_Duty4(PWMx->PWM4_Duty);
		
		PWMA_CCPCAPreloaded(PWMx->PWM_PreLoad);	//����/�Ƚ�Ԥװ�ؿ���λ(��λֻ�Ծ��л��������ͨ��������)
		PWMA_PS = PWMx->PWM_PS_SW;			//�л�IO
		PWMA_ENO = PWMx->PWM_EnoSelect;	//���ͨ��ѡ��
		PWMA_DeadTime(PWMx->PWM_DeadTime);	//��������������
		
		if(PWMx->PWM_BrakeEnable == ENABLE)	PWMA_BrakeEnable();	//����ɲ������
		else		PWMA_BrakeDisable();		//��ֹɲ������
		if(PWMx->PWM_MainOutEnable == ENABLE)	PWMA_BrakeOutputEnable();	//�����ʹ��
		else		PWMA_BrakeOutputDisable();		//�������ֹ
		if(PWMx->PWM_CEN_Enable == ENABLE)	PWMA_CEN_Enable();	//ʹ�ܼ�����
		else		PWMA_CEN_Disable();		//��ֹ������
		
		EAXRAM();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չRAM(XRAM) */
		return	0;
	}

	if(PWM == PWMB)
	{
		EAXSFR();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չSFR(XSFR) */
		
		if(PWMx->PWM5_SetPriority <= Priority_3)	PWM5_Priority(PWMx->PWM5_SetPriority);	//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		
		PWMB_CCER1_Disable();		//�ر��������벶��/�Ƚ����
		PWMB_CCER2_Disable();		//�ر��������벶��/�Ƚ����
		PWMB_OC5ModeSet(PWMx->PWM5_Mode);		//��������Ƚ�ģʽ
		PWMB_OC6ModeSet(PWMx->PWM6_Mode);		//��������Ƚ�ģʽ
		PWMB_OC7ModeSet(PWMx->PWM7_Mode);		//��������Ƚ�ģʽ
		PWMB_OC8ModeSet(PWMx->PWM8_Mode);		//��������Ƚ�ģʽ
		if(PWMx->PWM_Reload == ENABLE)	PWMB_OC5_ReloadEnable();	//����Ƚϵ�Ԥװ��ʹ��
		else		PWMB_OC5_RelosdDisable();	//��ֹ����Ƚϵ�Ԥװ��
		if(PWMx->PWM_Fast == ENABLE)	PWMB_OC5_FastEnable();		//����ȽϿ��ٹ���ʹ��
		else		PWMB_OC5_FastDisable();	//��ֹ����ȽϿ��ٹ���
		
		if(PWMx->PWM_CC5Enable == ENABLE)	PWMB_CC5E_Enable();			//�������벶��/�Ƚ����
		else		PWMB_CC5E_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC6Enable == ENABLE)	PWMB_CC6E_Enable();			//�������벶��/�Ƚ����
		else		PWMB_CC6E_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC7Enable == ENABLE)	PWMB_CC7E_Enable();			//�������벶��/�Ƚ����
		else		PWMB_CC7E_Disable();	//�ر����벶��/�Ƚ����
		if(PWMx->PWM_CC8Enable == ENABLE)	PWMB_CC8E_Enable();			//�������벶��/�Ƚ����
		else		PWMB_CC8E_Disable();	//�ر����벶��/�Ƚ����
		
		PWMB_AutoReload(PWMx->PWM_Period);
		PWMB_Duty5(PWMx->PWM5_Duty);
		PWMB_Duty6(PWMx->PWM6_Duty);
		PWMB_Duty7(PWMx->PWM7_Duty);
		PWMB_Duty8(PWMx->PWM8_Duty);
		
		PWMB_CCPCBPreloaded(PWMx->PWM_PreLoad);	//����/�Ƚ�Ԥװ�ؿ���λ(��λֻ�Ծ��л��������ͨ��������)
		PWMB_PS = PWMx->PWM_PS_SW;			//�л�IO
		PWMB_ENO = PWMx->PWM_EnoSelect;	//���ͨ��ѡ��
		PWMB_DeadTime(PWMx->PWM_DeadTime);	//��������������
		
		if(PWMx->PWM_BrakeEnable == ENABLE)	PWMB_BrakeEnable();	//����ɲ������
		else		PWMB_BrakeDisable();		//��ֹɲ������
		if(PWMx->PWM_MainOutEnable == ENABLE)	PWMB_BrakeOutputEnable();	//�����ʹ��
		else		PWMB_BrakeOutputDisable();		//�������ֹ
		if(PWMx->PWM_CEN_Enable == ENABLE)	PWMB_CEN_Enable();	//ʹ�ܼ�����
		else		PWMB_CEN_Disable();		//��ֹ������
		
		EAXRAM();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չRAM(XRAM) */
		return	0;
	}

	return	2;	//����
}

/*********************************************************/

/******************* PWM ռ�ձ����ú��� *******************/
void UpdatePwm(u8 PWM, PWMx_Duty *PWMx)
{
	EAXSFR();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չSFR(XSFR) */

	if(PWM == PWMA)
	{
		PWMA_Duty1(PWMx->PWM1_Duty);
		PWMA_Duty2(PWMx->PWM2_Duty);
		PWMA_Duty3(PWMx->PWM3_Duty);
		PWMA_Duty4(PWMx->PWM4_Duty);
	}
	else
	{
		PWMB_Duty5(PWMx->PWM5_Duty);
		PWMB_Duty6(PWMx->PWM6_Duty);
		PWMB_Duty7(PWMx->PWM7_Duty);
		PWMB_Duty8(PWMx->PWM8_Duty);
	}
	
	EAXRAM();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չRAM(XRAM) */
}

/*********************************************************/
