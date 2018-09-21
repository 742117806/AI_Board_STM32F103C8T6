
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__

#include "stmflash.h"
/*
定义和设备相关的信息
*/
//#define Use_74dcode                     //带74编码功能

#define MAC_Data_Len    8       //设备的MAC长度，单位字节
#define EEPROM_MAC_ADDR			(STM32_FLASH_END-1024)	//保存设备信息的首地址 ,在最后1K

#endif

