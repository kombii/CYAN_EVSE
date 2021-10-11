#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "TYPESS.H"
//״̬����-----------------------------------------

#define ac_voltage_max (260L)
#define ac_voltage_min (180L)

#define ac_current_size_20 (1.1)
#define ac_current_size_0 (5)

#define EPSINON (1e-6)

extern bit emgstate;//����״̬���� 1���� 0����
extern bit outputen;//������� 1���� 0������
extern bit lock;//�ⲿ����ϵͳ 1���� 0����

extern bit output;//��� 1��� 0�ر����

extern bit overtemp;//���� 1���� 0����
extern bit overvoltage;//��ѹ 1���� 0����
extern bit undervoltage;//Ƿѹ 1���� 0����
extern bit overcurrent;//���� 1���� 0����
extern bit emgstop;//����ֹͣ��ť 1���� 0����

extern uint32_t cp_voltage;//cp�ߵ�ѹ����0.01v��λ
extern uint8_t charge_state;//���״̬
extern uint8_t cp_state;//cp�ߵ�λ
//0 6 9 12 255
void State_Task_Init(void);
void main_tast(void);

#endif