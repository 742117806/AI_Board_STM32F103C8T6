

#include "sn3218_api.h"

extern uint8_t LedCtrl1Vlaue;
extern uint8_t LedCtrl2Vlaue;
extern uint8_t LedCtrl3Vlaue;

uint8_t led_set_mode = 0;
LED_Color_t led_set_color = LED_COLOR_ALL;
uint8_t led_delay = 0;

uint8_t led_last_mode = 0;        //上次状态
LED_Color_t led_last_color = LED_COLOR_ALL;  //上次颜色


const uint8_t led_breath_buff[32]=
{
//	0	,1	,2	,4	,6	,10	,13	,18	,
//	22	,28	,33	,39	,46	,53	,61	,69	,
//	78	,86	,96	,106,116,126,138,149,
//	161	,173,186,199,212,226,240,255,

	0	,1	,2	,4	,5	,6	,8	,10	,
	13	,16	,20	,25	,29	,33	,36	,36	,
	36	,33	,29	,25,20,16,13,10,
	8	,6,5,4,3,2,1,0,
};

const uint8_t  led_flow_blue_buff[10]=
{
	1,3,5,7,11,13,15,17,1,3	
};

const uint8_t led_flow_orange_buff[10]=
{
	2,4,6,8,12,14,16,18,2,4
};

const uint8_t led_flow_buff[18]=
{
	1,2,3,4,5,6,7,8,11,12,13,14,15,16,17,18,1,2	
};

/******************************************************************************
功能：	所有灯状态设置
参数：	@sta 外围灯状态（0或者1）
返回值：无
******************************************************************************
*/
void LED_ALL_StaSet(uint8_t sta)
{
    SN3218_LedStaSet(0,sta);
    SN3218_RegRef();
}
/******************************************************************************
功能：	外围灯状态设置
参数：	@color_index 颜色()
		@sta 外围灯状态（0或者1）

返回值：无
******************************************************************************
*/
void LED_AroundStaSet(LED_Color_t color_index,uint8_t sta)
{
    uint8_t led_center_sta;		//中间灯当前的状态
    uint8_t i=0;
    led_center_sta = (LedCtrl2Vlaue & SN3218_LED9_ON_DATA ? 1:0);

    switch(color_index)
    {
    case LED_COLOR_ORANG:
        for(i=2; i<=18; i+=2)
        {
            SN3218_LedStaSet(i,sta);
        }
        break;
    case LED_COLOR_BLUE:
        for(i=1; i<=18; i+=2)
        {
            SN3218_LedStaSet(i,sta);
        }
        break;
    case LED_COLOR_ALL:
        for(i=1; i<=18; i++)
        {
            SN3218_LedStaSet(i,sta);
        }
        break;
    }

    SN3218_LedStaSet(9,led_center_sta);
    SN3218_RegRef();
}

/******************************************************************************
功能：	外围灯亮度设置
参数：	@color_index 颜色()
		@pwm 亮度值（0-255）

返回值：无
******************************************************************************
*/
void LED_AroundPwmSet(LED_Color_t color_index,uint8_t pwm)
{

    uint8_t i=0;


    switch(color_index)
    {
    case LED_COLOR_ORANG:
        for(i=2; i<=18; i+=2)
        {
            SN3218_LedPwmSet(i,pwm);
        }
        break;
    case LED_COLOR_BLUE:
        for(i=1; i<=18; i+=2)
        {
            SN3218_LedPwmSet(i,pwm);
        }
        break;
    case LED_COLOR_ALL:
        for(i=1; i<=18; i++)
        {
            SN3218_LedPwmSet(i,pwm);
        }
        break;
    }

    SN3218_LedPwmSet(9,10);
    SN3218_RegRef();
}


/******************************************************************************
功能：	中间灯状态设置
参数：	@sta 状态（0或者1）

返回值：无
******************************************************************************
*/
void LED_CenterStaSet(uint8_t sta)
{
    SN3218_LedStaSet(9,sta);
    SN3218_RegRef();
}


/******************************************************************************
功能：	中间灯亮度设置
参数：	@pwm 亮度值（0-255）

返回值：无
******************************************************************************
*/
void LED_CenterPwmSet(uint8_t pwm)
{
    SN3218_LedPwmSet(9,pwm);
    SN3218_RegRef();
}


