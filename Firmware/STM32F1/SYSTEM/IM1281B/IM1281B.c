#include "IM1281B.h"
#include "usart.h"
#include "BspTime.h"
#include "delay.h"
#include "state_mech.h"
#include <stdbool.h>

#define Read_ID 0x01

uint8_t Tx_Buffer[8];

ELEC_TYPE_S elec_condition ={0,0,0,0,0,0,0};

uint8_t IM1281B_TIMEOUT = 0;
uint8_t IM1281B_COMMERR = 0;
autotimer * IM1281B_timer = NULL;

uint16_t calccrc(uint8_t crcbuf, uint16_t crc)
{
    uint8_t i;
    uint8_t chk;
    crc = crc ^ crcbuf;
    for (i = 0; i < 8; i++)
    {
        chk = (uint8_t)(crc & 1);
        crc = crc >> 1;
        crc = crc & 0x7fff;
        if (chk == 1)
            crc = crc ^ 0xa001;
        crc = crc & 0xffff;
    }
    return crc;
}

uint16_t chkcrc(uint8_t *buf, uint8_t len)
{
    uint8_t hi, lo;
    uint16_t i;
    uint16_t crc;
    crc = 0xFFFF;
    for (i = 0; i < len; i++)
    {
        crc = calccrc(*buf, crc);
        buf++;
    }
    hi = (uint8_t)(crc % 256);
    lo = (uint8_t)(crc / 256);
    crc = (((uint16_t)(hi)) << 8) | lo;
    return crc;
}

void im1281b_ask(void)
{
    union crcdata
    {
        uint16_t word16;
        uint8_t byte[2];
    } crcnow;
	
	Tx_Buffer[0] = Read_ID; //ģ��� ID �ţ�Ĭ�� ID Ϊ 0x01
	Tx_Buffer[1] = 0x03;
	Tx_Buffer[2] = 0x00;
	Tx_Buffer[3] = 0x48;
	Tx_Buffer[4] = 0x00;
	Tx_Buffer[5] = 0x08;
	crcnow.word16 = chkcrc(Tx_Buffer, 6);
	Tx_Buffer[6] = crcnow.byte[1]; // CRC Ч����ֽ���ǰ
	Tx_Buffer[7] = crcnow.byte[0];
	
	for (uint8_t t = 0; t < 8; t++){
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET){} //�ȴ�����
		USART_SendData(USART1, Tx_Buffer[t]); //�򴮿�2��������
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET){} //�ȴ�����
	}
}

uint8_t im1281b_analysis_data(void)
{
    union crcdata
    {
        uint16_t word16;
        uint8_t byte[2];
    } crcnow;
	
    if (USART1_RX_STA & 0x8000) //�������
    {
        if (USART1_RX_BUF[0] == Read_ID) //ȷ�� ID ��ȷ
        {
            crcnow.word16 = chkcrc(USART1_RX_BUF,(USART1_RX_STA & 0x3fff) - 2);
            if ((crcnow.byte[0] == USART1_RX_BUF[(USART1_RX_STA & 0x3fff) - 1]) && (crcnow.byte[1] == USART1_RX_BUF[(USART1_RX_STA & 0x3fff) - 2]))
            //ȷ�� CRC У����ȷ
            {
                elec_condition.Voltage = (((uint32_t)(USART1_RX_BUF[3])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[4])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[5])) 	<< 8) | USART1_RX_BUF[6];
                elec_condition.Current = (((uint32_t)(USART1_RX_BUF[7])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[8])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[9])) 	<< 8) | USART1_RX_BUF[10];
                elec_condition.ActivePower 	= (((uint32_t)(USART1_RX_BUF[11])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[12])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[13])) 	<< 8) | USART1_RX_BUF[14];
                elec_condition.Energy 	= (((uint32_t)(USART1_RX_BUF[15])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[16])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[17])) 	<< 8) | USART1_RX_BUF[18];
                elec_condition.PowerFactor 		= (((uint32_t)(USART1_RX_BUF[19])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[20])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[21])) 	<< 8) | USART1_RX_BUF[22];
                //Im_CO2_data 	= (((uint32_t)(USART1_RX_BUF[23])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[24])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[25])) 	<< 8) | USART1_RX_BUF[26];
				elec_condition.Temperature 	= (((uint32_t)(USART1_RX_BUF[27])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[28])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[29])) 	<< 8) | USART1_RX_BUF[30];
				elec_condition.Freq 	= (((uint32_t)(USART1_RX_BUF[31])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[32])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[33])) 	<< 8) | USART1_RX_BUF[34];
				//���е�����˵�����ݲɼ�����
				//��ջ�����
				USART1_RX_STA = 0;
				return 1;
            }
			//���е�����˵��crcУ��ʧ��
			//��ջ�����
			USART1_RX_STA = 0;
			return 2;
        }
		//���е�����˵��id����ȷ
		//��ջ�����
		USART1_RX_STA = 0;
		return 3;
    }
	//���е�����˵����û׼����
	return 0;
}

