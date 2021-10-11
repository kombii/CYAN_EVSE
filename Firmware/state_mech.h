#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "TYPESS.H"
//状态机们-----------------------------------------

#define ac_voltage_max (260L)
#define ac_voltage_min (180L)

#define ac_current_size_20 (1.1)
#define ac_current_size_0 (5)

#define EPSINON (1e-6)

extern bit emgstate;//紧急状态输入 1紧急 0正常
extern bit outputen;//输出允许 1允许 0不允许
extern bit lock;//外部锁定系统 1锁定 0解锁

extern bit output;//输出 1输出 0关闭输出

extern bit overtemp;//过温 1紧急 0正常
extern bit overvoltage;//过压 1紧急 0正常
extern bit undervoltage;//欠压 1紧急 0正常
extern bit overcurrent;//过流 1紧急 0正常
extern bit emgstop;//紧急停止按钮 1紧急 0正常

extern uint32_t cp_voltage;//cp线电压，以0.01v单位
extern uint8_t charge_state;//充电状态
extern uint8_t cp_state;//cp线档位
//0 6 9 12 255
void State_Task_Init(void);
void main_tast(void);

#endif