/******************************************************************************
功能：	流水灯
参数：	@color 颜色
        @speed 流水灯速度
返回值：无
******************************************************************************
*/
void LED_AroundFlow(LED_Color_t color,uint8_t speed)
{	
	static uint8_t i = 0;
	static uint8_t speed_cnt = 0;
	
	speed_cnt ++;
	if(speed_cnt > speed)	
	{
		speed_cnt = 0;
		if(color == LED_COLOR_ORANG)	
		{
			SN3218_LedStaSet(led_flow_orange_buff[i],0);
			SN3218_LedStaSet(led_flow_orange_buff[i+1],1);
			SN3218_LedStaSet(led_flow_orange_buff[i+2],1);
		}
		else if(color == LED_COLOR_BLUE)
		{
			SN3218_LedStaSet(led_flow_blue_buff[i],0);
			SN3218_LedStaSet(led_flow_blue_buff[i+1],1);
			SN3218_LedStaSet(led_flow_blue_buff[i+2],1);
		}
		else if(color == LED_COLOR_ALL)
		{
			SN3218_LedStaSet(led_flow_orange_buff[i],0);
			SN3218_LedStaSet(led_flow_orange_buff[i+1],1);
			SN3218_LedStaSet(led_flow_orange_buff[i+2],1);
			SN3218_LedStaSet(led_flow_blue_buff[i],0);
			SN3218_LedStaSet(led_flow_blue_buff[i+1],1);
			SN3218_LedStaSet(led_flow_blue_buff[i+2],1);	
		}
		SN3218_RegRef();
		i++;
		if(i>=8)i=0;
	}
}



/******************************************************************************
功能：	呼吸灯
参数：	@color 颜色

返回值：无
******************************************************************************
*/
void LED_AroundBreath(LED_Color_t color,uint8_t speed)
{
   	static uint8_t i = 0;
	static uint8_t j = 0;
	
	
	
	j++;
	if(j > speed)
	{
		j = 0;
		LED_AroundPwmSet(color,led_breath_buff[i]);
		i ++;
		if(i>=32)i=0;
	}
	LED_AroundStaSet(color,ON);
	SN3218_RegRef();
}


/******************************************************************************
功能：	按键1指示亮灯
参数：	@color 颜色

返回值：无
******************************************************************************
*/
void LedDispKey1(LED_Color_t color,uint8_t sta)
{
    //SN3218_LedStaSet(0,OFF);		//所有LED灭
	switch(color)
	{
	case LED_COLOR_ORANG:
		SN3218_LedStaSet(2,sta);
		SN3218_LedStaSet(4,sta);	
		break;
	case LED_COLOR_BLUE:
		SN3218_LedStaSet(1,sta);
		SN3218_LedStaSet(3,sta);	
		break;
	case LED_COLOR_ALL:
		SN3218_LedStaSet(1,sta);
		SN3218_LedStaSet(3,sta);
		SN3218_LedStaSet(2,sta);
		SN3218_LedStaSet(4,sta);	
		break;
	}

	SN3218_RegRef();
}

/******************************************************************************
功能：	按键3指示亮灯
参数：	@color 颜色

返回值：无
******************************************************************************
*/
void LedDispKey3(LED_Color_t color,uint8_t sta)
{
    //SN3218_LedStaSet(0,OFF);		//所有LED灭
	switch(color)
	{
	case LED_COLOR_ORANG:
		SN3218_LedStaSet(12,sta);
		SN3218_LedStaSet(14,sta);	
		break;
	case LED_COLOR_BLUE:
	
		SN3218_LedStaSet(11,sta);
		SN3218_LedStaSet(13,sta);	
		break;
	case LED_COLOR_ALL:
		SN3218_LedStaSet(11,sta);
		SN3218_LedStaSet(13,sta);
		SN3218_LedStaSet(12,sta);
		SN3218_LedStaSet(14,sta);	
		break;
	}

	SN3218_RegRef();
}

/******************************************************************************
功能：	LED音量指示
参数：	@color 颜色
        @vol 音量值(取值：0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E)
返回值：无
******************************************************************************
*/
void LedDispVol(uint8_t vol)
{
	uint8_t i;
	uint8_t led_index = 6;
 
	SN3218_LedStaSet(0,OFF);		//所有LED灭
	SN3218A_WriteCont(SN3218_PWM1_REG,SN3218_PWM18_REG,LED_AROUND_LIGHT_VALUE);   //初始化所有LED的PWM值
	for(i = 0;i < (vol/2)+1;i++)
	{	
		if(led_index == 4) led_index = 5;
		SN3218_LedStaSet(led_index*2+1,ON);
		SN3218_LedStaSet(led_index*2+2,ON);
		led_index ++;		
		if(led_index > 8) led_index = 0;
	}

	LED_CenterStaSet(ON);
	SN3218_RegRef();
}








