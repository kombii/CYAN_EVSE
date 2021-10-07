#ifndef _DrvTime2_h_
#define _DrvTime2_h_
#include "TYPESS.H"

#ifndef NULL
#define NULL        ((void *)0)
#endif
//�������ͬʱʹ�õļ�ʱ�����������
#define TIMER_COUNT 10

// ����һ���ṹ��
// ����ṹ�����˻�����ʱ����Ϣ
typedef struct {
    // ��ǰ��ʱ���ĵδ���
    uint16_t time;
    // �Ƿ�æ
    uint8_t busy;
} autotimer;

// ��ʱ����
// ����ع��������еĶ�ʱ��
volatile autotimer bsptimers[];

// ʵ��һ���жϳ�ʱ�ĺ궨��
#define IS_TIMEOUT_1MS(timer, count)    ((((autotimer*)timer)->time <= (count))?  1: 0)

autotimer* obtainTimer(uint16_t start_value);
uint8_t setTimer(autotimer* timer,uint16_t start_value);
void returnTimer(autotimer* timer);
void tickingTimer();

#endif
