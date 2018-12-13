
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__

#include "stmflash.h"
/*
定义和设备相关的信息
*/
//#define Use_74dcode                     //带74编码功能

#define MAC_Data_Len 8                                  //设备的MAC长度，单位字节
#define DEVICE_ROUTER_TAB_ADDR	(STM32_FLASH_END-10*1024) //最后10K作为保存数据区
#define DEVICE_ROUTER_TAB_LEN		(4*1024)			            //4K 

#define OLD_DEVICE_ADDR					(DEVICE_ROUTER_TAB_ADDR + DEVICE_ROUTER_TAB_LEN)	//旧设备
#define OLD_DEVICE_LEN					(4*1024)			                //4K
#define DEVICE_INFO_BASH_ADDR 	(OLD_DEVICE_ADDR + OLD_DEVICE_LEN+0x40)  //保存设备信息的首地址 ,在最后2K

#define LOW_PWR_DEVICE_ADDR		(STM32_FLASH_END-11*1024)


/*
设备信息
*/
typedef struct DeviceInfo_
{

	uint8_t mac_h;  		//MACÐ­ÒéÍ·
	uint8_t mac_frame_len;	//MACÐ­ÒéÊý¾Ý³¤¶È
	uint8_t mac_len;		//MACÊý¾Ý³¤¶È
    uint8_t mac[8];     	//Éè±¸MACµØÖ·
	uint8_t mac_crc_h;     	//MACÐ­ÒéCRC
	uint8_t mac_crc_l;     	//MACÐ­ÒéCRC

	//uint8_t mac[8];     //设备MAC地址
    uint8_t aes[16];    //密钥
    uint8_t addr_DA;    //逻辑地址
    uint8_t addr_GA[3]; //群众地址
    uint8_t deviceNum;      //配网的设备个数
    uint8_t deviceBuff[224]; //已经配网的设备地址

} DeviceInfo_t;

typedef struct OldDevice_
{
    uint8_t num;	//旧版本设备的个数
    uint8_t buff[100];	//旧版本的设备（0xAC），为了兼容新协议（0x69）
    uint8_t no;			//为了数据字节对齐2的倍数
} OldDevice_t;



extern DeviceInfo_t deviceInfo;
extern OldDevice_t 	oldDevice;
void LowPowerDeviceInit(void);
uint8_t  LowPowerDeviceInset(uint8_t mac_bit7,uint8_t addr);

uint8_t  LowPowerDeviceMach(uint8_t addr);
void LowPowerDeviceWakeUp(uint8_t ch);
uint8_t IsLowPowerDevice(uint8_t mac_bit7);
void LowPowerDeviceDelete(void);

#endif
