
#ifndef __TASKAPP_H__
#define __TASKAPP_H__

#include "stm32f1xx.h"
//#include "RTL.h"
#include "delay.h"
#include "uart.h"

#include "wireless_app.h"
#include "cmsis_os.h"

/* 任务事件定义 */
//串口任务事件
#define UART_TASK_EVNT_KEY1_DN (1ul << 0)
#define UART_TASK_EVNT_KEY1_UP (1ul << 1)
#define UART_TASK_EVNT_KEY2_DN (1ul << 2)
#define UART_TASK_EVNT_KEY2_UP (1ul << 3)
#define UART_TASK_EVNT_KEY3_DN (1ul << 4)
#define UART_TASK_EVNT_KEY3_UP (1ul << 5)
#define UART_TASK_EVNT_KEY4_DN (1ul << 6)
#define UART_TASK_EVNT_KEY4_UP (1ul << 7)
#define UART_TASK_EVNT_KEY1_DN_L (1ul << 8)      //KEY1长按
#define UART_TASK_EVNT_WIRELESS_ACK (1ul << 9)   //无线的应答数据
#define UART_TASK_EVNT_WIRELESS_EVNT (1ul << 10) //无线主动上报的数据

//LED任务事件
#define LED_TASK_EVNT_KEY1_DN (1ul << 0)
#define LED_TASK_EVNT_KEY1_UP (1ul << 1)
#define LED_TASK_EVNT_KEY2_DN (1ul << 2)
#define LED_TASK_EVNT_KEY2_UP (1ul << 3)
#define LED_TASK_EVNT_KEY3_DN (1ul << 4)
#define LED_TASK_EVNT_KEY3_UP (1ul << 5)
#define LED_TASK_EVNT_KEY4_DN (1ul << 6)
#define LED_TASK_EVNT_KEY4_UP (1ul << 7)
#define LED_TASK_EVNT_KEY1_DN_L (1ul << 8) //KEY1长按

extern QUEUE_WIRELESS_SEND_t retryWirelessBuff;
extern uint8_t currentFrameNum;

extern QueueHandle_t xQueueWirelessTask;
extern QueueHandle_t xQueueVol;
extern QueueHandle_t xQueueAckRouterTable;
extern QueueHandle_t xQueueNetCreateTask;
extern QueueHandle_t xQueueWirelessRetryTask;
/* 
********************************************************************************************************** 
                      函数声明 
********************************************************************************************************** 
*/
void AppTaskCreate(void);

#endif
