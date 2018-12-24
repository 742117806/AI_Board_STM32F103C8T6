
#ifndef __APP_H
#define __APP_H	 

#include "includes.h"




typedef struct Msg
{
	uint8_t ucMessageID;
	uint8_t ucMessageLen;
	uint8_t ucMessageData[256];
}MSG_T;


//����ת���ߵĶ��нṹ��
typedef struct QUEUE_UART_TO_WIRELESS_
{
  uint8_t msg[256]; //��������
  uint8_t len;      //���ݳ���
  uint8_t toCh;     //Ҫ���͵���������ͨ����
} QUEUE_WIRELESS_SEND_t;

extern QueueHandle_t xQueueWirelessTx;
extern QueueHandle_t xQueueLedTask;
extern QueueHandle_t xQueueNetTask;
extern SemaphoreHandle_t xSemWireless;



void vTouchProcess(eKEY_VALUE key_now);
void vAppWirelessInit(void);
void vWirelessSendBytes(uint8_t ch, uint8_t *buff, uint8_t len);
void vWirelessRecvProcess(void);
void vLedProcess(void);
void vLedCtrlModeLoop(uint8_t mode,uint8_t color);
void vFrameUartRemoteCmdDeal(uint8_t *buff,uint8_t len);
void LowPowerDeviceWakeUp(uint8_t ch);
void LowPowerDeviceInit(void);
uint8_t LowPowerDeviceIsExsit(uint8_t addr);
uint8_t  LowPowerDeviceInset(uint8_t mac_bit7,uint8_t addr);
void LowPowerDeviceDelete(uint8_t addr);
uint8_t  LowPowerDeviceMach(uint8_t addr);


#endif
 

