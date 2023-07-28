#include "ui.h"
#include "state_mech.h"
#include "IM1281B.h"
#include "BspTime.h"
#include "key.h"
#include "eeprom.h"
#include <string.h>
#include <stdio.h>

volatile uint8_t menu_layer; //�˵��ȼ���0Ϊ�رգ�1Ϊ�����棬2Ϊ��һ���˵���3Ϊ�ڶ����˵�
volatile uint8_t menu_layer_1; //��һ���˵���λ�ñ��� 0Ϊ���Ե���0.1��λ�ĵ����˵���1ΪԤ������˵���2Ϊʱ������ҳ�棬3Ϊ��������ҳ�棬4Ϊ��������ҳ��

volatile uint16_t menu_set_current = 60;
//���õ�������С6a��60�������32a��320��
volatile uint16_t menu_select_current = 0;
//ѡ�������ֻ�м�����λ
//0  1  2   3   4   5
//6��8��12��16��24��32 ��
volatile uint16_t menu_set_hour = 0;
//����Сʱ
volatile uint16_t menu_set_min = 0;
//���÷���
volatile uint16_t menu_set_energy = 0;
//���������޶�



autotimer * menutimer = NULL;

//uiӲ�����ֳ�ʼ��
void Ui_Hw_Init(void){
	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
	OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
}

//ui����ϵͳ��ʼ��
void Ui_System_Init(void){
	Ui_Hw_Init();
	menu_layer = 1;//�ص�������
	menu_layer_1 = 1;//��һ���˵�Ϊ��λ�����˵�
	
	menu_set_current = usr_set_current;
	menu_select_current = 0;
	
	menu_set_hour = time_limit/60;
	menu_set_min = time_limit%60;
	
	menu_set_energy = energy_limit;
}

//��ʾ32���ַ���,һ����ʾ����
//line:��ʾ��Ŀ���У�1����2
//*chr:�ַ�����ʼ��ַ 
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_Show32in2line(u8 line,u8 *chr,u8 mode){
	//printf("show 2line in line %d:%s",line,chr);
	u8 len = strlen((char*)chr);
	u8 x = (128-(len*16))/2;
	while(*chr!=0x00)//��0x0Ϊֹ
	{
		OLED_ShowChar(x,(line-1)*32,*chr,32,mode);
		x+=16;
		chr++;
	}
}

//��ʾ32���ַ���,һ����ʾ1��
//line:��ʾ��Ŀ���У�1����2
//*chr:�ַ�����ʼ��ַ 
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_Show32in1line(u8 *chr,u8 mode){
	u8 len = strlen((char*)chr);
	u8 x = (128-(len*16))/2;
	while(*chr!=0x00)//��0x0Ϊֹ
	{
		OLED_ShowChar(x,16,*chr,32,mode);
		x+=16;
		chr++;
	}
}


