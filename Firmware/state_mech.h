#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "TYPESS.H"
//状态机们-----------------------------------------

static bit emgstate;//紧急状态输入 1紧急 0正常
static bit outputen;//输出允许 1允许 0不允许
static bit lock;//外部锁定系统 1锁定 0解锁

static bit output;//输出 1输出 0关闭输出


static bit overtemp;//过温 1紧急 0正常
static bit overvoltage;//过压 1紧急 0正常
static bit undervoltage;//欠压 1紧急 0正常
static bit overcurrent;//过流 1紧急 0正常


static uint32_t cp_voltage;//cp线电压，以0.01v单位

static uint8_t charge_state;//充电状态

static uint8_t cp_state;//cp线档位
//0 6 9 12 255

#endif