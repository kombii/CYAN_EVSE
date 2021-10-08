#include "cp_signal.h"
#include "adc.h"

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
	//�ۼ�10��
	adcval = (adcval*476L)/intnalref1v9;
	
	//�ⲿ5v��ѹ���ڲ�У׼��(119/intnalref1v9)*1024(���㵽0.01v)
	//1218560/intnalref1v9
	//��ѹ�������4������׼��ѹ 121856/intnalref1v9
	//(4*(1218560/intnalref1v9))/1024
	//476/intnalref1v9
	
	adcval += 900L;
	//���������ܳԵ���0.6v�����˶�����Ҳ�Ե���0.6v
	cp_voltage = adcval;
	Conclude_CP_State();
}