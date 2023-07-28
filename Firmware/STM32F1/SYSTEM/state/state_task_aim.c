#include "state_mech.h"
#include "charge_current.h"
#include "BspTime.h"
#include "cp_signal.h"
#include "s1_switch.h"
#include "IM1281B.h"
#include "ui.h"
#include <stdio.h>
#include "usart.h"
#include "eeprom.h"
#include "rtc.h"
#include "ds18b20.h"
#include "Ds64RomSearch.h"

#define STATE_ADDR 0X20000100

volatile uint32_t statemech __attribute__((at(STATE_ADDR)));

volatile uint32_t emgstate 			__attribute__((at(BITBAND(STATE_ADDR,0))));
volatile uint32_t outputen 			__attribute__((at(BITBAND(STATE_ADDR,1))));
volatile uint32_t lock 				__attribute__((at(BITBAND(STATE_ADDR,2))));
volatile uint32_t output 			__attribute__((at(BITBAND(STATE_ADDR,3))));

volatile uint32_t sw_error 			__attribute__((at(BITBAND(STATE_ADDR,4))));
volatile uint32_t overtemp 			__attribute__((at(BITBAND(STATE_ADDR,5))));
volatile uint32_t overvoltage 		__attribute__((at(BITBAND(STATE_ADDR,6))));
volatile uint32_t undervoltage 		__attribute__((at(BITBAND(STATE_ADDR,7))));

volatile uint32_t overcurrent 		__attribute__((at(BITBAND(STATE_ADDR,8))));
volatile uint32_t emgstop 			__attribute__((at(BITBAND(STATE_ADDR,9))));
volatile uint32_t chargegun 		__attribute__((at(BITBAND(STATE_ADDR,10))));
volatile uint32_t chargestart 		__attribute__((at(BITBAND(STATE_ADDR,11))));

volatile uint32_t S1_Switch_State 	__attribute__((at(BITBAND(STATE_ADDR,12))));
volatile uint32_t rfidlock 			__attribute__((at(BITBAND(STATE_ADDR,13))));
volatile uint32_t blelock 			__attribute__((at(BITBAND(STATE_ADDR,14))));
volatile uint32_t outvol 			__attribute__((at(BITBAND(STATE_ADDR,15))));

volatile uint32_t meter_err 		__attribute__((at(BITBAND(STATE_ADDR,16))));
volatile uint32_t ble_pre_lock 		__attribute__((at(BITBAND(STATE_ADDR,17))));

volatile uint32_t cp_voltage = 0;//cp�ߵ�ѹ����0.01v��λ
volatile uint8_t charge_state = 0;//���״̬
volatile uint8_t cp_state = 0;//cp�ߵ�λ
volatile uint8_t stop_reason = 0;//ֹͣԭ��
//1;//��ѹ
//2;//����
//3;//�������Ͽ�
//4;//����ڼ�ֱ�Ӱ�ǹ
//5;//Ӧ������
//6;//����
//7;//�Ӵ����������
//8;//��������
//9;//������ʱ����
//10��//������������
//11; //�¶Ȳɼ�����
//255;//δ֪
volatile uint16_t avaliable_current = 0;//��ǰ����(0.1a)

volatile uint16_t usr_set_current = 0;//�û����õ���(0.1a)
volatile uint16_t usr_sel_limit_mode = 0;//�û�ѡ�������ģʽ
//0���û�ѡ������
//1���û�ѡ������ʱ��
//2���û�ѡ����������
volatile uint16_t time_limit = 0;//�û�ѡ�������ʱ������λ���ӣ�
volatile uint16_t energy_limit = 0;//�û�ѡ���������������λ0.1kwh��

volatile uint32_t energy_start_time = 0;//���ֳ�翪ʼʱ��ͷ��������¼

volatile uint32_t energy_charged = 0;//���ֳ��ĵ���
volatile uint32_t time_charged = 0;//���ֳ���ʱ��

