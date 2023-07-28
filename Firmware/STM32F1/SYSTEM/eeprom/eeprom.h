#ifndef __EEPROM_H
#define __EEPROM_H 			   
#include "sys.h"
#include "stmflash.h"

#define _eeprom_addr_base 0X0800FF74
#define _current_addr_base (_eeprom_addr_base+0x02)
#define _limit_mode_addr_base (_eeprom_addr_base+0x04)
#define _time_limit_addr_base (_eeprom_addr_base+0x06)
#define _energy_limit_addr_base (_eeprom_addr_base+0x08)

uint16_t EEPROM_Get_Current(void);
void EEPROM_Set_Current(uint16_t current);
uint16_t EEPROM_Get_Usr_Limit_Mode(void);
void EEPROM_Set_Usr_Limit_Mode(uint16_t limitmode);
uint16_t EEPROM_Get_Time_Limit(void);
void EEPROM_Set_Time_Limit(uint16_t timelimit);
uint16_t EEPROM_Get_Energy_Limit(void);
void EEPROM_Set_Energy_Limit(uint16_t energylimit);

#endif





























