#ifndef TOUCH_H
#define TOUCH_H

//#include "peri_includes.h"
#include "frame_process.h"
#include "main.h"

#define Touch_ShortTime 1u        //短按值
#define Touch_FilterTime 8u       //滤波值
#define Touch_LongTime (125 * 15) //长按值
#define Touch_DisTime 100u        //间隔值

#define Key_NoVal 0u
#define Key_ShortVal 1u
#define Key_LongVal 2u

#define Touch_Unchanged 0u
#define Touch_RiseEdge 1u
#define Touch_FallEdge 2u

#define Key1_ShortVal 0x01
#define Key1_LongVal 0x10
#define Key2_ShortVal 0x02
#define Key2_LongVal 0x20
#define Key3_ShortVal 0x04
#define Key3_LongVal 0x40
#define Key4_ShortVal 0x08
#define Key4_LongVal 0x80

#define KEY1 Touch1_RDPIN() //读取按键1
#define KEY2 Touch2_RDPIN() //读取按键2
#define KEY3 Touch3_RDPIN() //读取按键3
#define KEY4 Touch4_RDPIN() //读取按键4

#define KEY1_PRES 0x01      //KEY1按下
#define KEY2_PRES 0x02      //KEY2按下
#define KEY3_PRES 0x04      //KEY3按下
#define KEY4_PRES 0x08      //KEY4按下
#define KEY1_LONG_PRES 0x11 //KEY1按下
#define KEY2_LONG_PRES 0x22 //KEY1按下
#define KEY3_LONG_PRES 0x44 //KEY1按下
#define KEY4_LONG_PRES 0x88 //KEY1按下
#define KEY_UP 6            //按键抬起

/************************* 外部函数 ***************************/

uint8_t TouchKeyScan(uint8_t mode);

#endif
