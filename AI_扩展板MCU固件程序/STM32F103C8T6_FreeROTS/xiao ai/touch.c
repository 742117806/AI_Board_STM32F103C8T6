#include "touch.h"
#include "delay.h"

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY1按下
//2，KEY2按下
//3，KEY3按下 
//4，KEY4按下
//注意此函数有响应优先级,KEY1>KEY2>KEY3>KEY4!!

uint8_t TouchKeyScan(uint8_t mode)
{	 
	static uint8_t key_up=1;//按键按松开标志
	static uint16_t isPresCnt = 0;

	if(mode)key_up=1;  //支持连按		
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
		else if(KEY4==0)return KEY4_PRES;
	}
	else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)
	{
		if(key_up == 0)   //说明之前已经按下，现在刚刚释放按键
		{
			key_up=1; 
			return KEY_UP;
		}
		key_up=1; 
		
	}

	if(KEY1==1)
	{
	   isPresCnt = 0;
	   
	}
	else
	{
		isPresCnt ++;
		if(isPresCnt > 800)
		{
			isPresCnt = 0;
			return KEY1_LONG_PRES;
		}
	}
	
 	return 0;// 无按键按下
}


