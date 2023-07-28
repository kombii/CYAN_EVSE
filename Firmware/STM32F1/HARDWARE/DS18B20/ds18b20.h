#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   

//CNFy[1:0]-MODEy[1:0]

//CNFy[1:0]: 
//In input mode (MODE[1:0]=00):
//00: Analog mode
//01: Floating input (reset state)
//10: Input with pull-up / pull-down
//11: Reserved
//In output mode (MODE[1:0] >?00):
//00: General purpose output push-pull
//01: General purpose output Open-drain
//10: Alternate function output Push-pull
//11: Alternate function output Open-drain

//MODEy[1:0]: Port x mode bits (y= 0 .. 7)
//These bits are written by software to configure the corresponding I/O port.
//Refer to Table 20: Port bit configuration table.
//00: Input mode (reset state)
//01: Output mode, max speed 10 MHz.
//10: Output mode, max speed 2 MHz.
//11: Output mode, max speed 50 MHz.

//IO方向设置
#define DS18B20_IO_IN()  {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=4<<12;}//01 00 浮空输入
#define DS18B20_IO_OUT() {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=3<<12;}//00 11 50M输出
////IO操作函数											   
#define	DS18B20_DQ_OUT PBout(3)
#define	DS18B20_DQ_IN  PBin(3)
   	
u8 DS18B20_Init(void);//初始化DS18B20
short DS18B20_Get_Temp_Once(uint64_t deviceserial);//获取温度
short DS18B20_Get_Temp_After_Start(uint64_t deviceserial);//获取单次
void DS18B20_Start(uint64_t deviceserial);//开始温度转换
void DS18B20_Write_Bit(u8 bit);//写入一个bit
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);//读出一个字节
u8 DS18B20_Read_Bit(void);//读出一个位
u8 DS18B20_Read_2Bit(void);//读出2个位
u8 DS18B20_Check(void);//检测是否存在DS18B20
void DS18B20_Rst(void);//复位DS18B20    
#endif















