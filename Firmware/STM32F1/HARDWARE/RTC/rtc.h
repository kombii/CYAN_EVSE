#ifndef __RTC_H
#define __RTC_H
#include "sys.h"

u8 RTC_Init(void);        //初始化RTC,返回0,失败;1,成功;
u32 RTC_Get(void);
void RTC_Set(u32 sec);
#endif


