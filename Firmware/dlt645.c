#include "dlt645.h"
#include "dlt645_private.h"
#include "uart.h"
#include "BspTime.h"
#include <string.h>

typedef struct {
#define HANDLE_LEN 30
    u8 buff[HANDLE_LEN];
    u16 len;
} HANDLE_TYPE_S;

RX_BUFF_TYPE cli_rx_buff; //����buff

ELEC_TYPE_S elec_condition ={0,0,0,0,0,0};

uint8_t meter_addr[6]={0x45,0x01,0x00,0x00,0x00,0x00};//����ַ
//DLT645��������ʵ��
int dlt645_send_msg(uint8_t *msg, int len)
{
    msg[0] = DL645_START_CODE;
    msg[DL645_ADDR_LEN + 1] = DL645_START_CODE;
    msg[len - 1] = DL645_STOP_CODE;
    msg[len - 2] = _crc(msg, len - 2);
	return WriteData2(msg,len);
}

//DLT645-2007 ���ݰ�����
uint32_t dlt645_2007_parsing_data(uint8_t *read_data)
{
	uint16_t i;
	uint32_t cmdcode;
	uint8_t code_buf[4];
    for (i = 0; i < 4; i++)
    {
		code_buf[i] = read_data[DL645_DATA_POS+i] - 0x33;
    }
	cmdcode = (uint32_t)code_buf[0]         | 
			  ((uint32_t)code_buf[1] << 8)  | 
			  ((uint32_t)code_buf[2] << 16) | 
			  ((uint32_t)code_buf[3] << 24);
	//printf("cmd:%lu\r\n",cmdcode);
    switch (cmdcode)
    {
    case DIC_0:
    {
        dlt645_data_parse_by_format_to_float(read_data + DL645_DATA_POS + 4,4,"XXXXXX.XX",elec_condition.Energy);
        break;
    }
    case DIC_2010100://A���ѹ
	{
        dlt645_data_parse_by_format_to_float(read_data + DL645_DATA_POS + 4,2,"XXX.X",elec_condition.Voltage);
        break;
    }
    case DIC_2020100://A�����
	{
        dlt645_data_parse_by_format_to_float(read_data + DL645_DATA_POS + 4,3,"XXX.XXX",elec_condition.Current);
        break;
    }
    case DIC_2030100://A���й�����
    {
        dlt645_data_parse_by_format_to_float(read_data + DL645_DATA_POS + 4,3,"XX.XXXX",elec_condition.ActivePower);
        break;
    }
	case DIC_2040100://A���޹�����
    {
        dlt645_data_parse_by_format_to_float(read_data + DL645_DATA_POS + 4,3,"XX.XXXX",elec_condition.ReactivePower);
        break;
    }
    case DIC_2060100://A�๦������
    {
        dlt645_data_parse_by_format_to_float(read_data + DL645_DATA_POS + 4,2,"X.XXX",elec_condition.PowerFactor);
        break;
    }
    case DIC_2800002://A��Ƶ��
    {
        dlt645_data_parse_by_format_to_float(read_data + DL645_DATA_POS + 4,2,"XX.XX",elec_condition.Freq);
        break;
    }
    default:
		cmdcode = DIC_FFFFFFF;
    break;
    }
    return cmdcode;
}


uint32_t handle_485_rx(RX_BUFF_TYPE *rx_buff)
{
    static HANDLE_TYPE_S Handle = {{0},0};
	static autotimer* handlefree = NULL;
	
	uint32_t thistimecmd = 0;
	if(handlefree == NULL){
		handlefree = obtainTimer(0);
	}
	
    /*  ---------------------------------------
    ��1�����ӻ�������������
    ---------------------------------------*/
    while (1) {
        if (Handle.len < HANDLE_LEN) {
            //���������еط�
            if(TRUE == QUEUE_OUT((*rx_buff), Handle.buff[Handle.len]))
            {
                //�������д������ݣ����ֽڣ����뻺����
                Handle.len++;
                setTimer(handlefree,0);
            }
            else {
                //�Ѿ������ж�������
				 if(!IS_TIMEOUT_1MS(handlefree, 100)) {
                    //��ʱ������������Ѿ����Ͽɻ��������ݣ�ֱ��������
					setTimer(handlefree,0);
					Handle.len=0;
                }
                break;
            }
        }
        else {
			break;
        }
    }
    /*  ---------------------------------------
    ��2������������������ݣ�Ѱ��Ŀ�����ݰ�
    ---------------------------------------*/
    if(
        (0x68 == Handle.buff[0]) && //��ͷ68
        (0x16 == Handle.buff[Handle.len - 1]) && //��β16
        (Handle.len>11)
    ) {
        //��ͷ��β��ã��� �� ����ȷ�����ݰ�����
        //��ʼ��ʽ�������ݰ�
        if((Handle.buff[DL645_LEN_POS] == Handle.len-12)) { //���ݳ�����֤
			if (dlt645_common_check(Handle.buff,Handle.len,meter_addr) >= 0)
			{
				thistimecmd = dlt645_2007_parsing_data(Handle.buff);
				PrintString1("645 PROTROL OK\r\n");
			}
			else
			{
				PrintString1("645 PROTROL ERROR\r\n");
			}				
			setTimer(handlefree,0);
		    Handle.len = 0;
        } 
    }
    if (Handle.len >= HANDLE_LEN) {
        //���������ˣ���ͷ��ʼ
        Handle.len = 0;
    }
	return thistimecmd;
}

//DLT645-2007 ���ݶ�ȡ
int Dlt645_Read_Data(uint32_t cmdcode)
{
	uint8_t i;
    uint8_t send_buf[DL645_2007_RD_CMD_LEN];
	uint8_t send_code[4] = {0};
	autotimer* timeout = obtainTimer(0);
	
	
    memset(send_buf, 0, sizeof(send_buf));

    memcpy(send_buf + 1, meter_addr, DL645_ADDR_LEN);

    send_buf[DL645_CONTROL_POS] = C_2007_CODE_RD;
    send_buf[DL645_LEN_POS] = 4;

    send_code[0] = (cmdcode & 0xff) + 0x33;
    send_code[1] = ((cmdcode >> 8) & 0xff) + 0x33;
    send_code[2] = ((cmdcode >> 16) & 0xff) + 0x33;
    send_code[3] = ((cmdcode >> 24) & 0xff) + 0x33;

    memcpy(send_buf + DL645_DATA_POS, send_code, 4);
	for(i=0;i<3;i++)
	{
		dlt645_send_msg(send_buf, DL645_2007_RD_CMD_LEN);
		while(COM2.B_TX_busy);//�ȴ��������
		setTimer(timeout,0);
		while(IS_TIMEOUT_1MS(timeout, 500)){
			if(handle_485_rx(&cli_rx_buff) == cmdcode) 
			{
				returnTimer(timeout);
				return 0;
			}

		}
	}
	returnTimer(timeout);
	return -1;
}
//�ն˳�ʼ��ʵ��
void Cli_Init(void)
{
	UART_config();
	QUEUE_INIT(cli_rx_buff);
    memset((uint8_t *)&cli_rx_buff, 0, sizeof(RX_BUFF_TYPE));
}