//OK��ť�Ĵ������
void key_ok_handler(void){
	//printf("#�����¼�����ťOK����\r\n");
	alarm_on=1;
	if(menu_layer == 0){
		//����״̬
		menu_layer = 1;
	}
	else if(menu_layer == 1){
		//��һ��״̬����ת��һ���˵�
		menu_layer = 2;
		if(menutimer != NULL){
			returnTimer(menutimer);
			menutimer = NULL;
		}
	}
	else if(menu_layer == 2){
		//��һ���˵�����ͬ���õĲ˵����
		if(menu_layer_1 == 4){
			//4�������Ƴ���޶���ò�����
			EEPROM_Set_Usr_Limit_Mode(0);
			usr_sel_limit_mode = 0;
			menu_layer = 1;//�ص�������
		}
		else{
			//��һ���˵�������ڶ���
			menu_layer = 3;
		}
	}
	else if(menu_layer == 3){
		//�����������ݲ�ͬ�˵����治ͬ����
		switch(menu_layer_1){
			case 0:{//0�ǵ����ɵ�
				EEPROM_Set_Current(menu_set_current);
				printf("#�˵��¼����û�ѡ�����Ƶ���������Ϊ%0.1fA\r\n",menu_set_current/10.0);
				break;
			}
			case 1:{//1��ѡ�����
				switch(menu_select_current){
					//0  1  2   3   4   5
					//6��8��12��16��24��32 ��
					case 0:{
						EEPROM_Set_Current(60);
						printf("#�˵��¼����û�ѡ�����Ƶ���������Ϊ6A\r\n");
						break;
					}
					case 1:{
						EEPROM_Set_Current(80);
						printf("#�˵��¼����û�ѡ�����Ƶ���������Ϊ8A\r\n");
						break;
					}
					case 2:{
						EEPROM_Set_Current(120);
						printf("#�˵��¼����û�ѡ�����Ƶ���������Ϊ12A\r\n");
						break;
					}
					case 3:{
						EEPROM_Set_Current(160);
						printf("#�˵��¼����û�ѡ�����Ƶ���������Ϊ16A\r\n");
						break;
					}
					case 4:{
						EEPROM_Set_Current(240);
						printf("#�˵��¼����û�ѡ�����Ƶ���������Ϊ24A\r\n");
						break;
					}
					case 5:{
						EEPROM_Set_Current(320);
						printf("#�˵��¼����û�ѡ�����Ƶ���������Ϊ32A\r\n");
						break;
					}
				}
				break;
			}
			case 2:{//2��ʱ������
				EEPROM_Set_Usr_Limit_Mode(1);//1������ʱ��
				EEPROM_Set_Time_Limit(menu_set_min + menu_set_hour*60);
				printf("#�˵��¼����û�ѡ������ʱ�䣬����Ϊ%dH%dM\r\n",menu_set_hour,menu_set_min);
				break;
			}
			case 3:{//3����������
				EEPROM_Set_Usr_Limit_Mode(2);//2����������
				EEPROM_Set_Energy_Limit(menu_set_energy);
				printf("#�˵��¼����û�ѡ����������������Ϊ%dkWh\r\n",menu_set_energy);
				break;
			}
		}
		Get_Datas_From_EEPROM();
		menu_layer = 1;
	}	
}

//KEYA�Ĵ������
void key_a_handler(void){
	//printf("#�����¼�����ť+����\r\n");
	alarm_on=1;
	if(menu_layer == 0){
		//����״̬
		menu_layer = 1;
	}
	else if(menu_layer == 2){
		//�ڶ����˵����л�����
		if (menu_layer_1 == 4){
			menu_layer_1 = 0;
		} else {
			menu_layer_1 += 1;
		}
	}
	else if(menu_layer == 3){
		switch(menu_layer_1){
			case 0:{//0�ǵ����ɵ�
				if (menu_set_current >= 320){
				} else {
					menu_set_current += 1;
				}
				break;
			}
			case 1:{//1��ѡ�����
				if (menu_select_current >= 5){
					menu_select_current = 0;
				} else {
					menu_select_current += 1;
				}
				break;
			}
			case 2:{//2��ʱ������
				if(menu_set_min >= 55){
					if(menu_set_hour == 99){
						//ʲô������
					}
					else{
						menu_set_min = 0;
						menu_set_hour += 1;
					}
				}
				else{
					menu_set_min += 5;
				}
				break;
			}
			case 3:{//3����������
				if (menu_set_energy >= 5000){
				} else {
					menu_set_energy += 1;
				}
				break;
			}
		}
	}
}

//KEYB�Ĵ������
void key_b_handler(void){
	//printf("#�����¼�����ť-����\r\n");
	alarm_on=1;
	if(menu_layer == 0){
		//����״̬
		menu_layer = 1;
	}
	else if(menu_layer == 2){
		//�ڶ����˵����л�����
		if (menu_layer_1 == 0){
			menu_layer_1 = 4;
		} else {
			menu_layer_1 -= 1;
		}
	}
	else if(menu_layer == 3){
		switch(menu_layer_1){
			case 0:{//0�ǵ����ɵ�
				if (menu_set_current <= 60){
				} else {
					menu_set_current -= 1;
				}
				break;
			}
			case 1:{//1��ѡ�����
				if (menu_select_current == 0){
					menu_select_current = 5;
				} else {
					menu_select_current -= 1;
				}
				break;
			}
			case 2:{//2��ʱ������
				if(menu_set_min == 0){
					if(menu_set_hour == 0){
						//ʲô������
					}
					else{
						menu_set_min = 55;
						menu_set_hour -= 1;
					}
				}
				else{
					menu_set_min -= 5;
				}
				break;
			}
			case 3:{//3����������
				if (menu_set_energy <= 0){
				} else {
					menu_set_energy -= 1;
				}
				break;
			}
		}
	}
}

