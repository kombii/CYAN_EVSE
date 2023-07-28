#include "cp_signal.h"
#include "state_mech.h"

void CP_Init(void){
	Adc_Init();
}

//��ȡ���ϵ��CP��·����״̬
void Conclude_CP_State(void){
#define v_0v 4500
#define v_6v 7500
#define v_9v 10500
#define v_12v 13500

if(cp_voltage < v_0v)								cp_state = 0;//��ѹ0v(����)��
else if(cp_voltage >= v_0v && cp_voltage < v_6v)	cp_state = 6;//��ѹ6v��
else if(cp_voltage >= v_6v && cp_voltage < v_9v)	cp_state = 9;//��ѹ9v��
else if(cp_voltage >= v_9v && cp_voltage < v_12v)	cp_state = 12;//��ѹ12v��
else												cp_state = 255;//��ѹ255v(����)��
}
//��adc��ȡcp�źŵĵ�ѹ
void Get_CP_Vol_Task(void)
{
	uint32_t adcval = 0;
	uint32_t intnalref1v2=0;
	
	intnalref1v2 = Get_Adc_Average(1);
	adcval = Get_Adc_Average(0)*5815.384615384615/intnalref1v2;
	//�ڲ�1v2У׼��1msb����ĵ�ѹ��120/intnalref1v2(���㵽0.01v)
	//��ѹ�������4.846153846153846����ÿmsb��ѹ120/intnalref1v2
	//4.846153846153846*120/intnalref1v2
	//581.5384615384615/intnalref1v2
	
	adcval += 900L;
	//���������ܳԵ���0.6v�����˶�����Ҳ�Ե���0.6v
	cp_voltage = adcval;
	//printf("cp vol:%d\r\n",adcval);
	Conclude_CP_State();
}

