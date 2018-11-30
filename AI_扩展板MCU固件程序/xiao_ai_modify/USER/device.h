
#ifndef __DEVICE_H
#define __DEVICE_H	 

#include "includes.h"

#define Version_Number 0x0302

 //�����豸��Ϣ���׵�ַ
#define DEVICE_INFO_SAVE_ADDR    0x0800F840
#define DEVICE_MAC_SAVE_ADDR	 DEVICE_INFO_SAVE_ADDR
#define DEVICE_AES_SAVE_ADDR	 DEVICE_MAC_SAVE_ADDR + sizeof(Device_mac_t)
#define DEVICE_MATCH_SAVE_ADDR   DEVICE_AES_SAVE_ADDR + sizeof(Device_aes_t)

/*
�豸��Ϣ
�����STM32 �ڲ�FLAHS ��0x0800F840��ַ
*/

//MAC
typedef struct Device_mac_
{
	uint8_t mac_h;  		//MACЭ��ͷ
	uint8_t mac_frame_len;	//MACЭ�����ݳ���
	uint8_t mac_len;		//MAC���ݳ���
    uint8_t mac[8];     	//�豸MAC��ַ
	uint8_t mac_crc_h;     	//MACЭ��CRC
	uint8_t mac_crc_l;     	//MACЭ��CRC
	uint8_t mac_nc;			//��Ч��ȫ����λ
}Device_mac_t;

//��Կ��Ⱥ��
typedef struct Device_aes_
{	   
	uint8_t aes[16];    	//��Կ
    uint8_t addr_GA[3]; 	//Ⱥ�ڵ�ַ
    uint8_t addr_DA;		//�����߼���ַ���ڴ���Ч
}Device_aes_t;

typedef struct Device_Match_
{
	uint8_t deviceNum;      //�������豸����
	uint8_t deviceBuff[225]; //�Ѿ��������豸��ַ
}Device_Match_t;

typedef struct DeviceInfo_
{	
	Device_mac_t mac_frame;	//macЭ��֡
    Device_aes_t aes_field; //aes+��ͥ��+�豸�����ַ
    Device_Match_t match;	//ƥ������豸
} DeviceInfo_t;




extern DeviceInfo_t deviceInfo;

void  vDeviceMatchNet(uint8_t *buff,uint8_t len);
void vDeviceInfoInit(void);
uint8_t uxIsLowPowerDevice(uint8_t mac_byte7);
void vLowPowerDeviceWakeUp(uint8_t ch);
void vVersionReport(uint8_t *cmd,uint8_t len ,uint16_t version);
void vDeviceListSave(uint8_t *list,uint8_t len);
void vDeviceMacReport(uint8_t *cmd,uint8_t len ,uint8_t *mac);

#endif
 

