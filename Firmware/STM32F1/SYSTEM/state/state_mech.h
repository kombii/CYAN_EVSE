#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "sys.h"

//��׼io����

#define AC_LED_PIN GPIO_Pin_14 //�е�ledָʾ��
#define AC_LED_GPIO GPIOB
#define AC_LED_GPIO_OUT PBout(14)
#define GUN_LED_PIN GPIO_Pin_2 //��ǹledָʾ��
#define GUN_LED_GPIO GPIOB
#define GUN_LED_GPIO_OUT PBout(2)
#define UCK_LED_PIN GPIO_Pin_12//����ledָʾ��
#define UCK_LED_GPIO GPIOB
#define UCK_LED_GPIO_OUT PBout(12)
#define CHG_LED_PIN GPIO_Pin_7 //�����ledָʾ��
#define CHG_LED_GPIO GPIOA
#define CHG_LED_GPIO_OUT PAout(7)
#define EMG_OUT_PIN GPIO_Pin_13//����ledָʾ��+������
#define EMG_OUT_GPIO GPIOB
#define EMG_OUT_GPIO_OUT PBout(13)

#define EMG_STOP_PIN GPIO_Pin_12//������������
#define EMG_STOP_GPIO GPIOA
#define EMG_STOP_GPIO_IN PAin(12)
#define BTN_PIN GPIO_Pin_11     //��ť����
#define BTN_GPIO GPIOA
#define BTN_GPIO_IN PAin(11)
#define SELA_PIN GPIO_Pin_8	    //��ť�������
#define SELA_GPIO GPIOA
#define SELA_GPIO_IN PAin(8)
#define SELB_PIN GPIO_Pin_15    //��ť�Ҳ�����
#define SELB_GPIO GPIOB
#define SELB_GPIO_IN PBin(15)

#define RFID_UNLOCK_PIN GPIO_Pin_0 //rfid������
#define RFID_UNLOCK_GPIO GPIOB
#define RFID_UNLOCK_GPIO_IN PBin(0)

#define BLE_LOCK_PIN GPIO_Pin_6    //����������
#define BLE_LOCK_GPIO GPIOA
#define BLE_LOCK_GPIO_IN PAin(6)
#define BLE_UNLOCK_PIN GPIO_Pin_5  //������������
#define BLE_UNLOCK_GPIO GPIOA
#define BLE_UNLOCK_GPIO_IN PAin(5)

#define AC_OUT_IN_PIN GPIO_Pin_5    //AC�����Ӧ����
#define AC_OUT_IN_GPIO GPIOB
#define AC_OUT_IN_GPIO_IN PBin(5)

#define RELAY_OUT_PIN GPIO_Pin_4    //�Ӵ������
#define RELAY_OUT_GPIO GPIOB
#define RELAY_OUT_GPIO_OUT PBout(4)

//״̬����-----------------------------------------

#define ac_voltage_max (260.00f)
#define ac_voltage_min (180.00f)

#define ac_current_size_20 (1.1f)
#define ac_current_size_0 (2.0f)

#define EPSINON (1e-6)

#define temp_threshold_high 790

extern volatile uint32_t emgstate;//����״̬���� 1���� 0����
extern volatile uint32_t outputen;//������� 1���� 0������
extern volatile uint32_t lock;//�ⲿ����ϵͳ 1���� 0����

extern volatile uint32_t output;//��� 1��� 0�ر����

extern volatile uint32_t overtemp;//���� 1���� 0����
extern volatile uint32_t overvoltage;//��ѹ 1���� 0����
extern volatile uint32_t undervoltage;//Ƿѹ 1���� 0����
extern volatile uint32_t overcurrent;//���� 1���� 0����
extern volatile uint32_t emgstop;//����ֹͣ��ť 1���� 0����

extern volatile uint32_t chargegun;//��ǹ״̬ 1���� 0�γ�
extern volatile uint32_t chargestart;//�����������״̬ 1�������� 0û��

extern volatile uint32_t S1_Switch_State;//S1����״̬
extern volatile uint32_t rfidlock;//rfid��״̬
extern volatile uint32_t blelock;//ble��״̬
extern volatile uint32_t outvol;//�����ѹ״̬

extern volatile uint32_t meter_err;//��ͷͨ�Ŵ���

extern volatile uint32_t cp_voltage;//cp�ߵ�ѹ����0.01v��λ
extern volatile uint8_t charge_state;//���״̬
extern volatile uint8_t cp_state;//cp�ߵ�λ
extern volatile uint16_t avaliable_current;//��ǰ����

extern volatile uint16_t usr_set_current;//�û����õ���
extern volatile uint16_t usr_sel_limit_mode;//�û�ѡ�������ģʽ
extern volatile uint16_t time_limit;//�û�ѡ�������ʱ������λ���ӣ�
extern volatile uint16_t energy_limit;//�û�ѡ���������������λ0.1kwh��


extern volatile uint32_t energy_charged;//���ֳ��ĵ���
extern volatile uint32_t time_charged;//���ֳ���ʱ��
extern volatile uint8_t stop_reason;//ֹͣԭ��

extern volatile uint32_t alarm_on;//�Ƿ���������


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

void Get_Datas_From_EEPROM(void);
void State_Task_Init(void);
void main_task(void);
void Out_Task(void);

#endif

