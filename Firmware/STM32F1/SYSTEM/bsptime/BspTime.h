#ifndef _DrvTime2_h_
#define _DrvTime2_h_
#include "sys.h"

//#define BSPTIME_USE_SYSTICK

#ifndef NULL
#define NULL        ((void *)0)
#endif
//�������ͬʱʹ�õļ�ʱ�����������
#define TIMER_COUNT 25

// ����һ���ṹ��
// ����ṹ�����˻�����ʱ����Ϣ
// ��Ϊ�ж��ڻ����time�����Ա���Ҫ��volatile����ֹ�Ż���
typedef volatile struct {
    // ��ǰ��ʱ���ĵδ���
    uint16_t time;
    // �Ƿ�æ
    uint8_t busy;
} autotimer;

// ��ʱ����
// ����ع��������еĶ�ʱ��
extern autotimer bsptimers[];

// ʵ��һ���жϳ�ʱ�ĺ궨��
//if�ã��ж��Ƿ�ʱ
#define IS_TIMEOUT_MS(timer, count)    ((((autotimer*)timer)->time <= (count))?  0: 1)
//while�ã��ж��Ƿ���ʱ�䷶Χ��
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