//1281����������Ҫѭ������
//���Զ��޸�״̬��IM1281B_TIMEOUT��IM1281B_COMMERR
void IM1281B_Update(uint8_t setstep){
	static uint8_t step = 0;
	if(setstep != 255){
		step=setstep;
	}
	
	if(IM1281B_timer == NULL){
		IM1281B_timer = obtainTimer(0);
	}
	
	switch(step){
		case 0:{//0״̬����Ҫѯ��
			if(IS_TIMEOUT_MS(IM1281B_timer,50)){
				//printf("����,time=%d\r\n",getTimer(IM1281B_timer));
				im1281b_ask();//ѯ�ʷ���
				setTimer(IM1281B_timer,0);//���㶨ʱ��
				step = 1;
			}
			break;
		}
		case 1:{//1״̬��ѯ�ʽ������ȴ���Ӧ
			if(IS_TIMEOUT_MS(IM1281B_timer,500)){
				//printf("��Ӧ��ʱ,time=%d\r\n",getTimer(IM1281B_timer));
				IM1281B_TIMEOUT = 1;//��ʱ���ñ���λ
				//printf("��ʱ��\r\n");
				step = 0;//����������һ��ѭ����ʼ
				setTimer(IM1281B_timer,0);//���㶨ʱ��
				USART1_RX_STA=0;//���㴮�ڻ���
			}
			else{
				uint8_t res = im1281b_analysis_data();//��ȡһ������
				switch(res){
					case 0:{//����û���õ�
						//ʲô������
						break;
					}
					case 1:{//���ݲɼ�����
						//printf("�յ���Ӧ,time=%d\r\n",getTimer(IM1281B_timer));
						IM1281B_TIMEOUT = 0;
						IM1281B_COMMERR = 0;
						USART1_RX_STA = 0;
						step = 0;//����������һ��ѭ����ʼ
						setTimer(IM1281B_timer,0);//���㶨ʱ��
						break;
					}
					case 2:{//crcУ��ʧ��
						//printf("crcУ��ʧ��\r\n");
						IM1281B_COMMERR = 1;
						USART1_RX_STA = 0;
						step = 0;//����������һ��ѭ����ʼ
						setTimer(IM1281B_timer,0);//���㶨ʱ��
						break;
					}
					case 3:{//id����ȷ
						//printf("id����ȷ\r\n");
						IM1281B_COMMERR = 1;
						USART1_RX_STA = 0;
						step = 0;//����������һ��ѭ����ʼ
						setTimer(IM1281B_timer,0);//���㶨ʱ��
						break;
					}
				}
			}
		break;
		}
	}
}

void IM1281B_Update_Task(void){
		static bool meterfreeze = 0;
		static uint8_t meter_err_time = 0;
		static autotimer * waittimer = NULL;
		
		IM1281B_Update(255);
		if(meterfreeze == 0){
			if(IM1281B_TIMEOUT == 0 && IM1281B_COMMERR == 0){
				meter_err_time = 0;
				meter_err = 0;
				//PrintString1("IM1281B_ok!\r\n");
//				PrintString1("$");
//				WriteData1((uint8_t *)&(elec_condition.Voltage),4);
//				WriteData1((uint8_t *)&(elec_condition.Current),4);
//				WriteData1((uint8_t *)&(elec_condition.ActivePower),4);
//				WriteData1((uint8_t *)&(elec_condition.Energy),4);
//				PrintString1("&\r\n");
			}
			else if(IM1281B_TIMEOUT){
				//1,1 1,0
				PrintString1("#�Ǳ�ϵͳ�����Ǳ�ϵͳͨ�ų�ʱ!\r\n");
				meterfreeze = 1;
				if(IM1281B_COMMERR){
					//1,1
					PrintString1("#�Ǳ�ϵͳ��ͨ�ų��ִ���!\r\n");
					meterfreeze = 1;
					}
			}
			else {
			//0,1 
				PrintString1("#�Ǳ�ϵͳ��ͨ�ų��ִ���!\r\n");
				meterfreeze = 1;
			}
		}
		else{
			//���������
			if(waittimer == NULL){
				//PrintString1("timer!\r\n");
				waittimer = obtainTimer(0);
			}
			else if(IS_TIMEOUT_MS(waittimer,500))
			{
				//��ʱ��
				//PrintString1("timer 0!\r\n");
				returnTimer(waittimer);
				waittimer = NULL;
				meterfreeze = 0;
				IM1281B_Update(0);
				meter_err_time++;
			}
			else{
				//��ʱ��
				if(IM1281B_TIMEOUT == 0 && IM1281B_COMMERR == 0){
					//��⵽�˴���ָ�
					meter_err_time = 0;
					meter_err = 0;
					if(waittimer != NULL){
						returnTimer(waittimer);
						waittimer = NULL;
						meterfreeze = 0;
					}
				}
			}
		}
		if(meter_err_time>3){
			meter_err = 1;
		}
}
