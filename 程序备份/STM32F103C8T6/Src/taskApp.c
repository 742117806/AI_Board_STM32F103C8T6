//RTX任务应用

#include "taskApp.h"
#include "RTL.h"
#include "user_call_back.h"
#include "spi.h"
#include "delay.h"
/* 
********************************************************************************************************** 
                      函数声明 
********************************************************************************************************** 
*/

static void AppTaskCreate(void);
__task void AppTaskLED(void);
__task void AppTaskStart(void);

/* 
********************************************************************************************************** 
                       变量
********************************************************************************************************** 
*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
static uint64_t AppTaskLEDStk[256 / 8];   /* 任务栈 */
static uint64_t AppTaskStartStk[512 / 8]; /* 任务栈 */

/* 任务句柄 */
OS_TID HandleTaskLED = NULL;

/* 
********************************************************************************************************* 
*  函 数 名: RTX_OS_Init 
*  功能说明: 启动RTX系统
*  形    参: 无 
*  返 回 值: 无 
********************************************************************************************************* 
*/

void RTX_OS_Init(void)
{
    /* 创建启动任务 */
    os_sys_init_user(AppTaskStart,             /* 任务函数 */
                     2,                        /* 任务优先级 */
                     &AppTaskStartStk,         /* 任务栈 */
                     sizeof(AppTaskStartStk)); /* 任务栈大小，单位字节数 */
}
/* 
********************************************************************************************************* 
*  函 数 名: AppTaskLED 
*  功能说明: LED闪烁  
*  形    参: 无 
*  返 回 值: 无 
*    优 先 级: 1  (数值越小优先级越低，这个跟 uCOS相反) 
********************************************************************************************************* 
*/
extern TIM_HandleTypeDef htim1;
extern uint32_t time1_delay;
extern uint32_t delay_flag;
extern void Time1Delay_us(uint32_t n_us);
__task void AppTaskLED(void)
{
	uint8_t flag = 0;
	
//	SN3218_Init();
//	PowerOn_Led();
    //extern SPI_HandleTypeDef hspi1;
   // uint8_t wireless_init_result;

    //wireless_init_result = Wireless_Init();
    //DEBUG_Printf("Wireless_Init() = %d\r\n", wireless_init_result);
    //Si4438_Receive_Start(Wireless_Channel[0]);	//开始接收无线数据
    while (1)
    {
		
       // LEDR_TOGGLE();
	    //TouchLED_ALL(flag^1);
		delay_ms(30);
        os_dly_wait(110);
    }
}

/* 
********************************************************************************************************* 
*  函 数 名: AppTaskStart 
*  功能说明: 启动任务，也就是最高优先级任务。 
*  形    参: 无 
*  返 回 值: 无 
*    优 先 级: 2   
********************************************************************************************************* 
*/
__task void AppTaskStart(void)
{

    //Wireless_Init();
    uint16_t len, t;
    AppTaskCreate();

    while (1)
    {

        //LEDR_TOGGLE();

        if (USART2_RX_STA & 0x8000)
        {
            len = USART2_RX_STA & 0x3fff; //得到此次接收到的数据长度
            //printf("\r\n您发送的消息为:\r\n\r\n");
            for (t = 0; t < len; t++)
            {
                UartSendData(USART2, USART2_RX_BUF[t]); //向串口1发送数据
            }
            //printf("\r\n\r\n");//插入换行
            USART2_RX_STA = 0;
        }
        if (USART_RX_STA & 0x8000)
        {
            len = USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
            //printf("\r\n您发送的消息为:\r\n\r\n");
            for (t = 0; t < len; t++)
            {
                UartSendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
				
            }
			//Si4438_Transmit_Start(&Wireless_Buf,22,USART_RX_BUF,len);
            //printf("\r\n\r\n");//插入换行
            USART_RX_STA = 0;
        }
        os_dly_wait(1);
    }
}

/* 
********************************************************************************************************* 
*  函 数 名: AppTaskCreate 
*  功能说明: 创建应用任务 
*  形    参: 无 
*  返 回 值: 无 
********************************************************************************************************* 
*/
static void AppTaskCreate(void)
{
    HandleTaskLED = os_tsk_create_user(AppTaskLED,             /* 任务函数 */
                                       1,                      /* 任务优先级 */
                                       &AppTaskLEDStk,         /* 任务栈 */
                                       sizeof(AppTaskLEDStk)); /* 任务栈大小，单位字节数 */
}
