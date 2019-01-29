#include "timer.h"
#include "led.h"

void Delay_Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInitStruct.TIM_Period = 100-1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = (72-1);
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
    
    while((TIM3->SR & TIM_FLAG_Update)!=SET);
    TIM3->SR = (uint16_t)~TIM_FLAG_Update;
}


void delay_us(uint32_t nus)
{
    TIM3->CNT = nus-1;
    TIM3->CR1 |= TIM_CR1_CEN;    
    while((TIM3->SR & TIM_FLAG_Update)!=SET);
    TIM3->SR = (uint16_t)~TIM_FLAG_Update;
    TIM3->CR1 &= ~TIM_CR1_CEN;
}

/*
********************************************************************************************************* 
*  函 数 名: delay_ms 
*  功能说明: 延时nus 
*  形    参: nms 要延时的ms数
*  返 回 值: 无  
********************************************************************************************************* 
*/ 
void delay_ms(uint32_t nms)
{
    uint32_t i;
    for (i = 0; i < nms; i++)
        delay_us(1000);
}














