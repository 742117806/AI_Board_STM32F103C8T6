#include "stm32f1xx.h"
#include "led.h"
#include "frame_process.h"
#include "sn3218a_i2c.h"
#include "delay.h"
#include "cmsis_os.h"


#define LedBright_Select                    1

/**
******************************************************************************
功能：	SN3218A 初始化
参数：	无
返回值：无
******************************************************************************
*/
void SN3218_Init(void)
{
	uint8_t i=0;
	SN3218_PowerOn();
	delay_ms(10);	

	SN3218A_RegWirte(Addr_Reset, 0x01);
	SN3218A_RegWirte(Addr_All_Switch, 0x01);
	SN3218A_RegWirte(Addr_PWM1_6_Switch, 0x3f);
	SN3218A_RegWirte(Addr_PWM7_12_Switch, 0x3f);
	SN3218A_RegWirte(Addr_PWM13_18_Switch, 0x3f);	
	
	I2C_Start();
	I2C_Write1Byte(SN3218_ADDR);
	I2C_Write1Byte(0x01);	
	for(i=0;i<18;i++)
	{

		I2C_Write1Byte(15);        //第一路LED亮度30，接着后面的各路
	}
	I2C_Stop();
	
    SN3218A_RegWirte(Addr_DataRefresh,0x55);	 
}
/**
******************************************************************************
功能：	寄存器设置
参数：	@regAddr 寄存器地址	
		@value 寄存器值
返回值：无
******************************************************************************
*/
void SN3218A_RegWirte(uint8_t regAddr,uint8_t value)
{
  	I2C_Start();
	I2C_Write1Byte(SN3218_ADDR);
	I2C_Write1Byte(regAddr);
	I2C_Write1Byte(value);
	I2C_Stop();
}
void LED_PowerOff()
{

}

/**
******************************************************************************
功能：	LED 控制
参数：	@ledBits 每一位分别表示每一路LED(1表示选择，0表示不选择)	
		@ledVal 寄存器值
返回值：无
******************************************************************************
*/
//
void LED_LightCtrl(uint32_t ledBits,uint8_t ledVal)
{
	uint8_t i;
	uint32_t bit_map = 0X00000001;

	for(i=1;i<=18;i++)		//一共有18路LED
	{
		if((ledBits & bit_map) == bit_map)  //判端是否要控制该路LED
		{
			SN3218A_RegWirte(i,ledVal);
		}
//		else
//		{
//			if((i!=9)&&(i!=10))		//跳过中间灯控制，避免熄灭了中间灯
//			{
//				SN3218A_RegWirte(i,0);		//熄灭该路LED
//			}
//		}
		bit_map <<= 1;		//切换到下一路LED

	}
	
	SN3218A_RegWirte(Addr_DataRefresh, 0x55); 	//刷新寄存器数据
}
/**
******************************************************************************
功能：	LED 控制各路开关
参数：	@ledBits 每一位分别表示每一路LED(1开，0关)	
返回值：无
******************************************************************************
*/
void LED_Switch(uint32_t ledBits)
{
	
	SN3218A_RegWirte(0X13,(ledBits&0x0000003F));
	SN3218A_RegWirte(0X14,(ledBits&0x00000FC0)>>6);
	SN3218A_RegWirte(0X15,(ledBits&0x0003F000)>>12);	
	SN3218A_RegWirte(Addr_DataRefresh, 0x55); 	//刷新寄存器数据
}
/**
******************************************************************************
功能：	外围灯熄灭 
参数：	无

返回值：无
******************************************************************************
*/
void LED_AroundOff(void)
{
	LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS,0);
	LED_Switch(0x300);	
}



/**
******************************************************************************
功能：	外围灯点亮
参数：	@color  点亮的颜色

返回值：无
******************************************************************************
*/
void LED_AroundOn(LedColor_e color)
{
	
	LED_LightCtrl(LED_CENTRE_MAP_BITS,LED_CENTRE_LIGHT_VALUE);
    if(color == BLUE)
	{
		LED_LightCtrl(LED_BLUE_MAP_BITS,LED_AROUND_LIGHT_VALUE);  //控制亮度
		LED_Switch(LED_BLUE_MAP_BITS|LED_CENTRE_MAP_BITS);
	}
	else if(color == RED_OR_ORANGE)
	{
		LED_LightCtrl(LED_ORANGE_MAP_BITS,LED_AROUND_LIGHT_VALUE);  //控制亮度
		LED_Switch(LED_ORANGE_MAP_BITS|LED_CENTRE_MAP_BITS);
	}
	else if(color == GREEN_OR_PURPLE)
	{
	   LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS,LED_AROUND_LIGHT_VALUE);  //控制亮度
	   LED_Switch(LED_ORANGE_BLUE_MAP_BITS|LED_CENTRE_MAP_BITS);
	}
}

