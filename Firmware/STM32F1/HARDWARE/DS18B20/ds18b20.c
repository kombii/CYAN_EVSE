#include "ds18b20.h"
#include "delay.h"	

//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); 	//SET PG11 OUTPUT
    DS18B20_DQ_OUT=0; 	//拉低DQ
    delay_us(750);    	//拉低750us
    DS18B20_DQ_OUT=1; 	//DQ=1 
	delay_us(15);     	//15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
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
//从DS18B20读取一个位
//返回值：1/0
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
//从DS18B20读取2个位
//返回值：00/01/10/11(0,1,2,3)
u8 DS18B20_Read_2Bit(void)
{	
	u8 temp = DS18B20_Read_Bit();
	temp = (temp << 1) | DS18B20_Read_Bit();
	return temp;
}
//从DS18B20读取一个字节
//返回值：读到的数据
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
//写一个bit到DS18B20
//bit：要写入的bit
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
//写一个字节到DS18B20
//dat：要写入的字节
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

//写地址，用于选择目标器件
void DS18B20_Write_UUID(uint64_t deviceserial)     
{             
    u8 j;
    for (j=0;j<8;j++)
	{
		DS18B20_Write_Byte((u8)((deviceserial>>(j*8)) & 0xFF));
	}
}

//开始温度转换
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

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PORTG口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				//PORTG.11 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_3);    //输出1

	DS18B20_Rst();

	return DS18B20_Check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp_Once(uint64_t deviceserial)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start(deviceserial);    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
	if(deviceserial){
		DS18B20_Write_Byte(0x55);	// 匹配ROM 
		DS18B20_Write_UUID(deviceserial);
		DS18B20_Write_Byte(0xbe);	// 读寄存器    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}
	else{
		DS18B20_Write_Byte(0xcc);	// skip rom
		DS18B20_Write_Byte(0xbe);	// 读寄存器    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//温度为负  
    }else temp=1;				//温度为正	  	  
    tem=TH; 					//获得高八位
    tem<<=8;    
    tem+=TL;					//获得底八位
    tem=(float)tem*0.625;		//转换     
	if(temp)return tem; 		//返回温度值
	else return -tem;    
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp_After_Start(uint64_t deviceserial)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Rst();
    DS18B20_Check();	 
	if(deviceserial){
		DS18B20_Write_Byte(0x55);	// 匹配ROM 
		DS18B20_Write_UUID(deviceserial);
		DS18B20_Write_Byte(0xbe);	// 读寄存器    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}
	else{
		DS18B20_Write_Byte(0xcc);	// skip rom
		DS18B20_Write_Byte(0xbe);	// 读寄存器    
		TL=DS18B20_Read_Byte(); 	// LSB   
		TH=DS18B20_Read_Byte(); 	// MSB  
	}	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//温度为负  
    }else temp=1;				//温度为正	  	  
    tem=TH; 					//获得高八位
    tem<<=8;    
    tem+=TL;					//获得底八位
    tem=(float)tem*0.625;		//转换     
	if(temp)return tem; 		//返回温度值
	else return -tem;    
}

 
