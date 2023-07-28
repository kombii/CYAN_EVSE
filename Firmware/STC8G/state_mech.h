#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "TYPESS.H"
//״̬����-----------------------------------------

#define ac_voltage_max (260.00f)
#define ac_voltage_min (180.00f)

#define ac_current_size_20 (1.1f)
#define ac_current_size_0 (2.0f)

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

extern bit chargegun;//��ǹ״̬ 1���� 0�γ�
extern bit chargestart;//�����������״̬ 1�������� 0û��

extern bit S1_Switch_State;//S1����״̬
extern bit rfidlock;//rfid��״̬
extern bit blelock;//ble��״̬
extern bit outvol;//�����ѹ״̬

extern uint32_t cp_voltage;//cp�ߵ�ѹ����0.01v��λ
extern uint8_t charge_state;//���״̬
extern uint8_t cp_state;//cp�ߵ�λ
extern uint16_t avaliable_current;//��ǰ����
//0 6 9 12 255

typedef enum _charge_states{
	charge_state_idle,//״̬1
	charge_state_insert_gun,//״̬2
	charge_state_gun_pwm,//״̬2��
	charge_state_charging,//״̬3��
	charge_state_stopping,//״̬3
	charge_state_nogun_pwm,//״̬1��
	charge_state_err//״̬0
}charge_states;

void State_Task_Init(void);
void main_task(void);

#endif