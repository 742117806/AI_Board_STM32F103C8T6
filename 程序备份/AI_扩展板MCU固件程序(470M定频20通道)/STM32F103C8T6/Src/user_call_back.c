//stm32中断回到函数
#include "user_call_back.h"

#include "delay.h"
#include "wireless_app.h"
#include <time.h>

uint8_t USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART_RX_STA = 0; //接收状态标记

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
uint8_t USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART2_RX_STA = 0; //接收状态标记

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

        if ((USART2_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (USART2_RX_STA & 0x4000) //接收到了0x0d
            {
                if (uart2_rec != 0x0a)
                    USART2_RX_STA = 0; //接收错误,重新开始
                else
                    USART2_RX_STA |= 0x8000; //接收完成了
            }
            else //还没收到0X0D
            {
                if (uart2_rec == 0x0d)
                    USART2_RX_STA |= 0x4000;
                else
                {
                    USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = uart2_rec;
                    USART2_RX_STA++;
                    if (USART_RX_STA > (USART2_REC_LEN - 1))
                        USART2_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }
    }
	
	if (huart->Instance == USART1)
    {
        if ((USART_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (USART_RX_STA & 0x4000) //接收到了0x0d
            {
                if (uart1_rec != 0x0a)
                    USART_RX_STA = 0; //接收错误,重新开始
                else
                    USART_RX_STA |= 0x8000; //接收完成了
            }
            else //还没收到0X0D
            {
                if (uart1_rec == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = uart1_rec;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }
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
uint32_t test_time1_cnt=0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	extern TIM_HandleTypeDef htim1;
	
	if(htim->Instance == htim1.Instance)
	{
		#if OS_USE
		 time1_delay_cnt ++;		 
		#endif
		test_time1_cnt++;
		if(test_time1_cnt>1000)
		{
			test_time1_cnt = 0;
			printf("HAL_TIM_PeriodElapsedCallback");
		}
	}
}

/********************************************************************************************************* 
*  函 数 名: HAL_SYSTICK_Callback 
*  功能说明: 系统滴答时钟回调函数
*  形    参: 无 
*  返 回 值: 无 
********************************************************************************************************* 
*/
extern void SysTick_Handle(void);
void HAL_SYSTICK_Callback(void)
{
	SYSTICK_CNT++;
    SysTick_Handle();
    
    if((SYSTICK_CNT % 1000) == 0)
    {
      time_dat++;
    }
}



