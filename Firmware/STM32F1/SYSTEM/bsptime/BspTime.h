#ifndef _DrvTime2_h_
#define _DrvTime2_h_
#include "sys.h"

//#define BSPTIME_USE_SYSTICK

#ifndef NULL
#define NULL        ((void *)0)
#endif
//定义可以同时使用的计时器的最多数量
#define TIMER_COUNT 25

// 定义一个结构体
// 这个结构体存放了基本的时钟信息
// 因为中断内会操作time，所以必须要有volatile，防止优化掉
typedef volatile struct {
    // 当前定时器的滴答数
    uint16_t time;
    // 是否繁忙
    uint8_t busy;
} autotimer;

// 定时器池
// 这个池管理了所有的定时器
extern autotimer bsptimers[];

// 实现一个判断超时的宏定义
//if用，判断是否超时
#define IS_TIMEOUT_MS(timer, count)    ((((autotimer*)timer)->time <= (count))?  0: 1)
//while用，判断是否在时间范围内
#define IS_INTIME_MS(timer, count)    ((((autotimer*)timer)->time <= (count))?  1: 0)

void init_bsptimer(void);
#ifdef BSPTIME_USE_SYSTICK
void delay_ms(uint16_t nms);
#endif
autotimer* obtainTimer(uint16_t start_value);
uint8_t setTimer(autotimer* timer,uint16_t start_value);
uint16_t getTimer(autotimer* timer);
void returnTimer(autotimer* timer);
void tickingTimer(void);

#endif
