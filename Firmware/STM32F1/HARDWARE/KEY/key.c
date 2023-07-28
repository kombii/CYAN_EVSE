#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
#include "BspTime.h"

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==1||KEY1==1||KEY2==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==1)return KEY0_PRES;
		else if(KEY1==1)return KEY1_PRES;
		else if(KEY2==1)return KEY2_PRES;
	}else if(KEY0==0&&KEY1==0&&KEY2==0)key_up=1; 	    
 	return 0;// 无按键按下
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
		//第一次按下，保存当前时间
		selaswiztime = timeswiz;
	} else if (keystate == 1 && timeswiz > 10){
		if (keystate == 1){
			return getTimer(sel1timer);
		}
	} else if (keystate == 1 && timeswiz <= 10){
		//10ms内什么都不做，要等待确认
	} else if(keystate == 0){//抬起
		setTimer(sel1timer,0);
		selaswiztime = 0;
		return 0;// 无按键按下
	}
	return getTimer(sel1timer);// 无按键按下
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
		//第一次按下，保存当前时间
		selbswiztime = timeswiz;
	} else if (keystate == 1 && timeswiz > 10){
		if (keystate == 1){
			return getTimer(sel2timer);
		}
	} else if (keystate == 1 && timeswiz <= 10){
		//10ms内什么都不做，要等待确认
	} else if(keystate == 0){//抬起
		setTimer(sel2timer,0);
		selbswiztime = 0;
		return 0;// 无按键按下
	}
	return getTimer(sel2timer);// 无按键按下
}
