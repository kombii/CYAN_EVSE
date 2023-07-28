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
	
	Tx_Buffer[0] = Read_ID; //模块的 ID 号，默认 ID 为 0x01
	Tx_Buffer[1] = 0x03;
	Tx_Buffer[2] = 0x00;
	Tx_Buffer[3] = 0x48;
	Tx_Buffer[4] = 0x00;
	Tx_Buffer[5] = 0x08;
	crcnow.word16 = chkcrc(Tx_Buffer, 6);
	Tx_Buffer[6] = crcnow.byte[1]; // CRC 效验低字节在前
	Tx_Buffer[7] = crcnow.byte[0];
	
	for (uint8_t t = 0; t < 8; t++){
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET){} //等待发送
		USART_SendData(USART1, Tx_Buffer[t]); //向串口2发送数据
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET){} //等待发送
	}
}

uint8_t im1281b_analysis_data(void)
{
    union crcdata
    {
        uint16_t word16;
        uint8_t byte[2];
    } crcnow;
	
    if (USART1_RX_STA & 0x8000) //接收完成
    {
        if (USART1_RX_BUF[0] == Read_ID) //确认 ID 正确
        {
            crcnow.word16 = chkcrc(USART1_RX_BUF,(USART1_RX_STA & 0x3fff) - 2);
            if ((crcnow.byte[0] == USART1_RX_BUF[(USART1_RX_STA & 0x3fff) - 1]) && (crcnow.byte[1] == USART1_RX_BUF[(USART1_RX_STA & 0x3fff) - 2]))
            //确认 CRC 校验正确
            {
                elec_condition.Voltage = (((uint32_t)(USART1_RX_BUF[3])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[4])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[5])) 	<< 8) | USART1_RX_BUF[6];
                elec_condition.Current = (((uint32_t)(USART1_RX_BUF[7])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[8])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[9])) 	<< 8) | USART1_RX_BUF[10];
                elec_condition.ActivePower 	= (((uint32_t)(USART1_RX_BUF[11])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[12])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[13])) 	<< 8) | USART1_RX_BUF[14];
                elec_condition.Energy 	= (((uint32_t)(USART1_RX_BUF[15])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[16])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[17])) 	<< 8) | USART1_RX_BUF[18];
                elec_condition.PowerFactor 		= (((uint32_t)(USART1_RX_BUF[19])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[20])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[21])) 	<< 8) | USART1_RX_BUF[22];
                //Im_CO2_data 	= (((uint32_t)(USART1_RX_BUF[23])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[24])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[25])) 	<< 8) | USART1_RX_BUF[26];
				elec_condition.Temperature 	= (((uint32_t)(USART1_RX_BUF[27])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[28])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[29])) 	<< 8) | USART1_RX_BUF[30];
				elec_condition.Freq 	= (((uint32_t)(USART1_RX_BUF[31])) 	<< 24) | (((uint32_t)(USART1_RX_BUF[32])) 	<< 16) | (((uint32_t)(USART1_RX_BUF[33])) 	<< 8) | USART1_RX_BUF[34];
				//运行到这里说明数据采集正常
				//清空缓冲区
				USART1_RX_STA = 0;
				return 1;
            }
			//运行到这里说明crc校验失败
			//清空缓冲区
			USART1_RX_STA = 0;
			return 2;
        }
		//运行到这里说明id不正确
		//清空缓冲区
		USART1_RX_STA = 0;
		return 3;
    }
	//运行到这里说明包没准备好
	return 0;
}

//1281更新任务，需要循环调用
//会自动修改状态量IM1281B_TIMEOUT和IM1281B_COMMERR
void IM1281B_Update(uint8_t setstep){
	static uint8_t step = 0;
	if(setstep != 255){
		step=setstep;
	}
	
	if(IM1281B_timer == NULL){
		IM1281B_timer = obtainTimer(0);
	}
	
	switch(step){
		case 0:{//0状态是需要询问
			if(IS_TIMEOUT_MS(IM1281B_timer,50)){
				//printf("发送,time=%d\r\n",getTimer(IM1281B_timer));
				im1281b_ask();//询问发送
				setTimer(IM1281B_timer,0);//归零定时器
				step = 1;
			}
			break;
		}
		case 1:{//1状态是询问结束，等待回应
			if(IS_TIMEOUT_MS(IM1281B_timer,500)){
				//printf("回应超时,time=%d\r\n",getTimer(IM1281B_timer));
				IM1281B_TIMEOUT = 1;//超时设置标置位
				//printf("超时！\r\n");
				step = 0;//并且设置下一次循环开始
				setTimer(IM1281B_timer,0);//归零定时器
				USART1_RX_STA=0;//归零串口缓存
			}
			else{
				uint8_t res = im1281b_analysis_data();//获取一次数据
				switch(res){
					case 0:{//数据没有拿到
						//什么都不做
						break;
					}
					case 1:{//数据采集正常
						//printf("收到回应,time=%d\r\n",getTimer(IM1281B_timer));
						IM1281B_TIMEOUT = 0;
						IM1281B_COMMERR = 0;
						USART1_RX_STA = 0;
						step = 0;//并且设置下一次循环开始
						setTimer(IM1281B_timer,0);//归零定时器
						break;
					}
					case 2:{//crc校验失败
						//printf("crc校验失败\r\n");
						IM1281B_COMMERR = 1;
						USART1_RX_STA = 0;
						step = 0;//并且设置下一次循环开始
						setTimer(IM1281B_timer,0);//归零定时器
						break;
					}
					case 3:{//id不正确
						//printf("id不正确\r\n");
						IM1281B_COMMERR = 1;
						USART1_RX_STA = 0;
						step = 0;//并且设置下一次循环开始
						setTimer(IM1281B_timer,0);//归零定时器
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
				PrintString1("#仪表系统：和仪表系统通信超时!\r\n");
				meterfreeze = 1;
				if(IM1281B_COMMERR){
					//1,1
					PrintString1("#仪表系统：通信出现错误!\r\n");
					meterfreeze = 1;
					}
			}
			else {
			//0,1 
				PrintString1("#仪表系统：通信出现错误!\r\n");
				meterfreeze = 1;
			}
		}
		else{
			//如果冻结了
			if(waittimer == NULL){
				//PrintString1("timer!\r\n");
				waittimer = obtainTimer(0);
			}
			else if(IS_TIMEOUT_MS(waittimer,500))
			{
				//超时了
				//PrintString1("timer 0!\r\n");
				returnTimer(waittimer);
				waittimer = NULL;
				meterfreeze = 0;
				IM1281B_Update(0);
				meter_err_time++;
			}
			else{
				//超时内
				if(IM1281B_TIMEOUT == 0 && IM1281B_COMMERR == 0){
					//检测到了错误恢复
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
