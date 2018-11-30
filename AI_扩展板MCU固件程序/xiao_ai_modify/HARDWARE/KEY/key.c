

#include "includes.h"


/*
*********************************************************************************************************
*  函 数 名: vKeyInit
*  功能说明: 按键初始化KEY1,KEY2,KEY3,KEY4(PC15,PC13,PB6,PC14)
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vKeyInit(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB,PC端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_14;	//KEY1,KEY2,KEY4,端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	 //上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //KEY3端口配置
    GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //根据设定参数初始化GPIOB_6

}

/*
*********************************************************************************************************
*  函 数 名: u8KeyScan
*  功能说明: 按键扫描
*  形    参: @mode (0,不支持连续按;1,支持连续按)
*  返 回 值: 按键键值
*********************************************************************************************************
*/
eKEY_VALUE eKeyScan(uint8_t mode)
{	
	static u8 key_up=1;//按键按松开标志
	static u16 key_long_cnt = 0;    //按键长按计数
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY1_READ()==0||KEY2_READ()==0||KEY3_READ()==0||KEY4_READ()==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY1_READ()==0)return KEY1_PRES_S;
		else if(KEY2_READ()==0)return KEY2_PRES_S;
		else if(KEY3_READ()==0)return KEY3_PRES_S;
		else if(KEY4_READ()==0)return KEY4_PRES_S;
	}
	else if(KEY1_READ()==1&&KEY2_READ()==1&&KEY3_READ()==1&&KEY4_READ()==1) //所有按键都释放了
	{
		key_up=1; 		
		return KEY_ALL_REL;
	}
	else
	{
		
		key_long_cnt ++;
		if(key_long_cnt > KEY_LONG_TIME)
		{
			key_long_cnt = 0;
			if(KEY1_READ()==0)return KEY1_PRES_L;
			else if(KEY2_READ()==0)return KEY2_PRES_L;
			else if(KEY3_READ()==0)return KEY3_PRES_L;
			else if(KEY4_READ()==0)return KEY4_PRES_L;
		}
	}
 	return KEY_NONE;// 无按键按下

}

