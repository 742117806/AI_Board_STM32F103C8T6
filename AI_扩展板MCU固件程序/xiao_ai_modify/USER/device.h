
#ifndef __DEVICE_H
#define __DEVICE_H	 

#include "includes.h"

#define Version_Number 0x010304	//硬件版本01，主版本03，次版本 04
#define DEVICE_NUM_MAX	225

 //保存设备信息的首地址(最后2K的开始地址为0x0800F800)
#define DEVICE_INFO_SAVE_ADDR    0x0800F840
#define DEVICE_MAC_SAVE_ADDR	 DEVICE_INFO_SAVE_ADDR
#define DEVICE_AES_SAVE_ADDR	 DEVICE_MAC_SAVE_ADDR + sizeof(Device_mac_t)
#define DEVICE_MATCH_SAVE_ADDR   DEVICE_AES_SAVE_ADDR + sizeof(Device_aes_t)
//0x080E800 作为低功耗设备逻辑地址
#define DEVICE_SEELP_ADDR	 0x0800E800

//typedef struct mac_link_da_
//{
//	uint8_t mac[8];		//MAC地址
//	uint8_t addr;        //设备分配的逻辑地址
//    uint8_t na;			//无效，为了对齐字节
//}mac_link_da_t;

/*
设备信息
存放在STM32 内部FLAHS 的0x0800F840地址
*/

//MAC
typedef struct Device_mac_
{
	uint8_t mac_h;  		//MAC协议头
	uint8_t mac_frame_len;	//MAC协议数据长度
	uint8_t mac_len;		//MAC数据长度
    uint8_t mac[8];     	//设备MAC地址
	uint8_t mac_crc_h;     	//MAC协议CRC
	uint8_t mac_crc_l;     	//MAC协议CRC
	uint8_t mac_nc;			//无效补全对齐位
}Device_mac_t;

//密钥和群组
typedef struct Device_aes_
{	   
	uint8_t aes[16];    	//密钥
    uint8_t addr_GA[3]; 	//群众地址
    uint8_t addr_DA;		//自身逻辑地址，在此无效
}Device_aes_t;

typedef struct Device_Match_
{
	uint8_t deviceNum;      //配网的设备个数
	uint8_t deviceBuff[DEVICE_NUM_MAX]; //已经配网的设备地址
}Device_Match_t;

typedef struct DeviceInfo_
{	
	Device_mac_t mac_frame;	//mac协议帧
    Device_aes_t aes_field; //aes+家庭组+设备自身地址
    Device_Match_t match;	//匹配过的设备
} DeviceInfo_t;




extern DeviceInfo_t deviceInfo;
extern Device_Match_t sleep_device;     //低功耗设备

void  vDeviceMatchNet(uint8_t *buff,uint8_t len);
void vDeviceInfoInit(void);
uint8_t uxIsLowPowerDevice(uint8_t mac_byte7);
void vLowPowerDeviceWakeUp(uint8_t ch);
void vVersionReport(uint8_t *cmd,uint8_t len ,uint32_t version);
void vDeviceListSave(uint8_t *list,uint8_t len);
void vDeviceMacReport(uint8_t *cmd,uint8_t len ,uint8_t *mac);
void AES_Init(void);
uint8_t vDeviceIsExistList(Device_Match_t *list,uint8_t desAddr);

#endif
 

