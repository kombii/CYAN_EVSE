#ifndef __PWM_H
#define __PWM_H	 

#include	"config.h"

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80

#define ENO5P       0x01
#define ENO6P       0x04
#define ENO7P       0x10
#define ENO8P       0x40

#define	PWMA	1
#define	PWMB	2

#define	PWM1_SW_P10_P11		0
#define	PWM1_SW_P20_P21		1
#define	PWM1_SW_P60_P61		2

#define	PWM2_SW_P12_P13		0
#define	PWM2_SW_P22_P23		(1<<2)
#define	PWM2_SW_P62_P63		(2<<2)

#define	PWM3_SW_P14_P15		0
#define	PWM3_SW_P24_P25		(1<<4)
#define	PWM3_SW_P64_P65		(2<<4)

#define	PWM4_SW_P16_P17		0
#define	PWM4_SW_P26_P27		(1<<6)
#define	PWM4_SW_P66_P67		(2<<6)
#define	PWM4_SW_P34_P33		(3<<6)

#define	PWM5_SW_P20				0
#define	PWM5_SW_P17				1
#define	PWM5_SW_P00				2
#define	PWM5_SW_P74				3

#define	PWM6_SW_P21				0
#define	PWM6_SW_P54				(1<<2)
#define	PWM6_SW_P01				(2<<2)
#define	PWM6_SW_P75				(3<<2)

#define	PWM7_SW_P22				0
#define	PWM7_SW_P33				(1<<4)
#define	PWM7_SW_P02				(2<<4)
#define	PWM7_SW_P76				(3<<4)

#define	PWM8_SW_P23				0
#define	PWM8_SW_P34				(1<<6)
#define	PWM8_SW_P03				(2<<6)
#define	PWM8_SW_P77				(3<<6)

typedef struct
{ 
	u8	PWM1_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	u8	PWM2_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	u8	PWM3_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	u8	PWM4_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	u8	PWM5_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	u8	PWM6_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	u8	PWM7_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	u8	PWM8_Mode;			//ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2

	u16	PWM_Period;			//����ʱ��,   0~65535
	u16	PWM1_Duty;			//PWM1ռ�ձ�ʱ��, 0~Period
	u16	PWM2_Duty;			//PWM2ռ�ձ�ʱ��, 0~Period
	u16	PWM3_Duty;			//PWM3ռ�ձ�ʱ��, 0~Period
	u16	PWM4_Duty;			//PWM4ռ�ձ�ʱ��, 0~Period
	u16	PWM5_Duty;			//PWM5ռ�ձ�ʱ��, 0~Period
	u16	PWM6_Duty;			//PWM6ռ�ձ�ʱ��, 0~Period
	u16	PWM7_Duty;			//PWM7ռ�ձ�ʱ��, 0~Period
	u16	PWM8_Duty;			//PWM8ռ�ձ�ʱ��, 0~Period
	u8	PWM_DeadTime;		//��������������, 0~255

	u8	PWM_CC1Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC1NEnable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC2Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC2NEnable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC3Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC3NEnable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC4Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC4NEnable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC5Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC6Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC7Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE
	u8	PWM_CC8Enable;	//�������벶��/�Ƚ����,  ENABLE,DISABLE

	u8	PWM_Reload;			//����Ƚϵ�Ԥװ��ʹ��,   ENABLE,DISABLE
	u8	PWM_Fast;				//����ȽϿ��ٹ���ʹ��,   ENABLE,DISABLE

	u8	PWM1_SetPriority;	//����PWM1���ȼ�,   Priority_0,Priority_1,Priority_2,Priority_3
	u8	PWM2_SetPriority;	//����PWM2���ȼ�,   Priority_0,Priority_1,Priority_2,Priority_3
	u8	PWM3_SetPriority;	//����PWM3���ȼ�,   Priority_0,Priority_1,Priority_2,Priority_3
	u8	PWM4_SetPriority;	//����PWM4���ȼ�,   Priority_0,Priority_1,Priority_2,Priority_3
	u8	PWM5_SetPriority;	//����PWM4���ȼ�,   Priority_0,Priority_1,Priority_2,Priority_3

	u8	PWM_EnoSelect;	//���ͨ��ѡ��,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
	u8	PWM_PreLoad;		//Ԥװ��,     ENABLE,DISABLE
	u8	PWM_PS_SW;			//�л��˿�,   PWM1_SW_P10_P11,PWM1_SW_P20_P21,PWM1_SW_P60_P61
	u8	PWM_CEN_Enable;	//ʹ�ܼ�����, ENABLE,DISABLE

	u8	PWM_BrakeEnable;	//ɲ������ʹ��,  ENABLE,DISABLE
	u8	PWM_MainOutEnable;//�����ʹ��,  ENABLE,DISABLE
} PWMx_InitDefine; 

typedef struct
{ 
	u16	PWM1_Duty;			//PWM1ռ�ձ�ʱ��, 0~Period
	u16	PWM2_Duty;			//PWM2ռ�ձ�ʱ��, 0~Period
	u16	PWM3_Duty;			//PWM3ռ�ձ�ʱ��, 0~Period
	u16	PWM4_Duty;			//PWM4ռ�ձ�ʱ��, 0~Period
	u16	PWM5_Duty;			//PWM5ռ�ձ�ʱ��, 0~Period
	u16	PWM6_Duty;			//PWM6ռ�ձ�ʱ��, 0~Period
	u16	PWM7_Duty;			//PWM7ռ�ձ�ʱ��, 0~Period
	u16	PWM8_Duty;			//PWM8ռ�ձ�ʱ��, 0~Period
} PWMx_Duty; 

extern PWMx_Duty PWMA_Duty;

void PWM_config(void);
u8	PWM_Configuration(u8 PWM, PWMx_InitDefine *PWMx);
void UpdatePwm(u8 PWM, PWMx_Duty *PWMx);

#endif

