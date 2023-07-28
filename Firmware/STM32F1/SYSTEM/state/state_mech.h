#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "sys.h"

//标准io定义

#define AC_LED_PIN GPIO_Pin_14 //市电led指示灯
#define AC_LED_GPIO GPIOB
#define AC_LED_GPIO_OUT PBout(14)
#define GUN_LED_PIN GPIO_Pin_2 //插枪led指示灯
#define GUN_LED_GPIO GPIOB
#define GUN_LED_GPIO_OUT PBout(2)
#define UCK_LED_PIN GPIO_Pin_12//解锁led指示灯
#define UCK_LED_GPIO GPIOB
#define UCK_LED_GPIO_OUT PBout(12)
#define CHG_LED_PIN GPIO_Pin_7 //充电中led指示灯
#define CHG_LED_GPIO GPIOA
#define CHG_LED_GPIO_OUT PAout(7)
#define EMG_OUT_PIN GPIO_Pin_13//警告led指示灯+蜂鸣器
#define EMG_OUT_GPIO GPIOB
#define EMG_OUT_GPIO_OUT PBout(13)

#define EMG_STOP_PIN GPIO_Pin_12//紧急开关输入
#define EMG_STOP_GPIO GPIOA
#define EMG_STOP_GPIO_IN PAin(12)
#define BTN_PIN GPIO_Pin_11     //按钮输入
#define BTN_GPIO GPIOA
#define BTN_GPIO_IN PAin(11)
#define SELA_PIN GPIO_Pin_8	    //旋钮左侧输入
#define SELA_GPIO GPIOA
#define SELA_GPIO_IN PAin(8)
#define SELB_PIN GPIO_Pin_15    //旋钮右侧输入
#define SELB_GPIO GPIOB
#define SELB_GPIO_IN PBin(15)

#define RFID_UNLOCK_PIN GPIO_Pin_0 //rfid锁输入
#define RFID_UNLOCK_GPIO GPIOB
#define RFID_UNLOCK_GPIO_IN PBin(0)

#define BLE_LOCK_PIN GPIO_Pin_6    //蓝牙锁输入
#define BLE_LOCK_GPIO GPIOA
#define BLE_LOCK_GPIO_IN PAin(6)
#define BLE_UNLOCK_PIN GPIO_Pin_5  //蓝牙解锁输入
#define BLE_UNLOCK_GPIO GPIOA
#define BLE_UNLOCK_GPIO_IN PAin(5)

#define AC_OUT_IN_PIN GPIO_Pin_5    //AC输出感应输入
#define AC_OUT_IN_GPIO GPIOB
#define AC_OUT_IN_GPIO_IN PBin(5)

#define RELAY_OUT_PIN GPIO_Pin_4    //接触器输出
#define RELAY_OUT_GPIO GPIOB
#define RELAY_OUT_GPIO_OUT PBout(4)

//状态机们-----------------------------------------

#define ac_voltage_max (260.00f)
#define ac_voltage_min (180.00f)

#define ac_current_size_20 (1.1f)
#define ac_current_size_0 (2.0f)

#define EPSINON (1e-6)

#define temp_threshold_high 790

extern volatile uint32_t emgstate;//紧急状态输入 1紧急 0正常
extern volatile uint32_t outputen;//输出允许 1允许 0不允许
extern volatile uint32_t lock;//外部锁定系统 1锁定 0解锁

extern volatile uint32_t output;//输出 1输出 0关闭输出

extern volatile uint32_t overtemp;//过温 1紧急 0正常
extern volatile uint32_t overvoltage;//过压 1紧急 0正常
extern volatile uint32_t undervoltage;//欠压 1紧急 0正常
extern volatile uint32_t overcurrent;//过流 1紧急 0正常
extern volatile uint32_t emgstop;//紧急停止按钮 1紧急 0正常

extern volatile uint32_t chargegun;//插枪状态 1插入 0拔出
extern volatile uint32_t chargestart;//充电启动过的状态 1启动过了 0没有

extern volatile uint32_t S1_Switch_State;//S1开关状态
extern volatile uint32_t rfidlock;//rfid锁状态
extern volatile uint32_t blelock;//ble锁状态
extern volatile uint32_t outvol;//输出电压状态

extern volatile uint32_t meter_err;//表头通信错误

extern volatile uint32_t cp_voltage;//cp线电压，以0.01v单位
extern volatile uint8_t charge_state;//充电状态
extern volatile uint8_t cp_state;//cp线档位
extern volatile uint16_t avaliable_current;//当前电流

extern volatile uint16_t usr_set_current;//用户设置电流
extern volatile uint16_t usr_sel_limit_mode;//用户选择的限制模式
extern volatile uint16_t time_limit;//用户选择的限制时长（单位分钟）
extern volatile uint16_t energy_limit;//用户选择的限制能量（单位0.1kwh）


extern volatile uint32_t energy_charged;//本轮充电的电量
extern volatile uint32_t time_charged;//本轮充电的时长
extern volatile uint8_t stop_reason;//停止原因

extern volatile uint32_t alarm_on;//是否启动警告


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

void Get_Datas_From_EEPROM(void);
void State_Task_Init(void);
void main_task(void);
void Out_Task(void);

#endif

