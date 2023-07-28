#include "config.h"
#include "gpio.h"
#include "delay.h"
#include "timer.h"
#include "s1_switch.h"
#include "charge_current.h"
#include "dlt645.h"
#include "cp_signal.h"
#include "state_mech.h"
#include "BspTime.h"
#include "watch_dog.h"
#include "uart.h"


void main(void)
{
	bit meterfreeze = 0;
	uint8_t meter_err_time = 0;
	int runret = 0;
	autotimer * waittimer = NULL;
	
	Timer_config();
	
	Cli_Init();
	State_Task_Init();
	CP_Init();
	S1_Switch_Init();
	S1_Switch_Off();
	Set_Charge_Current(180);
	wdt_init();wdt_enable();
	EA = 1;
	
	while(1)
	{	
		if(meterfreeze == 0){
			runret = Dlt645_update_task();
			if(runret == 0)
			{
				meter_err_time = 0;
				PrintString1("$");
				WriteData1((uint8_t *)elec_condition.Voltage,4);
				WriteData1((uint8_t *)elec_condition.Current,4);
				WriteData1((uint8_t *)elec_condition.ActivePower,4);
				WriteData1((uint8_t *)elec_condition.Energy,4);
				PrintString1("$\r\n");
			}
			else if(runret < 0)
			{
				PrintString1("runret errpr!\r\n");
				meterfreeze = 1;
			}
		}
		else{
			if(waittimer == NULL){
				//PrintString1("timer!\r\n");
				waittimer = obtainTimer(0);
			}
			else if(!IS_TIMEOUT_1MS(waittimer,500))
			{
				//PrintString1("timer 0!\r\n");
				returnTimer(waittimer);
				waittimer = NULL;
				meterfreeze = 0;
				meter_err_time++;
			}
			delay_ms(3);
			
		}
		if(S1_Switch_State && meter_err_time>10){
			S1_Switch_Off();//s1ÇÐ»»µ½12v
			charge_state = charge_state_stopping;//×ªÏò3
		}
		main_task();
		wdt_feed();
	}
}




