
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__

#include "stmflash.h"
/*
定义和设备相关的信息
*/
//#define Use_74dcode                     //带74编码功能


#define MAC_Data_Len    8       //设备的MAC长度，单位字节
#define DEVICE_INFO_BASH_ADDR			(STM32_FLASH_END-2048)	//保存设备信息的首地址 ,在最后1K

/*
设备信息
*/
typedef struct DeviceInfo_
{
	uint8_t mac_exist;		//标识设备MAC是否已经烧录
	uint8_t mac[8];			//设备MAC地址
	uint8_t aes[16];		//密钥
	uint8_t addr_DA;		//逻辑地址
	uint8_t addr_GA[3];		//群众地址
}DeviceInfo_t;


extern  DeviceInfo_t  deviceInfo;		




#endif

