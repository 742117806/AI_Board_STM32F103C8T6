//stm32中断回到函数
#ifndef __STM32F0XX_CALL_BACK_H__
#define __STM32F0XX_CALL_BACK_H__




#include "stm32f1xx.h"
#include "uart.h"
#include <time.h>


extern volatile uint32_t SYSTICK_CNT;                //系统TICK值
extern volatile uint8_t TICK_8ms;
extern volatile time_t time_dat;

#endif

