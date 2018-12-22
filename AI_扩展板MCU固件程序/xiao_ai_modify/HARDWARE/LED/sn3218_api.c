

#include "sn3218_api.h"

extern uint8_t LedCtrl1Vlaue;
extern uint8_t LedCtrl2Vlaue;
extern uint8_t LedCtrl3Vlaue;

uint8_t led_set_mode = 0;
LED_Color_t led_set_color = LED_COLOR_ALL;
uint8_t led_delay = 0;

uint8_t led_last_mode = 0;        //�ϴ�״̬
LED_Color_t led_last_color = LED_COLOR_ALL;  //�ϴ���ɫ


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
���ܣ�	���е�״̬����
������	@sta ��Χ��״̬��0����1��
����ֵ����
******************************************************************************
*/
void LED_ALL_StaSet(uint8_t sta)
{
    SN3218_LedStaSet(0,sta);
    SN3218_RegRef();
}
/******************************************************************************
���ܣ�	��Χ��״̬����
������	@color_index ��ɫ()
		@sta ��Χ��״̬��0����1��

����ֵ����
******************************************************************************
*/
void LED_AroundStaSet(LED_Color_t color_index,uint8_t sta)
{
    uint8_t led_center_sta;		//�м�Ƶ�ǰ��״̬
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
���ܣ�	��Χ����������
������	@color_index ��ɫ()
		@pwm ����ֵ��0-255��

����ֵ����
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
���ܣ�	�м��״̬����
������	@sta ״̬��0����1��

����ֵ����
******************************************************************************
*/
void LED_CenterStaSet(uint8_t sta)
{
    SN3218_LedStaSet(9,sta);
    SN3218_RegRef();
}


/******************************************************************************
���ܣ�	�м����������
������	@pwm ����ֵ��0-255��

����ֵ����
******************************************************************************
*/
void LED_CenterPwmSet(uint8_t pwm)
{
    SN3218_LedPwmSet(9,pwm);
    SN3218_RegRef();
}


/******************************************************************************
���ܣ�	��ˮ��
������	@color ��ɫ
        @speed ��ˮ���ٶ�
����ֵ����
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
���ܣ�	������
������	@color ��ɫ

����ֵ����
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
���ܣ�	����1ָʾ����
������	@color ��ɫ

����ֵ����
******************************************************************************
*/
void LedDispKey1(LED_Color_t color,uint8_t sta)
{
    //SN3218_LedStaSet(0,OFF);		//����LED��
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
���ܣ�	����3ָʾ����
������	@color ��ɫ

����ֵ����
******************************************************************************
*/
void LedDispKey3(LED_Color_t color,uint8_t sta)
{
    //SN3218_LedStaSet(0,OFF);		//����LED��
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
���ܣ�	LED����ָʾ
������	@color ��ɫ
        @vol ����ֵ(ȡֵ��0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E)
����ֵ����
******************************************************************************
*/
void LedDispVol(uint8_t vol)
{
	uint8_t i;
	uint8_t led_index = 6;
 
	SN3218_LedStaSet(0,OFF);		//����LED��
	SN3218A_WriteCont(SN3218_PWM1_REG,SN3218_PWM18_REG,LED_AROUND_LIGHT_VALUE);   //��ʼ������LED��PWMֵ
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








