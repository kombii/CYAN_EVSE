#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "TYPESS.H"
//状态机们-----------------------------------------

#define ac_voltage_max (260.00f)
#define ac_voltage_min (180.00f)

#define ac_current_size_20 (1.1f)
#define ac_current_size_0 (2.0f)

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

extern bit chargegun;//插枪状态 1插入 0拔出
extern bit chargestart;//充电启动过的状态 1启动过了 0没有

extern bit S1_Switch_State;//S1开关状态
extern bit rfidlock;//rfid锁状态
extern bit blelock;//ble锁状态
extern bit outvol;//输出电压状态

extern uint32_t cp_voltage;//cp线电压，以0.01v单位
extern uint8_t charge_state;//充电状态
extern uint8_t cp_state;//cp线档位
extern uint16_t avaliable_current;//当前电流
//0 6 9 12 255

typedef enum _charge_states{
	charge_state_idle,//状态1
	charge_state_insert_gun,//状态2
	charge_state_gun_pwm,//状态2‘
	charge_state_charging,//状态3’
	charge_state_stopping,//状态3
	charge_state_nogun_pwm,//状态1‘
	charge_state_err//状态0
}charge_states;

void State_Task_Init(void);
void main_task(void);

#endif