/**
******************************************************************************
功能：	外围灯按个点亮,用来指示音量大小
参数：	@vol  音量大小命令值，取值范围：0-14

返回值：无
******************************************************************************
*/
const uint32_t ledVolIndexArray[8]={0x3000,0xF000,0x3F000,0x3F003,0x3F00F,0x3F03F,0x3F0FF,0x3FCFF};
void LED_AroundVolIndex(uint8_t vol)
{
	if(vol>15)return;
	LED_LightCtrl(LED_CENTRE_MAP_BITS,LED_CENTRE_LIGHT_VALUE);
	LED_LightCtrl(ledVolIndexArray[vol/2],LED_AROUND_LIGHT_VALUE);  //控制亮度
	LED_Switch(ledVolIndexArray[vol/2]|LED_CENTRE_MAP_BITS);
}


//
/****************************************************************
*功能：非阻塞LED呼吸灯模式
*参数：	@ledBits 需要控制的LED对应的路
		@brightMax 亮度
*	   	@speed 速度
*****************************************************************/
void LED_Breath(uint32_t ledBits, uint8_t brightMax,uint8_t speed)
{
	static uint8_t light = 0; //PWM值
	static uint8_t dir;   //变化方向
	static uint8_t time_cnt = 0;
	
	time_cnt ++;

	if(time_cnt > speed)
	{
		time_cnt = 0;
		LED_Switch(ledBits|LED_CENTRE_MAP_BITS);		//打开所有LED灯,|LED_CENTRE_MAP_BITS为了排除中间灯的控制
		
		LED_LightCtrl(ledBits,light);  //控制亮度
		
		if(light == brightMax)
		{
			dir = 1;
		}
		else if(light == 0)
		{
			dir = 0;
		}
		
		if(dir == 1)
		{
			light --;
		}
		else
		{
		   light ++;
		}
	}

}

/****************************************************************
*功能：流水灯
*参数：	@bright 亮度
*	   	@speed 速度
*****************************************************************/
void LED_Flow(LedColor_e color,uint8_t bright,uint8_t speed)
{
	uint8_t i;

	uint32_t bits = 0x01;
	uint32_t lastBits = 0x010000;
	
	
	if(color == BLUE)
	{
		bits = 0x01;
		lastBits = 0x010000;
	}
	else  if(color == RED_OR_ORANGE)
	{
		bits = 0x02;
		lastBits = 0x020000;
	}
	
	LED_Switch(0x00000000|LED_CENTRE_MAP_BITS); //先关闭所有灯
	LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS,bright);  //控制亮度
	LED_Switch(bits|LED_CENTRE_MAP_BITS|lastBits);		//打开所有LED灯,|LED_CENTRE_MAP_BITS为了排除中间灯的控制	
			

	osDelay(speed);
	LED_Switch(bits|LED_CENTRE_MAP_BITS);
	osDelay(speed*6);   
	for(i=0;i<8;i++)
	{
		if(i!=4)		//避开中间那颗灯
		{
		  lastBits = bits;
		}
		
		bits <<= 2;
		if(i==3)continue;
		
		
		LED_Switch(bits|LED_CENTRE_MAP_BITS|lastBits);
		osDelay(speed);
		LED_Switch(bits|LED_CENTRE_MAP_BITS);
		osDelay(speed*6);
	}
   
}


void LED_CentreOn(void)
{
   uint16_t i = 0;
	for(i=0;i<LED_CENTRE_LIGHT_VALUE;i++)
	{
		LED_LightCtrl(LED_CENTRE_MAP_BITS,i);
		if(i<255)i++;
		delay_ms(5);
	}
}

void LED_Key1Show(LedColor_e color)
{
	LED_LightCtrl(LED_KEY1_ORANGE_BLUE_MAP_BITS,LED_AROUND_LIGHT_VALUE);  //控制亮度
	LED_LightCtrl(LED_CENTRE_MAP_BITS,LED_CENTRE_LIGHT_VALUE);
	if(color == RED_OR_ORANGE)
	{
		LED_Switch(LED_KEY1_ORANGE_MAP_BITS|LED_CENTRE_MAP_BITS);
	}
	else if(color == BLUE)
	{
	
		LED_Switch(LED_KEY1_BLUE_MAP_BITS|LED_CENTRE_MAP_BITS);
	}
	
}

void LED_Key3Show(LedColor_e color)
{
	LED_LightCtrl(LED_KEY3_ORANGE_BLUE_MAP_BITS,LED_AROUND_LIGHT_VALUE);  //控制亮度
	LED_LightCtrl(LED_CENTRE_MAP_BITS,LED_CENTRE_LIGHT_VALUE);
	if(color == RED_OR_ORANGE)
	{
		LED_Switch(LED_KEY3_ORANGE_MAP_BITS|LED_CENTRE_MAP_BITS);
	}
	else if(color == BLUE)
	{
	
		LED_Switch(LED_KEY3_BLUE_MAP_BITS|LED_CENTRE_MAP_BITS);
	}
	
}













