#include "cp_signal.h"
#include "state_mech.h"

void CP_Init(void){
	Adc_Init();
}

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
void Get_CP_Vol_Task(void)
{
	uint32_t adcval = 0;
	uint32_t intnalref1v2=0;
	
	intnalref1v2 = Get_Adc_Average(1);
	adcval = Get_Adc_Average(0)*5815.384615384615/intnalref1v2;
	//内部1v2校准后1msb代表的电压：120/intnalref1v2(换算到0.01v)
	//分压电阻比例4.846153846153846倍，每msb电压120/intnalref1v2
	//4.846153846153846*120/intnalref1v2
	//581.5384615384615/intnalref1v2
	
	adcval += 900L;
	//整流二极管吃掉了0.6v，车端二极管也吃掉了0.6v
	cp_voltage = adcval;
	//printf("cp vol:%d\r\n",adcval);
	Conclude_CP_State();
}

