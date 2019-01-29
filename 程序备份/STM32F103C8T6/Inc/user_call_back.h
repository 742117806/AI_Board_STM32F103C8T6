//stm32中断回到函数
#ifndef __STM32F0XX_CALL_BACK_H__
#define __STM32F0XX_CALL_BACK_H__




#include "stm32f1xx.h"
#include "uart.h"
#include <time.h>


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define USART2_REC_LEN  			200  	//定义最大接收字节数 200

extern uint8_t USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
extern uint16_t USART_RX_STA; //接收状态标记

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
extern uint8_t USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
extern uint16_t USART2_RX_STA; //接收状态标记

extern volatile uint32_t SYSTICK_CNT;                //系统TICK值
extern volatile uint8_t TICK_8ms;
extern volatile time_t time_dat;

#endif