volatile uint32_t alarm_on = 0;//�Ƿ���������

volatile uint32_t temp_list[3]={0};//�¶��б�

//��eeprom�ó����ݵ���ʱ������
void Get_Datas_From_EEPROM(){
	usr_sel_limit_mode = EEPROM_Get_Usr_Limit_Mode();
	usr_set_current = EEPROM_Get_Current();
	time_limit = EEPROM_Get_Time_Limit();
	energy_limit = EEPROM_Get_Energy_Limit();
}

void State_Task_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = RELAY_OUT_PIN; //�Ӵ������
	GPIO_Init(RELAY_OUT_GPIO, &GPIO_InitStructure);
	RELAY_OUT_GPIO_OUT = 0;
	
	GPIO_InitStructure.GPIO_Pin = AC_LED_PIN; //�е�ledָʾ��
	GPIO_Init(AC_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GUN_LED_PIN; //��ǹledָʾ��
	GPIO_Init(GUN_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = UCK_LED_PIN; //����ledָʾ��
	GPIO_Init(UCK_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CHG_LED_PIN; //�����ledָʾ��
	GPIO_Init(CHG_LED_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = EMG_OUT_PIN; //����ledָʾ��+������
	GPIO_Init(EMG_OUT_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_InitStructure.GPIO_Pin = EMG_STOP_PIN; //������������
	GPIO_Init(EMG_STOP_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BTN_PIN; //��ť����
	GPIO_Init(BTN_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SELA_PIN; //��ť�������
	GPIO_Init(SELA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SELB_PIN; //��ť�Ҳ�����
	GPIO_Init(SELB_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = RFID_UNLOCK_PIN; //rfid������
	GPIO_Init(RFID_UNLOCK_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLE_LOCK_PIN; //����������
	GPIO_Init(BLE_LOCK_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BLE_UNLOCK_PIN; //������������
	GPIO_Init(BLE_UNLOCK_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AC_OUT_IN_PIN; //AC�����Ӧ����
	GPIO_Init(AC_OUT_IN_GPIO, &GPIO_InitStructure);
	
	//��ʼ��sbit����
	emgstate = 0;//����״̬���� 1���� 0����
	outputen = 0;//������� 1���� 0������
	lock = 1;//�ⲿ����ϵͳ 1���� 0����
	output = 0;//��� 1��� 0�ر����
	sw_error = 0;//��������� 1���� 0����
	overtemp = 0;//���� 1���� 0����
	overvoltage = 0;//��ѹ 1���� 0����
	undervoltage = 0;//Ƿѹ 1���� 0����
	overcurrent = 0;//���� 1���� 0����
	emgstop = 0;//����ֹͣ��ť 1���� 0����
	chargegun = 0;//��ǹ״̬ 1���� 0�γ�
	chargestart = 0;//�����������״̬ 1�������� 0û��
	rfidlock = 1;//rfid����ϵͳ 1���� 0����
	blelock = 1;//ble����ϵͳ 1���� 0����
	outvol = 0;//�����ѹ״̬
	meter_err = 0;//����ģ�鹤���쳣��־ 1���� 0����
}

//Ӳ����������ɼ�
void HW_EMG_Task(void){
	uint32_t highest_temp = 0;
	u8 i;
	emgstop = EMG_STOP_GPIO_IN; 	//Ӧ���������룬����Ч
	for(i=0;i<3;i++){
		if(temp_list[i]>highest_temp){
			highest_temp = temp_list[i];
		}
	}
	if (highest_temp > temp_threshold_high){
		overtemp = 1;
	} 
	else{
		overtemp = 0;
	}
	emgstate = emgstop || overtemp; //������������Ļ��ϵ
	if(emgstate){
		alarm_on=1;
	}
}
//���״̬
void Out_Task(void){
	if(	(outputen)&&!(emgstate)&&!(emgstop)&&!(overtemp)){
		if(output == 0){
			output = 1;
			printf("#���ʼ����������\r\n");
		}
	}else{
		if(output == 1){
			output = 0;
			printf("#���ʼ����ر����\r\n");
		}
	}
	RELAY_OUT_GPIO_OUT = output;
	outvol = AC_OUT_IN_GPIO_IN;
}
//�������ָʾ��
void LED_Task(void){
	static uint8_t is_alarm_on = 0;
	static autotimer * alarmtimer = NULL;
	if (alarmtimer == NULL)alarmtimer = obtainTimer(0);
	
	AC_LED_GPIO_OUT	 = !(overvoltage||undervoltage);
	GUN_LED_GPIO_OUT = chargegun;
	UCK_LED_GPIO_OUT = lock;
	CHG_LED_GPIO_OUT = output;
	if(alarm_on==1){
		is_alarm_on = 1;
		alarm_on = 0;
		EMG_OUT_GPIO_OUT = 1;
		setTimer(alarmtimer,0);
	}
	if(is_alarm_on){
		if(IS_TIMEOUT_MS(alarmtimer,10)){
			EMG_OUT_GPIO_OUT = 0;
			is_alarm_on = 0;
		}
	}
}
//�ɼ���ѹ����
void Elec_Vol_Task(void){
	//�жϵ�ѹ״̬
	if((elec_condition.Voltage*0.0001) > ac_voltage_max){
		overvoltage = 1;
	}else{
		overvoltage = 0;
		if ((elec_condition.Voltage*0.0001) < ac_voltage_min && !(((elec_condition.Voltage*0.0001)>=-EPSINON) && ((elec_condition.Voltage*0.0001)<=EPSINON))){
			undervoltage = 1;
		}else {
			undervoltage = 0;
		}	
	}
}

//�ɼ���������
void Elec_Curr_Task(void){
	//�жϵ���״̬
	static autotimer *Overcurrenttimer = NULL;
	if(avaliable_current > 200)
	{//20a������ac_current_size_20�жϣ�1.1��
		if((elec_condition.Current*0.0001) > (ac_current_size_20*avaliable_current/10))
		{//����ֵ�����붨ʱ������ʼ��ʱ
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(IS_TIMEOUT_MS(Overcurrenttimer,5000)){
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
				overcurrent = 1;
			}
		}
		else
		{
			if(Overcurrenttimer != NULL)
			{//�մӹ����ָ�
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
			}
			overcurrent = 0;
		}	
	}
	else
	{//20a������ac_current_size_0�ж� +2a
		if((elec_condition.Current*0.0001) > (ac_current_size_0+(avaliable_current/10L)))
		{//����ֵ�����붨ʱ������ʼ��ʱ
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(IS_TIMEOUT_MS(Overcurrenttimer,5000)){
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
				overcurrent = 1;
			}
		}
		else
		{
			if(Overcurrenttimer != NULL)
			{//�մӹ����ָ�
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
			}
			overcurrent = 0;
		}	
	}
}
//����ܽᣬ�����energy_charged��time_charged
void Charge_Summary(void){
	//�ܽ᱾�γ��
	energy_charged = elec_condition.Energy - energy_start_time;
	time_charged = RTC_GetCounter()/60;
	
	printf("#����¼����ϵ�ʱ�����������%01.1fKWh\r\n",energy_charged*0.0001);
	printf("#����¼����ϵ�ʱ�����ʱ��Ϊ%u����\r\n",time_charged);
}
//������񣬸�������������
void Charge_Task(void){
	static autotimer *stoppingtimer = NULL;//ֹͣ��ʱ��
	static autotimer *connecttimer = NULL;//�Ӵ���ճ����ʱ��
	switch(charge_state){
	case charge_state_idle:
		{
			if(cp_state == 12){
				//״̬1�¼�⵽12v��ѹ��ͦ�����ģ�ɶ�����ø�
				S1_Switch_Off();//s1�л���12v
				chargestart = 0;//����δ����״̬
				if(outvol == 1&& connecttimer == NULL){
					connecttimer = obtainTimer(0);//ȡһ����ʱ��������ʱ��0
				}
				else if(outvol == 1 && connecttimer != NULL){
					if(IS_TIMEOUT_MS(connecttimer,2000))
					{//��2s��ѹ��û�е���ȥ���Ǵ���״̬
						returnTimer(connecttimer);//��ʱ��û����
						connecttimer = NULL;
						printf("#��ȫ�¼�����⵽�Ӵ�������޷��Ͽ�\r\n");
						charge_state = charge_state_err;//����0
					}
				}
				else if(outvol == 0 && connecttimer == NULL){
					//0������״̬�����������ʱ��
				}
				else if(outvol == 0 && connecttimer != NULL){
					returnTimer(connecttimer);//��ʱ��û����
					connecttimer = NULL;
				}
			}
			else if(cp_state == 9){
				//״̬1�¼�⵽9v��ѹ����ζ�Ų�ǹ
				printf("#����¼�����⵽��ǹ\r\n");
				if (connecttimer != NULL){
					returnTimer(connecttimer);//��ʱ��û����
					connecttimer = NULL;
				}
				chargegun = 1;//���ò�ǹ
				charge_state = charge_state_insert_gun;
			}
			else if(cp_state == 6){
			}
			else{
				//charge_state = charge_state_err;//����0
			}
			break;
		}
	case charge_state_insert_gun:
		{
			//״̬2�¼�⵽12v��ѹ����ζ�Ű�ǹ
			if(cp_state == 12){
				printf("#����¼�����⵽��ǹ\r\n");
				S1_Switch_Off();//s1�л���12v
				chargegun = 0;//���ð�ǹ
				rfidlock = 1;//����ʱ����
				if(ble_pre_lock){
					//������Ԥλ����
					ble_pre_lock = 0;
					blelock = 1;
				}
				charge_state = charge_state_idle;
			}
			//״̬2��,�Լ����ͨ������ô��ʼ���pwm
			else if(cp_state == 9){
				//lock����״̬����ô��pwm����������
				if(lock == 0 && !(emgstate) && chargestart == 0 && !(meter_err))
				{
					printf("#����¼����Լ�������׼���������\r\n");
					printf("#����¼����û�����ʱѡ�����Ϊ%01.1fA\r\n",avaliable_current/10.0);
					switch(usr_sel_limit_mode){
						case 0://0���û�ѡ������
						{
							printf("#����¼����û�����ʱ���ò������ֹͣ����\r\n");	
							break;
						}
						case 1://1���û�ѡ������ʱ��
						{
							printf("#����¼����û�����ʱ���ó��ʱ�䳤��Ϊ%dH%dM\r\n",time_limit/60,time_limit%60);	
							break;
						}
						case 2://2���û�ѡ����������
						{
							printf("#����¼����û�����ʱ������������Ϊ%dkWh\r\n",energy_limit);	
							break;
						}
					}		
					S1_Switch_On();//s1�л���pwm
					charge_state = charge_state_gun_pwm;
					chargestart = 1;//�����Ѿ���������ǹ�����̣����γ��������ٴ�����
					stop_reason = 0;//���²�ǹ�ˣ�����ֹͣԭ��
				}
				else{
					if(chargestart == 1){
						//��������pwm֮����һ�λص����ֻ�п����ǳ����ɣ�ִ�жϿ�֮������	
						
					}
					//ʲô���������ó��ӵ�
				}
			}
			else if(cp_state == 6){
				//״̬2��,����ȶ�6v,��������,��Ҫ����
				//charge_state = charge_state_err;//����0
			}
			else{
				//charge_state = charge_state_err;//����0
			}
			break;
		}
	case charge_state_gun_pwm:
		{
			//״̬2'�¼�⵽12v��ѹ����ζ�Ű�ǹ
			if(cp_state == 12){
				printf("#����¼�����⵽��PWM����ڼ��ǹ\r\n");
				chargegun = 0;//���ð�ǹ
				charge_state = charge_state_nogun_pwm;
			}
			else if(cp_state == 9){
				//״̬2'�¼�⵽9v��ѹ����ζ��һ������
			}
			//״̬2'�¼�⵽6v��ѹ����ζ��ev׼����
			else if(cp_state == 6){
				//lock����״̬����ô��pwm����������
				if(lock == 0&&!(emgstate))
				{
					printf("#����¼����ܵ��豸�Լ�ͨ��������������������\r\n");
					energy_start_time = elec_condition.Energy;//���µ�ǰ�ĵ�����
					printf("#����¼���׼���������ʱ��ϵͳ��¼������%01.1fKWh\r\n",elec_condition.Energy*0.0001);
					RTC_Set(0);//����rtcʱ��Ϊ0
					printf("#����¼���׼��������磬ʱ������\r\n");
					
					outputen = 1;//���õ������
					charge_state = charge_state_charging;
				}
				else{
					//ʲô���������ó��ӵ�
				}
			}
			else
			{
				//charge_state = charge_state_err;//����0
			}
			break;
		}
	case charge_state_charging:
		{
			//״̬3'�¼�⵽12v��ѹ����ζ�Ű�ǹ
			if(cp_state == 12){
				printf("#����¼�����⵽�ڳ���ڼ��ǹ�����棩\r\n");
				stop_reason = 4;//����ڼ�ֱ�Ӱ�ǹ
				chargegun = 0;//���ð�ǹ
				outputen = 0;//�رյ�����������ܴ��أ�
				charge_state = charge_state_nogun_pwm;//ת��1'
			}
			//״̬3'�¼�⵽9v��ѹ����ζ�ų����Լ������Ͽ�
			else if(cp_state == 9){
				stop_reason = 3;//�������Ͽ�
				printf("#����¼�����⵽�ڳ���ڼ䣬�ܵ������ϵ�\r\n");
				outputen = 0;//�رյ�����������ܴ��أ�ȡ���ڳ����Ƿ񽵵͵�����
				
				//�ܽ᱾�γ��
				Charge_Summary();
				
				charge_state = charge_state_gun_pwm;//ת��2'
			}
			//״̬3'�¼�⵽6v��ѹ�������
			else if(cp_state == 6){
				//��ʼ�����û�ѡ��Ķϵ�����
				u8 user_stop = 0;
				switch(usr_sel_limit_mode){
					case 0://0���û�ѡ������
					{	
						//�����Ƶ�����£�����ڼ�ѭ�������κβ���
						break;
					}
					case 1://1���û�ѡ������ʱ��
					{
						if (RTC_GetCounter()/60 >= time_limit){
							printf("#����¼�����⵽�ڳ���ڼ䣬�ﵽ�û����Ƶ�ʱ��\r\n");
							stop_reason = 9;//������ʱ����
							user_stop = 1;
						}
						break;
					}
					case 2://2���û�ѡ����������
					{
						if (((elec_condition.Energy - energy_start_time)*0.0001 >= energy_limit*0.1)){
							printf("#����¼�����⵽�ڳ���ڼ䣬�ﵽ�û����Ƶ�����\r\n");
							stop_reason = 10;//������������
							user_stop = 1;
						}
						break;
					}
				}

				//��ʼ����ϵ�����
				if(overvoltage||overcurrent||emgstate||user_stop||meter_err){//�ﵽ�ϵ�����
					if(stoppingtimer == NULL)
					{
						stoppingtimer = obtainTimer(0);//ȡһ����ʱ��������ʱ��0
					}
					else
					{
						setTimer(stoppingtimer,0);//��ʱ��������ʱ��0
					}
					if(overvoltage)stop_reason = 1;//��ѹ
					if(overcurrent)stop_reason = 2;//����
					if(emgstate&&emgstop)stop_reason = 5;//Ӧ������
					if(emgstate&&overtemp)stop_reason = 6;//����
					if(meter_err)stop_reason = 8;//��������
					printf("#����¼�������ϵ���������������ϵ�\r\n");
					
					//�ܽ᱾�γ��
					Charge_Summary();
					
					S1_Switch_Off();//s1�л���12v
					charge_state = charge_state_stopping;//ת��3
				}
			}
			else
			{
				//charge_state = charge_state_err;//����0
			}
			break;
		}
	case charge_state_stopping:
		{	
			if(cp_state == 12){
			}
			//ģʽ3ʱ��⵽9v��Ҳ����ev�Ͽ�׼����
			else if(cp_state == 9){
				printf("#����¼�������ϵ��ڼ䣬�ܵ������ϵ�ȷ��\r\n");
				if(stoppingtimer != NULL){
					returnTimer(stoppingtimer);//��ʱ��û����
					stoppingtimer = NULL;
				}
				outputen = 0;//�رյ�����������ܴ��أ�ȡ���ڳ����Ƿ񽵵͵�����
				charge_state = charge_state_insert_gun;//ת��2
			}
			else if(cp_state == 6){
			//ģʽ3ʱ��⵽6v��Ҳ����ev���ڽ���
				if(IS_TIMEOUT_MS(stoppingtimer,3000)){
					//����3��
					printf("#����¼�������ϵ�������ܵ�˳�ʱ��Ӧ���������ǿ�ƶϿ�\r\n");
					if(stoppingtimer != NULL){
						returnTimer(stoppingtimer);//��ʱ��û����
						stoppingtimer = NULL;
					}
					outputen = 0;//�رյ���������ܿ��ܴ��أ�ȡ���ڳ����Ƿ񽵵͵�����
					charge_state = charge_state_insert_gun;//ת��2
				}
			}
			else
			{
				//charge_state = charge_state_err;//����0
			}
			break;
		}
	case charge_state_nogun_pwm:
		{
			//״̬1'״̬��⵽12v�������ζ��ǹȷ�϶Ͽ��ˣ��ر�pwm
			if(cp_state == 12){
				printf("#����¼�����ǹpwmģʽ��⵽��ǹ\r\n");
				S1_Switch_Off();//s1�л���12v
				rfidlock = 1;//����ʱ����
				charge_state = charge_state_idle;
			}
			//״̬1'״̬��⵽9v�������ζ���ֲ������
			if(cp_state == 9){
				printf("#����¼�����ǹpwmģʽ��⵽��ǹ\r\n");
				charge_state = charge_state_gun_pwm;
			}
			if(cp_state == 6){
			}	
			else
			{
				//charge_state = charge_state_err;//����0
			}
			break;
		}
	case charge_state_err:
		{
			printf("#����¼���״̬0�����������쳣��\r\n");
			rfidlock = 1;//����
			S1_Switch_Off();//s1�л���12v
			outputen = 0;//�رյ�����������ܴ��أ�ȡ���ڳ����Ƿ񽵵͵�����
		}

	}
	
}

//���������ж��߼�
void Lock_Task(void){
	//in1 22 ����
	//in2 21 ����
	//in3 20 ��ʱ����
	if(blelock){
		if(BLE_UNLOCK_GPIO_IN){blelock = 0;}//�����У�������Ч
	}
	else{
		if(BLE_LOCK_GPIO_IN){
			if(chargegun){
				//���ǹ������ֹ����
				printf("#��Ȩ�¼�����ǹ�ڼ�����������Ԥλ���ȴ����ǹ�˳�\r\n");
				ble_pre_lock = 1;//����Ԥλ
			}
			else{
				blelock = 1;//�����У���δ��ǹ����������Ч
			}
			
		}
	}
	
	if(rfidlock)
		if(RFID_UNLOCK_GPIO_IN){rfidlock = 0;}//�����У�������Ч
		
	if(lock != !((!blelock)|(!rfidlock)))
	{
		lock = !((!blelock)|(!rfidlock));
		lock?printf("#��Ȩ�¼���ϵͳ����\r\n"):printf("#��Ȩ�¼���ϵͳ����\r\n");
	}
}

void Update_Charge_Current_Task(void){
	Set_Charge_Current(usr_set_current);
}

//���ش��ڴ���״̬��
void Print_State_Task(void){
	PrintString1("^");
	WriteData1((uint8_t *)&statemech,4);
	WriteData1((uint8_t *)&stop_reason,1);
	WriteData1((uint8_t *)&avaliable_current,2);
	PrintString1("&\r\n");
//	PrintString1("$");
//	WriteData1((uint8_t *)&(elec_condition.Voltage),4);
//	WriteData1((uint8_t *)&(elec_condition.Current),4);
//	WriteData1((uint8_t *)&(elec_condition.ActivePower),4);
//	WriteData1((uint8_t *)&(elec_condition.Energy),4);
//	PrintString1("&\r\n");
}

void Get_Temp_Task(void){
	static autotimer * temptimer = NULL;
	static u8 step = 0;
	static u8 errtimes = 0;
	u8 i;
	u32 temp;
	if (temptimer == NULL)temptimer = obtainTimer(0);
	switch(step){
		case 0:{
			//0������Ҫ�����ɼ�
			for(i=0;i<3;i++){
				DS18B20_Start(DsGuid[i].guid.u64Bits);
			}
			setTimer(temptimer,0);
			step++;
			break;
		}
		case 1:{
			if(IS_TIMEOUT_MS(temptimer,1000)){
				//ʱ�䵽��
				for(i=0;i<3;i++){
					temp = DS18B20_Get_Temp_After_Start(DsGuid[i].guid.u64Bits);
					if (temp == 0 || temp == 850){
						//�������������������
						printf("#����ϵͳ���������������������β���\r\n");
						errtimes++;
					}
					else{
						temp_list[i] = temp;
						//printf("#����ϵͳ����������%d\r\n",temp);
					}
					if(errtimes >= 3){
						errtimes = 0;
						temp_list[0] = 10240;
					}
				}
				step = 0;
			}
			break;
		}
	}
}

void main_task(void){
	static uint32_t mainloopcounter = 0;
	static autotimer * mainlooptimer = NULL;
	if (mainlooptimer == NULL)mainlooptimer = obtainTimer(0);
	
	IM1281B_Update_Task();						//���±�ͷ����
	Elec_Vol_Task();							//ʵ�ֵ�ѹ�жϣ���Ҫ����
	Elec_Curr_Task();							//ʵ�ֵ����жϣ���Ҫ����
	Get_Temp_Task();							//��������
	Lock_Task();								//�����ź��ж��߼�
	Get_CP_Vol_Task();							//cp��ѹ�ɼ�
	HW_EMG_Task();								//�ж�Ӧ�������豸״̬
	Charge_Task();								//�������߼�
	Update_Charge_Current_Task();				//�����û����õ���ֵ
	Out_Task();									//�ж�����߼�������ִ�����(�͹��£���ͣ�йأ�
	//Print_State_Task();//���״̬��
	Ui_Task();
	LED_Task();
	//delay_ms(20);
	
	if(S1_Switch_State && meter_err){
		S1_Switch_Off();//s1�л���12v
		stop_reason = 8;//��������
		charge_state = charge_state_stopping;//ת��3
		printf("METER ERR!\r\n");
	}
	mainloopcounter++;
	if(IS_TIMEOUT_MS(mainlooptimer,60000)){
		printf("#����ϵͳ���ں�����Ƶ�ʣ�%d hz\r\n",mainloopcounter/60);
		mainloopcounter = 0;
		setTimer(mainlooptimer,0);
	}
}
