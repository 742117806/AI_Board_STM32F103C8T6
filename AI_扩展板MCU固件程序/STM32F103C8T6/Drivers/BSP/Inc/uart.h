
#ifndef __UART_H__
#define __UART_H__

#include "stm32f1xx.h"
#include "stm32f0_usart.h"

#define _DEBUG_ 0				//定义是否打印调试信息：0不打印，1打印

#if _DEBUG_
#define DEBUG_Printf(...)   printf(__VA_ARGS__)
#define DEBUG_SendData(d)   Uart1SendData(d)
#define DEBUG_SendBytes(a,l)  Uart1SendBytes(a,l)
#else
#define DEBUG_Printf(...) 
#define DEBUG_SendData(d)
#define DEBUG_SendBytes(a)
#endif


void Uart1SendData(uint8_t byte);
void Uart1SendBytes(uint8_t *buf,uint16_t len);
void UartSendData(USART_TypeDef *USARTx,uint8_t byte);
void UartSendBytes(USART_TypeDef *USARTx,uint8_t *buf,uint16_t len);
void UartSendStr(USART_TypeDef *USARTx,char *str);
#endif

