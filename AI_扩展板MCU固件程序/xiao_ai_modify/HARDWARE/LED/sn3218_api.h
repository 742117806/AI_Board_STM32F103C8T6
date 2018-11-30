#ifndef __SN3218_API_H
#define __SN3218_API_H

#include "includes.h"

#define OFF	0
#define ON	(!OFF)



typedef enum 
{
	LED_COLOR_ORANG,
	LED_COLOR_BLUE,
    LED_COLOR_ALL,
}LED_Color_t;

extern uint8_t led_set_mode;
extern LED_Color_t led_set_color;
extern uint8_t led_delay;


void LED_AroundStaSet(LED_Color_t color_index,uint8_t sta);
void LED_CenterStaSet(uint8_t sta);
void LED_ALL_StaSet(uint8_t sta);
void LED_CenterPwmSet(uint8_t pwm);
void LED_AroundPwmSet(LED_Color_t color_index,uint8_t pwm);
void LED_AroundFlow(LED_Color_t color,uint8_t speed);
void LED_AroundBreath(LED_Color_t color,uint8_t speed);
void LedDispKey1(LED_Color_t color,uint8_t sta);
void LedDispKey3(LED_Color_t color,uint8_t sta);
void LedDispVol(uint8_t vol);



#endif
