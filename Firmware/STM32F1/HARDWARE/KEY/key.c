#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
#include "BspTime.h"

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==1||KEY1==1||KEY2==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==1)return KEY0_PRES;
		else if(KEY1==1)return KEY1_PRES;
		else if(KEY2==1)return KEY2_PRES;
	}else if(KEY0==0&&KEY1==0&&KEY2==0)key_up=1; 	    
 	return 0;// �ް�������
}

u16 KEY_SELA(u8 clean)
{	 
	static autotimer * sel1timer = NULL;
	static u16 selaswiztime = 0;
	
	if(sel1timer == NULL)sel1timer = obtainTimer(0);

	if (clean) {
		setTimer(sel1timer,0);
		selaswiztime = 0;
		return 0;
	}
	
	u8 timeswiz = getTimer(sel1timer) - selaswiztime;
	u8 keystate = KEY0;
	
	if(keystate == 1 && selaswiztime == 0){
		//��һ�ΰ��£����浱ǰʱ��
		selaswiztime = timeswiz;
	} else if (keystate == 1 && timeswiz > 10){
		if (keystate == 1){
			return getTimer(sel1timer);
		}
	} else if (keystate == 1 && timeswiz <= 10){
		//10ms��ʲô��������Ҫ�ȴ�ȷ��
	} else if(keystate == 0){//̧��
		setTimer(sel1timer,0);
		selaswiztime = 0;
		return 0;// �ް�������
	}
	return getTimer(sel1timer);// �ް�������
}

u16 KEY_SELB(u8 clean)
{	 
	static autotimer * sel2timer = NULL;
	static u16 selbswiztime = 0;
	
	if(sel2timer == NULL)sel2timer = obtainTimer(0);

	if (clean) {
		setTimer(sel2timer,0);
		selbswiztime = 0;
		return 0;
	}
	
	u8 timeswiz = getTimer(sel2timer) - selbswiztime;
	u8 keystate = KEY1;
	
	if(keystate == 1 && selbswiztime == 0){
		//��һ�ΰ��£����浱ǰʱ��
		selbswiztime = timeswiz;
	} else if (keystate == 1 && timeswiz > 10){
		if (keystate == 1){
			return getTimer(sel2timer);
		}
	} else if (keystate == 1 && timeswiz <= 10){
		//10ms��ʲô��������Ҫ�ȴ�ȷ��
	} else if(keystate == 0){//̧��
		setTimer(sel2timer,0);
		selbswiztime = 0;
		return 0;// �ް�������
	}
	return getTimer(sel2timer);// �ް�������
}
