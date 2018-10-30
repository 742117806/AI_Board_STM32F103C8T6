//stm32中断回到函数
#include "user_call_back.h"

#include "delay.h"
#include "wireless_app.h"
#include <time.h>


volatile uint32_t SYSTICK_CNT;                //系统TICK值
volatile uint8_t TICK_8ms;
volatile time_t time_dat;


//外部管脚中断
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_12) //无线IRQ中断，PB6
    {
        Si4438_Interrupt_Handler(&Wireless_Buf);
    }
}

/* 
********************************************************************************************************* 
*  函 数 名: HAL_UART_RxCpltCallback 
*  功能说明: 串口回调函数
*  形    参: 无 
*  返 回 值: 无 
********************************************************************************************************* 
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    extern uint8_t uart1_rec;
    extern uint8_t uart2_rec;



    if (huart->Instance == USART2)
    {

    }
	
	if (huart->Instance == USART1)
    {
		UpUart_RX_INT_Process(uart1_rec, &UpCom_RxBuf);
    }
}


 /* 
********************************************************************************************************* 
*  函 数 名: HAL_TIMEx_BreakCallback 
*  功能说明: 定时器回调函数
*  形    参: 无 
*  返 回 值: 无 
********************************************************************************************************* 
*/
extern void SysTick_Handle(void);
uint32_t test_time1_cnt=0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	extern TIM_HandleTypeDef htim1;	
	if(htim->Instance == htim1.Instance)
	{
		SYSTICK_CNT++;
		SysTick_Handle();
		
		if((SYSTICK_CNT % 1000) == 0)
		{
		  time_dat++;
		}
		
		if (UpCom_RxBuf.Over_time_count)
		{
			if (++UpCom_RxBuf.Over_time_count > Up_TimeOut_Val)
			{
				UpCom_RxBuf.Over_time_count = 0;
				UpCom_RxBuf.Rx_Status = UartRx_FrameHead;
			}
		}

//		if (Wireless_Buf.Sent_TimeOut_Cnt > 0)
//		{
//			if (++Wireless_Buf.Sent_TimeOut_Cnt > WLSent_TimeOut_Val)
//			{
//				Wireless_Buf.Sent_TimeOut_Cnt = 0;
//				WIRELESS_STATUS = Wireless_TX_Finish;
//			}
//		}		
	}
}

/********************************************************************************************************* 
*  函 数 名: HAL_SYSTICK_Callback 
*  功能说明: 系统滴答时钟回调函数
*  形    参: 无 
*  返 回 值: 无 
********************************************************************************************************* 
*/

//void HAL_SYSTICK_Callback(void)
//{

//}



