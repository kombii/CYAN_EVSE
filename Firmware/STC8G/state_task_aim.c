#include "state_mech.h"
#include "gpio.h"
#include "dlt645.h"
#include "charge_current.h"
#include "BspTime.h"
#include "cp_signal.h"
#include "s1_switch.h"
#include "uart.h"

uint16_t bdata statemech = 0;

sbit emgstate			=statemech^0;
sbit outputen			=statemech^1;
sbit lock				=statemech^2;
sbit output				=statemech^3;

sbit sw_error			=statemech^4;
sbit overtemp			=statemech^5;
sbit overvoltage		=statemech^6;
sbit undervoltage		=statemech^7;

sbit overcurrent		=statemech^8;
sbit emgstop			=statemech^9;
sbit chargegun			=statemech^10;
sbit chargestart		=statemech^11;

sbit S1_Switch_State	=statemech^12;
sbit rfidlock			=statemech^13;
sbit blelock 			=statemech^14;
sbit outvol 			=statemech^15;

uint32_t cp_voltage = 0;//cp�ߵ�ѹ����0.01v��λ
uint8_t charge_state = 0;//���״̬
uint8_t cp_state = 0;//cp�ߵ�λ
uint8_t stop_reason = 0;//ֹͣԭ��
uint16_t avaliable_current = 0;//��ǰ����
//1;//��ѹ
//2;//����
//3;//�������Ͽ�
//4;//����ڼ�ֱ�Ӱ�ǹ
//5;//Ӧ������
//6;//����
//7;//�Ӵ����������
//255;//δ֪

void State_Task_Init(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//33 EMG������ 
	GPIO_InitStructure.Pin  = GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	//23 �����ѹ��� 22 21 20 �ⲿ�����ź�
	GPIO_InitStructure.Pin  = GPIO_Pin_6|GPIO_Pin_5;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//36 �¶ȴ��� 35 Ӧ��ֹͣ
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//34 �Ӵ������
	
	//GPIO_InitStructure.Pin  = GPIO_Pin_0;
	//GPIO_InitStructure.Mode = GPIO_OUT_PP;
	//GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	//20 ָʾ���
	P34 = 0;
	
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
}

