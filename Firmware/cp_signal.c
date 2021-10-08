#include "cp_signal.h"
#include "adc.h"

//获取充电系的CP线路工作状态
void Conclude_CP_State(void){
#define v_0v 4500
#define v_6v 7500
#define v_9v 10500
#define v_12v 13500

if(cp_voltage < v_0v)								cp_state = 0;//电压0v(超低)档
else if(cp_voltage >= v_0v && cp_voltage < v_6v)	cp_state = 6;//电压6v档
else if(cp_voltage >= v_6v && cp_voltage < v_9v)	cp_state = 9;//电压9v档
else if(cp_voltage >= v_9v && cp_voltage < v_12v)	cp_state = 12;//电压12v档
else												cp_state = 255;//电压255v(超高)档

}
//从adc获取cp信号的电压
void get_cp_vol_task(void)
{
	uint32_t adcval = 0;
	uint32_t intnalref1v9=0;
	uint8_t i;
	
	for(i=0;i<3;i++){
		adcval += Get_ADCResult(15);
	}
	intnalref1v9 = adcval /= 3;
	adcval = 0;
	for(i=0;i<10;i++){
		adcval += Get_ADCResult(10);
	}
	//累计10次
	adcval = (adcval*476L)/intnalref1v9;
	
	//外部5v电压用内部校准：(119/intnalref1v9)*1024(换算到0.01v)
	//1218560/intnalref1v9
	//分压电阻比例4倍，基准电压 121856/intnalref1v9
	//(4*(1218560/intnalref1v9))/1024
	//476/intnalref1v9
	
	adcval += 900L;
	//整流二极管吃掉了0.6v，车端二极管也吃掉了0.6v
	cp_voltage = adcval;
	Conclude_CP_State();
}