//ɨ�谴������
void MAINKEYTASK(void){
	//������־λ������ǰ�����Ѿ��ǳ����ڼ���Ҫ��������
	static u8 selalongpress = 0;
	static u8 selblongpress = 0;
	u8 key = 0;
	//---------------------------------------------sela��������
	if(selalongpress == 1)
	{
		//��������1000ms����λ�����볤������
		u8 ret = KEY_SELA(0);
		if(ret == 0){
			//����̧��
			KEY_SELA(1);
			selalongpress = 0;
		}
		else if (ret > 30){
			//������������100ms
			KEY_SELA(1);
			key_a_handler();
		}

	}
	else if (KEY_SELA(0) > 1000)
	{
		//��������1000ms�ҳ�������λδ��λ
		KEY_SELA(1);
		selalongpress = 1;
	}
	//---------------------------------------------selb��������
	//��������
	if(selblongpress == 1)
	{
		//��������1000ms����λ�����볤������
		u8 ret = KEY_SELB(0);
		if(ret == 0)
		{
			//����̧��
			KEY_SELB(1);
			selblongpress = 0;
		}
		else if (ret > 30)
		{
			//������������100ms
			KEY_SELB(1);
			key_b_handler();
		}

	}
	else if (KEY_SELB(0) > 1000)
	{
		KEY_SELB(1);
		selblongpress = 1;
	}
	//---------------------------------------------�̰���
	key = KEY_Scan(0);	//�õ���ֵ
	if(key)
	{
		switch (key)
		{
		case KEY0_PRES:
			key_a_handler();
			break;
		case KEY1_PRES:
			key_b_handler();
			break;
		case KEY2_PRES:
			key_ok_handler();
			break;
		}
	}
}

