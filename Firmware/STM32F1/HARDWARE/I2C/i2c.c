#include "i2c.h"

u16 timeout=100;
/*i2c�ĳ�ʼ��*/
void IIC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//GPIO������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//IIC������
	I2C_InitStruct.I2C_Ack=I2C_Ack_Disable;
	I2C_InitStruct.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed=400000;
	I2C_InitStruct.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1=0x55;
	I2C_Init(I2C1,&I2C_InitStruct);
	
	I2C_Cmd(I2C1,ENABLE);
}
 
/*д��һ���ֽ�*/
void HWIIC_WriteByte(u8 devaddr,u8 addr,u8 data)
{
	I2C_GenerateSTART(I2C1,ENABLE);//������ʼ�ź�
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))//���	EV5�¼�
	{
		  if((timeout--)==0)
			printf("EV5 Fail");
	}	timeout=100;
	
	I2C_Send7bitAddress(I2C1,devaddr,I2C_Direction_Transmitter);//����7λEEPROM��Ӳ����ַ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//���EV6�¼�
	{
		  if((timeout--)==0)
		{
			//printf("EV6 Fail");
			 break;
		  }
	}	timeout=100;
	
	I2C_SendData(I2C1,addr);//���Ͳ������ڴ��ַ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�¼�
	{
		  if((timeout--)==0)
			printf("EV8 Fail");
	}	timeout=100;	
	
	I2C_SendData(I2C1,data);//Ҫд������ݣ�һ���ֽڣ�
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�¼�
	{
		  if((timeout--)==0)
			printf("EV8 Fail");
	}	timeout=100;
	I2C_GenerateSTOP(I2C1,ENABLE);//���ͽ����ź�
}
