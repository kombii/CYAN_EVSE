#ifndef _DS64_ROM_SEARCH_H_
#define _DS64_ROM_SEARCH_H_

#include "stdint.h"
#include <stddef.h>



#define    DS_DEVICE_NUM       (8)

#define    DS_ROM_SRCH_PRINT                printf

#define    DS_ROM_SRCH_DEV_RST()            DS18B20_Rst()
#define    DS_ROM_SRCH_DELAY_US(us)         delay_us(us)
#define    DS_ROM_SRCH_WRITE_BYTE(byte)     DS18B20_Write_Byte(byte)


#define     DS_ROM_SRCH_READ_BIT()           DS18B20_Read_Bit()
#define     DS_ROM_SRCH_READ2BITS()          DS18B20_Read_2Bit()
//#define DS_ROM_SRCH_READ_BYTE()               DS18B20_Read_Byte()

#define     DS_ROM_SRCH_WRITE_BIT(dat)       DS18B20_Write_Bit(dat)
//#define DS_ROM_SRCH_WRITE_BYTE(byte)        DS18B20_Write_Byte(byte)


typedef struct _GuidSearchTypdef{
    union{
        uint64_t u64Bits;
        uint8_t  bytes[8];
    }guid;
}GuidSearchTypdef,*PGuidSearchCtrl;

extern volatile uint8_t Ds_Device_Searched_Num;
extern volatile GuidSearchTypdef  DsGuid[DS_DEVICE_NUM];


int16_t Ds_RomSearch(GuidSearchTypdef pGuidSrch[], uint8_t deviceNum);





#endif
