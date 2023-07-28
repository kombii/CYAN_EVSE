#include "watch_dog.h"
#include "gpio.h"

void wdt_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_5;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//15 wdt
}
void wdt_feed(void){
	P15 = !P15;
}
void wdt_enable(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_5;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//15 wdt
}
void wdt_diable(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_5;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//15 wdt
}