#include "eeprom.h"


uint16_t EEPROM_Get_Current(){
	uint16_t read = 0;
	STMFLASH_Read(_current_addr_base,&read,1);
	return read;
}

void EEPROM_Set_Current(uint16_t current){
	STMFLASH_Write(_current_addr_base,&current,1);
}

uint16_t EEPROM_Get_Usr_Limit_Mode(){
	uint16_t read = 0;
	STMFLASH_Read(_limit_mode_addr_base,&read,1);
	return read;
}

void EEPROM_Set_Usr_Limit_Mode(uint16_t limitmode){
	STMFLASH_Write(_limit_mode_addr_base,&limitmode,1);
}

uint16_t EEPROM_Get_Time_Limit(){
	uint16_t read = 0;
	STMFLASH_Read(_time_limit_addr_base,&read,1);
	return read;
}

void EEPROM_Set_Time_Limit(uint16_t timelimit){
	STMFLASH_Write(_time_limit_addr_base,&timelimit,1);
}


uint16_t EEPROM_Get_Energy_Limit(){
	uint16_t read = 0;
	STMFLASH_Read(_energy_limit_addr_base,&read,1);
	return read;
}

void EEPROM_Set_Energy_Limit(uint16_t energylimit){
	STMFLASH_Write(_energy_limit_addr_base,&energylimit,1);
}




