//Ӳ����������ɼ�
void HW_EMG_Task(void){
	emgstate = !P33; 	//NOR���������Ч
	emgstop = P35; 		//��ż������� ����Ч
	overtemp = P36;		//��ż������� ����Ч
}
//���״̬
void Out_Task(void){
	if(	(outputen) 	 && 
		!(emgstate) && 
		!(emgstop) &&
	    !(overtemp)
	) output = 1;
	else output = 0;
	P34 = output;
	outvol = P23;
	PrintString1("^");
	WriteData1((uint8_t *)&statemech,2);
	WriteData1((uint8_t *)&stop_reason,1);
	WriteData1((uint8_t *)&avaliable_current,2);
	PrintString1("&\r\n");
}
//�ɼ���ѹ����
void Elec_Vol_Task(void){
	//�жϵ�ѹ״̬
	if((*(float *)elec_condition.Voltage) > ac_voltage_max)
	{
	overvoltage = 1;
	}
	else {
		overvoltage = 0;
		if ((*(float *)elec_condition.Voltage) < ac_voltage_min && !(((*(float *)elec_condition.Voltage)>=-EPSINON) && ((*(float *)elec_condition.Voltage)<=EPSINON)))
		{
		undervoltage = 1;
		}
		else 
		{
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
		if((*(float *)elec_condition.Current) > (ac_current_size_20*avaliable_current/10))
		{//����ֵ�����붨ʱ������ʼ��ʱ
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(!IS_TIMEOUT_1MS(Overcurrenttimer,5000)){
				returnTimer(Overcurrenttimer);
				Overcurrenttimer = NULL;
				overcurrent = 1;
			}
		}
		else
		{
			if(Overcurrenttimer != NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			overcurrent = 0;
		}	
	}
	else
	{//20a������ac_current_size_0�ж� +2a
		if((*(float *)elec_condition.Current) > (ac_current_size_0+(avaliable_current/10L)))
		{//����ֵ�����붨ʱ������ʼ��ʱ
			if(Overcurrenttimer == NULL)
			{
				Overcurrenttimer = obtainTimer(0);
			}
			if(!IS_TIMEOUT_1MS(Overcurrenttimer,5000)){
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
//������񣬸�������������
void Charge_task(void){
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
					if(!IS_TIMEOUT_1MS(connecttimer,2000))
					{//��2s��ѹ��û�е���ȥ���Ǵ���״̬
						returnTimer(connecttimer);//��ʱ��û����
						connecttimer = NULL;
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
				PrintString1("#gun insert\r\n");
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
				PrintString1("#gun pull out\r\n");
				S1_Switch_Off();//s1�л���12v
				chargegun = 0;//���ð�ǹ
				rfidlock = 1;//����ʱ����
				charge_state = charge_state_idle;
			}
			//״̬2��,�Լ����ͨ������ô��ʼ���pwm
			else if(cp_state == 9){
				//lock����״̬����ô��pwm����������
				if(lock == 0&&!(emgstate) && chargestart == 0)
				{
					PrintString1("#self test ok\r\n");
					S1_Switch_On();//s1�л���pwm
					charge_state = charge_state_gun_pwm;
					chargestart = 1;//�����Ѿ���������ǹ�����̣����γ��������ٴ�����
					stop_reason = 0;//���²�ǹ�ˣ�����ֹͣԭ��
				}
				else{
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
				PrintString1("#gun pull out(pwm)\r\n");
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
					PrintString1("#ev ok power on\r\n");
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
				PrintString1("#gun pull out(charging)\r\n");
				stop_reason = 4;//����ڼ�ֱ�Ӱ�ǹ
				chargegun = 0;//���ð�ǹ
				outputen = 0;//�رյ�����������ܴ��أ�
				charge_state = charge_state_nogun_pwm;//ת��1'
			}
			//״̬3'�¼�⵽9v��ѹ����ζ�ų����Լ������Ͽ�
			else if(cp_state == 9){
				stop_reason = 3;//�������Ͽ�
				PrintString1("#ev power out(charging)\r\n");
				outputen = 0;//�رյ�����������ܴ��أ�ȡ���ڳ����Ƿ񽵵͵�����
				charge_state = charge_state_gun_pwm;//ת��2'
			}
			//״̬3'�¼�⵽6v��ѹ�������
			else if(cp_state == 6){
				if(overvoltage||overcurrent||emgstate){//�ﵽ�ϵ�����
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
					PrintString1("#pripare stop(charging)\r\n");
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
				PrintString1("#ev power out(stopping)\r\n");
				returnTimer(stoppingtimer);//��ʱ��û����
				stoppingtimer = NULL;
				outputen = 0;//�رյ�����������ܴ��أ�ȡ���ڳ����Ƿ񽵵͵�����
				charge_state = charge_state_insert_gun;//ת��2
			}
			else if(cp_state == 6){
			//ģʽ3ʱ��⵽6v��Ҳ����ev���ڽ���
				if(!IS_TIMEOUT_1MS(stoppingtimer,3000)){
				//����3��
				PrintString1("#power out(stopping force)\r\n");
				returnTimer(stoppingtimer);//��ʱ��û����
				stoppingtimer = NULL;	
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
				PrintString1("#pwm off(nogun_pwm)\r\n");
				S1_Switch_Off();//s1�л���12v
				rfidlock = 1;//����ʱ����
				charge_state = charge_state_idle;
			}
			//״̬1'״̬��⵽9v�������ζ���ֲ������
			if(cp_state == 9){
				PrintString1("#gun return(nogun_pwm)\r\n");
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
			PrintString1("#smt err\r\n");
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
		if(P22 == 0)
		{
			blelock = 0;//�����У�������Ч
			//P20 = 0;
		}
	}
	else{
		if(P21 == 0)
		{
			blelock = 1;//�����У�������Ч
			//P20 = 1;
		}
	}
	if(rfidlock)
		if(P20 == 0)
		{
			rfidlock = 0;//�����У�������Ч
		}
	lock = !((!blelock)|(!rfidlock));
}

void main_task(void){
	Elec_Vol_Task();//ʵ�ֵ�ѹ�жϣ���Ҫ����
	Elec_Curr_Task();//ʵ�ֵ����жϣ���Ҫ����
	Lock_Task();//�����ź��ж��߼�
	Get_CP_Vol_Task();//cp��ѹ�ɼ�
	HW_EMG_Task();//�ж�Ӧ�������豸״̬
	Charge_task();//�������߼�
	Out_Task();//�ж�����߼�������ִ�����(�͹��£���ͣ�йأ�
}
