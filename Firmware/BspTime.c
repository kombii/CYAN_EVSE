#include "BspTime.h"
volatile autotimer bsptimers[TIMER_COUNT]={0,0};
/*
* 获取一个空闲的定时器，这个定时器
* 1、会自动跑滴答
* 2、是空闲的
*/
autotimer* obtainTimer(uint16_t start_value) {
    uint8_t i;
    for (i = 0; i < TIMER_COUNT; i++) {
        if (bsptimers[i].busy == 0) {
            bsptimers[i].time = start_value;
            bsptimers[i].busy = 1;
			break;
        }
    }
    return &bsptimers[i];
}
/*
* 设置定时器，该操作会操作目标定时器，修改当前值
*/
uint8_t setTimer(autotimer* timer,uint16_t start_value) {
    if(timer->busy == 0)return 0;
    timer->time = start_value;
    return 1;
}
/*
* 归还定时器，该操作会自动释放定时器
* 并且对定时器归零
*/
void returnTimer(autotimer* timer) {
    timer->busy = 0;
    timer->time = 0;
}

/*
* 让所有被使用的定时器滴答一下
*/
void tickingTimer() {
    uint8_t i;
    for (i = 0; i < TIMER_COUNT; i++) {
        if (bsptimers[i].busy == 1) {
            bsptimers[i].time += 1;
        }
    }
}