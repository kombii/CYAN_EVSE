#ifndef _DrvTime2_h_
#define _DrvTime2_h_
#include "TYPESS.H"

#ifndef NULL
#define NULL        ((void *)0)
#endif
//定义可以同时使用的计时器的最多数量
#define TIMER_COUNT 10

// 定义一个结构体
// 这个结构体存放了基本的时钟信息
typedef struct {
    // 当前定时器的滴答数
    uint16_t time;
    // 是否繁忙
    uint8_t busy;
} autotimer;

// 定时器池
// 这个池管理了所有的定时器
volatile autotimer bsptimers[];

// 实现一个判断超时的宏定义
#define IS_TIMEOUT_1MS(timer, count)    ((((autotimer*)timer)->time <= (count))?  1: 0)

autotimer* obtainTimer(uint16_t start_value);
uint8_t setTimer(autotimer* timer,uint16_t start_value);
void returnTimer(autotimer* timer);
void tickingTimer();

#endif
