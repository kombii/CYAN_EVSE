#ifndef __RTC_H
#define __RTC_H
#include "sys.h"

u8 RTC_Init(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u32 RTC_Get(void);
void RTC_Set(u32 sec);
#endif


