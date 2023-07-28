#include "ds18b20.h"
#include "delay.h"	

//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); 	//SET PG11 OUTPUT
    DS18B20_DQ_OUT=0; 	//����DQ
    delay_us(750);    	//����750us
    DS18B20_DQ_OUT=1; 	//DQ=1 
	delay_us(15);     	//15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();	//SET PG11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 	 
{
    u8 data;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT
    DS18B20_DQ_OUT=0; 
	delay_us(2);
    DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();	//SET PG11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}
//��DS18B20��ȡ2��λ
//����ֵ��00/01/10/11(0,1,2,3)
u8 DS18B20_Read_2Bit(void)
{	
	u8 temp = DS18B20_Read_Bit();
	temp = (temp << 1) | DS18B20_Read_Bit();
	return temp;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)     
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//дһ��bit��DS18B20
//bit��Ҫд���bit
void DS18B20_Write_Bit(u8 bit)     
 {             
	DS18B20_IO_OUT();	//SET PG11 OUTPUT;
	if (bit) 
	{
		DS18B20_DQ_OUT=0;	// Write 1
		delay_us(2);                            
		DS18B20_DQ_OUT=1;
		delay_us(60);             
	}
	else 
	{
		DS18B20_DQ_OUT=0;	// Write 0
		delay_us(60);             
		DS18B20_DQ_OUT=1;
		delay_us(2);                          
	}
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;	// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;	// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}

//д��ַ������ѡ��Ŀ������
void DS18B20_Write_UUID(uint64_t deviceserial)     
{             
    u8 j;
    for (j=0;j<8;j++)
	{
		DS18B20_Write_Byte((u8)((deviceserial>>(j*8)) & 0xFF));
	}
}

//��ʼ�¶�ת��
void DS18B20_Start(uint64_t deviceserial) 
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();
	if(deviceserial){
		DS18B20_Write_Byte(0x55);	// sel rom
		DS18B20_Write_UUID(deviceserial);
		DS18B20_Write_Byte(0x44);	// convert
	}
	else{
	    DS18B20_Write_Byte(0xcc);	// skip rom
		DS18B20_Write_Byte(0x44);	// convert
	}
} 

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTG��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				//PORTG.11 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_3);    //���1

	DS18B20_Rst();

	return DS18B20_Check();
}  
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp_Once(uint64_t deviceserial)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start(deviceserial);    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
	if(deviceserial){
		DS18B20_Write_Byte(0x55);	// ƥ��ROM 
		DS18B20_Write_UUID(deviceserial);
		DS18B20_Write_Byte(0xbe);	// ���Ĵ���    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}
	else{
		DS18B20_Write_Byte(0xcc);	// skip rom
		DS18B20_Write_Byte(0xbe);	// ���Ĵ���    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//�¶�Ϊ��  
    }else temp=1;				//�¶�Ϊ��	  	  
    tem=TH; 					//��ø߰�λ
    tem<<=8;    
    tem+=TL;					//��õװ�λ
    tem=(float)tem*0.625;		//ת��     
	if(temp)return tem; 		//�����¶�ֵ
	else return -tem;    
}

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp_After_Start(uint64_t deviceserial)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Rst();
    DS18B20_Check();	 
	if(deviceserial){
		DS18B20_Write_Byte(0x55);	// ƥ��ROM 
		DS18B20_Write_UUID(deviceserial);
		DS18B20_Write_Byte(0xbe);	// ���Ĵ���    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}
	else{
		DS18B20_Write_Byte(0xcc);	// skip rom
		DS18B20_Write_Byte(0xbe);	// ���Ĵ���    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//�¶�Ϊ��  
    }else temp=1;				//�¶�Ϊ��	  	  
    tem=TH; 					//��ø߰�λ
    tem<<=8;    
    tem+=TL;					//��õװ�λ
    tem=(float)tem*0.625;		//ת��     
	if(temp)return tem; 		//�����¶�ֵ
	else return -tem;    
}

 