//��ʾ����״̬����Ϣ���������ú͵�ǰ����ģʽ)(��һ��)
void Show_Idle_State(){
	uint8_t hours = 0;
	uint8_t mins = 0;
	char strings[9] = "";
	memset(strings,0,sizeof(strings));
	sprintf(strings,"%.1f A",usr_set_current/10.0);
	OLED_Show32in2line(1,(u8*)strings,1);
	memset(strings,0,sizeof(strings));
	switch(usr_sel_limit_mode){
		case 0:
			sprintf(strings,"NOLIMIT");	
			break;
		case 1:
			hours = time_limit / 60;
			mins = time_limit % 60;
			sprintf(strings,"%dH %dM", hours, mins);	
			break;
		case 2:
			sprintf(strings,"%.1fkWh",energy_limit/10.0);
			break;
		default:
			sprintf(strings,"LIMITERR");	
			break;
	}
	OLED_Show32in2line(2,(u8*)strings,1);
	//sprintf(strings,"%.1f A",usr_set_current/10.0);
}
//��ʾ�˵�ҳ�棬����ȫ�ֱ�����λ����ȷ��
void Show_Menus(void){
	char strings[9] = "";
	switch(menu_layer){
		case 0:{//0������ʾ
			//ʲô�����ɣ��Ȱ������̿�������
			break;
		}
		case 1:{//1���������棬ֻ��ʾĿ������
			if(menutimer == NULL){
				menutimer = obtainTimer(0);//����һ����ʱ��
			}
			if(IS_TIMEOUT_MS(menutimer,1000)){
				//10���ʱ
				menu_layer = 0;//�������״̬
				OLED_Clear();//�����Ļ
				returnTimer(menutimer);
				menutimer = NULL;
			}
			else{
				Show_Idle_State();
			}
			break;
		}
		case 2:{//2Ϊ����ҳ�棬Ҫ����menu_layer_1����ֵ
			switch(menu_layer_1){
				case 0:{//0�ǵ����ɵ�
					memset(strings,0,sizeof(strings));
					sprintf(strings,"MAXIMUM");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"CURRENT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 1:{//1�ǵ�λ����
					memset(strings,0,sizeof(strings));
					sprintf(strings,"PREFER");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"CURRENT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 2:{//2��ʱ������
					memset(strings,0,sizeof(strings));
					sprintf(strings,"TIME");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"LIMIT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 3:{//3����������
					memset(strings,0,sizeof(strings));
					sprintf(strings,"ENERGY");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"LIMIT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 4:{//4�ǲ�����
					memset(strings,0,sizeof(strings));
					sprintf(strings,"NO");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"LIMIT");
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
			}
			break;
		}
		case 3:{//3Ϊ���õ����飬Ҫ����menu_layer_1����ֵ
			switch(menu_layer_1){
				case 0:{//0�ǵ����ɵ�
					memset(strings,0,sizeof(strings));
					sprintf(strings,"MAX CUR");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"%.1f A", menu_set_current/10.0);
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 1:{//1�ǵ�λ����
					memset(strings,0,sizeof(strings));
					sprintf(strings,"MAX CUR");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					switch(menu_select_current){
						case 0:{
							sprintf(strings,"6.0 A");
							break;
						}
						case 1:{
							sprintf(strings,"8.0 A");
							break;
						}
						case 2:{
							sprintf(strings,"12.0 A");
							break;
						}
						case 3:{
							sprintf(strings,"16.0 A");
							break;
						}
						case 4:{
							sprintf(strings,"24.0 A");
							break;
						}
						case 5:{
							sprintf(strings,"32.0 A");
							break;
						}
					}
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 2:{//2��ʱ������
					memset(strings,0,sizeof(strings));
					sprintf(strings,"TIME");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"%02dH %02dM", menu_set_hour, menu_set_min);
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
				case 3:{//3����������
					memset(strings,0,sizeof(strings));
					sprintf(strings,"ENERGY");
					OLED_Show32in2line(1,(u8*)strings,1);
					memset(strings,0,sizeof(strings));
					sprintf(strings,"%.1fkWh", menu_set_energy/10.0);
					OLED_Show32in2line(2,(u8*)strings,1);
					break;
				}
			}
			break;
		}
	}
}
//��ʾ�����ҳ��
void Show_Summary(void)
{
	char strings[9] = "";
	memset(strings,0,sizeof(strings));
	sprintf(strings,"%.1fkWh",energy_charged*0.0001);
	OLED_Show32in2line(1,(u8*)strings,1);
	memset(strings,0,sizeof(strings));
	sprintf(strings,"%02dH %02dM", time_charged/60, time_charged%60);
	OLED_Show32in2line(2,(u8*)strings,1);
	
}
//��ǹ���ui����
void Ui_Gun(){
	switch(charge_state){
	case charge_state_idle:
		{
			break;
		}
	case charge_state_insert_gun:
		{
			if(chargestart == 1){
				//��ʱ�Ѿ����й�������״̬����ʵ���ǳ����ɺ󷵻ز�ǹ״̬
				Show_Summary();
			}
			else{
				OLED_Show32in2line(1,(u8*)"WAIT",1);
				OLED_Show32in2line(2,(u8*)"UNLOCK",1);
			}
			break;
		}
	case charge_state_gun_pwm:
		{
			if(stop_reason == 3){
				//��ʱ�ǳ��������Ͽ��ģ��Ͳ�����ʾwait
				Show_Summary();
			}
			else{
				OLED_Show32in2line(1,(u8*)"WAIT",1);
				OLED_Show32in2line(2,(u8*)"EV",1);
			}
			break;
		}
	case charge_state_charging:
		{
			break;
		}
	case charge_state_stopping:
		{	
			OLED_Show32in2line(1,(u8*)"STOP",1);
			break;
		}
	case charge_state_nogun_pwm:
		{
			break;
		}
	case charge_state_err:
		{
		    OLED_ShowString(0,0,(u8*)"ERR!",32,1);
			break;
		}

	}
}

void Ui_Task(void){
	OLED_ClearBuffer();
	if (chargegun){
		Ui_Gun();
	} else {
		Show_Menus();
		MAINKEYTASK();
	}
	OLED_Refresh();
}
