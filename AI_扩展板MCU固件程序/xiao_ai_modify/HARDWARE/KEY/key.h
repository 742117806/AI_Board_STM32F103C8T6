#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 
#define KEY_LONG_TIME	200   //长按时间设置

//按键相关定义
#define KEY1_READ()	PCin(15)
#define KEY2_READ()	PCin(13)
#define KEY3_READ()	PBin(6)
#define KEY4_READ()	PCin(14)

//按键键值枚举结构
typedef enum 
{
	KEY_NONE,		//没有按键
	KEY1_PRES_S,		//短按
	KEY1_PRES_L,		//长按
	KEY2_PRES_S,		//短按
	KEY2_PRES_L,		//长按
	KEY3_PRES_S,		//短按
	KEY3_PRES_L,		//长按
	KEY4_PRES_S,		//短按
	KEY4_PRES_L,		//长按
	KEY_ALL_REL,		//所有按键都释放了
}eKEY_VALUE;

void vKeyInit(void);//初始化
eKEY_VALUE eKeyScan(uint8_t mode);

		 				    
#endif
