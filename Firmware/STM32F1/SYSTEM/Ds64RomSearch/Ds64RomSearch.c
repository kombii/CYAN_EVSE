#include "Ds64RomSearch.h"
#include "delay.h"
#include "DS18B20.h"
#include <stdio.h>
#include "string.h"

volatile GuidSearchTypdef DsGuid[DS_DEVICE_NUM]={0};
volatile uint8_t Ds_Device_Searched_Num = 0;

int16_t DS_FindAnUnusedGuid(uint8_t bitPosArr[], uint8_t deviceNum)
{
	if(NULL == bitPosArr || 0 == deviceNum){
		return -1;
	}
	uint8_t index=0;
	for(index=0; index<deviceNum; ++index){
		if(0 == bitPosArr[index]){
			break;
		}
	}
	return index;
}

int16_t Ds_RomSearch(GuidSearchTypdef pGuidSrch[], uint8_t deviceNum)
{
    //DS_ROM_SRCH_PRINT("-> %s \r\n",__func__);
	//DS_ROM_SRCH_PRINT("DSROM搜索开始\r\n");
    if(NULL==pGuidSrch || 0==deviceNum){
        return -1;
    }
	uint8_t bitPosArr[deviceNum];
    int8_t  bitCnt=0,guidCnt=0;
    uint8_t readByte=0;
    uint8_t devCnt=1,lastDevCnt=0;
    int16_t index=0;

    memset(bitPosArr, 0, sizeof(bitPosArr) );
    
    for(guidCnt=0; guidCnt<deviceNum && lastDevCnt<devCnt; ++guidCnt){
		//DS_ROM_SRCH_PRINT("DSROM列表%02u设备搜索开始\r\n",guidCnt);
        //DS_ROM_SRCH_PRINT("%s, guidCnt %u\r\n",__func__,guidCnt);
        
        lastDevCnt=devCnt;
        
        DS_ROM_SRCH_DEV_RST();
        DS_ROM_SRCH_DELAY_US(480);//添加一点延时，因为rst和search需要的延时不一样。
        DS_ROM_SRCH_WRITE_BYTE(0xF0); //发送Search指令
        
        
        for(bitCnt=1; bitCnt<=bitPosArr[guidCnt]; ++bitCnt){
			readByte=DS_ROM_SRCH_READ2BITS(); // 2 read
            
            //写入pGuidSrch[guidCnt].guid.u64Bits
            //写入范围是0到pGuidSrch[guidCnt].pos
            //由于union，也就是bit[0, pos)            
            if(pGuidSrch[guidCnt].guid.u64Bits & ((uint64_t)1<<(bitCnt-1) )){
                DS_ROM_SRCH_WRITE_BIT( 1 ); 
                //DS_ROM_SRCH_PRINT("%s, %2u: 1x\r\n",__func__,bitCnt);
            }
            else{
                DS_ROM_SRCH_WRITE_BIT( 0 ); 
                //DS_ROM_SRCH_PRINT("%s, %2u: 0x\r\n",__func__,bitCnt);
            }
		}
		
        for( bitCnt=bitPosArr[guidCnt]+1; bitCnt<=64; ++bitCnt){
            readByte=DS_ROM_SRCH_READ2BITS(); // 2 bits read -> readByte 
            readByte &= 0x03;

            bitPosArr[guidCnt]=bitCnt;
            switch(readByte){
            case 0x00:  //conflict bit
                index=DS_FindAnUnusedGuid(bitPosArr, DS_DEVICE_NUM);
                if(0 > index){
                    return -2;
                }
                bitPosArr[index]=bitPosArr[guidCnt];
                pGuidSrch[index]=pGuidSrch[guidCnt];
                pGuidSrch[guidCnt].guid.u64Bits &= ~ ((uint64_t)1<<(bitCnt-1) );
                pGuidSrch[index].guid.u64Bits   |=   ((uint64_t)1<<(bitCnt-1) );

                DS_ROM_SRCH_WRITE_BIT( 0 ); // 1 bits write 0


                ++devCnt;
                //DS_ROM_SRCH_PRINT("%s, %2u: 00---index:%d  pos:%d\r\n",__func__,bitCnt,index,bitPosArr[guidCnt]);
                //DS_ROM_SRCH_PRINT("%s, %2u: 00\r\n",__func__,bitCnt);
                break;	
            case 0x01:  //all are zero
                pGuidSrch[guidCnt].guid.u64Bits &= ~((uint64_t)1<<(bitCnt-1) );

                DS_ROM_SRCH_WRITE_BIT( 0 ); // 1 bits write 0
                
                //DS_ROM_SRCH_PRINT("%s, %2u: 01\r\n",__func__,bitCnt);
                break;	
            case 0x02:  //all are one
                pGuidSrch[guidCnt].guid.u64Bits |= ((uint64_t)1<<(bitCnt-1) );

                DS_ROM_SRCH_WRITE_BIT( 1 ); // 1 bits write 1
            
                //DS_ROM_SRCH_PRINT("%s, %2u: 10\r\n",__func__,bitCnt);
                break;	
            case 0x03:  //no device
                devCnt=0;
                guidCnt=0;
            
                //DS_ROM_SRCH_PRINT("%s, %2u: 11\r\n",__func__,bitCnt);	
                break;

            default:
                //DS_ROM_SRCH_PRINT("%s, default, readByte: %#X\r\n",__func__, readByte);
                break;
            }
            
        }
    }
    
    //DS_ROM_SRCH_PRINT("%s, ROM_GUID[0]: %#X \r\n",__func__, pGuidSrch[0].guid.u64Bits);
    
//    for(index=0; index<devCnt; ++index){
//        //DS_ROM_SRCH_PRINT("%s, device %u ROM_GUID: ",__func__,index);
//		DS_ROM_SRCH_PRINT("DSROM列表%02u设备ROM_GUID: ",index);
//        for(bitCnt=0; bitCnt<8; ++bitCnt){
//            DS_ROM_SRCH_PRINT("%02X",pGuidSrch[index].guid.bytes[bitCnt]);
//        }
//        DS_ROM_SRCH_PRINT("\r\n");
//    }
    
    //DS_ROM_SRCH_PRINT("%s, guidCnt: %d,  devCnt:%d \r\n",__func__,guidCnt,devCnt);
    
    //DS_ROM_SRCH_PRINT("<- %s \r\n",__func__);
	Ds_Device_Searched_Num = devCnt;
	//DS_ROM_SRCH_PRINT("DSROM搜索结束\r\n");
    return devCnt;
}



