

#include  "sn3218_drv.h"

#define LedBright_Select 1
uint8_t LedCtrl1Vlaue=0x3f;
uint8_t LedCtrl2Vlaue=0x3f;
uint8_t LedCtrl3Vlaue=0x3f;
//
/*
*********************************************************************************************************
*  函 数 名: SN3218_PinConfig
*  功能说明: SN3218管脚初始化
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void SN3218_PinConfig(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
}

/**
******************************************************************************
功能：	模拟I2C
参数：	无
返回值：无
******************************************************************************
*/
//I2C开始
void I2C_Start(void)
{

    I2C_SDA_H();
    I2C_SCL_H();
    delay_us(1);
    //__NOP;
    I2C_SDA_L();
    delay_us(1);
    I2C_SCL_L();
}

//I2C停止
void I2C_Stop(void)
{
    I2C_SDA_L();
    I2C_SCL_H();
    delay_us(1);
    //__NOP;
    I2C_SDA_H();
    delay_us(1);
    I2C_SCL_L();
}

//I2C写数据
void I2C_Write1Byte(uint8_t buf)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        I2C_SCL_L();
        if (buf & 0x80)
            I2C_SDA_H();
        else
            I2C_SDA_L();
        delay_us(1);
        I2C_SCL_H();
        delay_us(1);
        buf <<= 1;
    }
    I2C_SCL_L();
    delay_us(1);
    I2C_SDA_H();
    delay_us(1);
    I2C_SCL_H();
    delay_us(1);
    while (I2C_SDA_RD() && (i < 255))
        i++;

    I2C_SCL_L();
    delay_us(1);
}


/**
******************************************************************************
功能：	寄存器设置
参数：	@regAddr 寄存器地址
		@value 寄存器值
返回值：无
******************************************************************************
*/
void SN3218A_Write(uint8_t regAddr, uint8_t value)
{
    I2C_Start();
    I2C_Write1Byte(SN3218_ADDR);
    I2C_Write1Byte(regAddr);
    I2C_Write1Byte(value);
    I2C_Stop();
}

/**
******************************************************************************
功能：	寄存器连续地址设置
参数：	@reg_startAddr 寄存开始器地址
		@reg_endAddr 寄存器结束地址
		@value 寄存器值
返回值：无
******************************************************************************
*/
void SN3218A_WriteCont(uint8_t reg_startAddr,uint8_t reg_endAddr, uint8_t value)
{
    uint8_t i=0;
    uint8_t reg_num = 0;

    if(reg_endAddr < reg_startAddr) return; //参数输入有误

    reg_num =  (reg_endAddr - reg_startAddr)+1;  //连续寄存器的个数

    I2C_Start();
    I2C_Write1Byte(SN3218_ADDR);		//器件地址

    I2C_Write1Byte(reg_startAddr);      //寄存器开始地址
    for(i=0; i< reg_num; i++)
    {
        I2C_Write1Byte(value);              //写入的值
    }
    I2C_Stop();
}

void SN3218_RegRef(void)
{
	SN3218A_Write(SN3218_REF_REG, 0x00);	//刷新寄存器值生效
}


/**
******************************************************************************
功能：	SN3218A 初始化
参数：	无
返回值：无
******************************************************************************
*/
void SN3218_Init(void)
{

    SN3218_PinConfig();  	//相关连接管脚配置
    SN3218_PowerOn();		//上电
    delay_ms(10);

    SN3218A_Write(SN3218_REST_REG, 0x01);	//复位寄存器为默认值
    SN3218A_Write(SN3218_OFF_REG, 0x01);    //寄存器打开
    SN3218A_Write(SN3218_LED_CTRL1_REG, LedCtrl1Vlaue);   	//初始化所有LED的开关状态
    SN3218A_Write(SN3218_LED_CTRL2_REG, LedCtrl2Vlaue);
    SN3218A_Write(SN3218_LED_CTRL3_REG, LedCtrl3Vlaue);
    SN3218A_WriteCont(SN3218_PWM1_REG,SN3218_PWM18_REG,LED_AROUND_LIGHT_VALUE);   //初始化所有LED的PWM值
	SN3218_RegRef();	//刷新寄存器值生效
}


/**
******************************************************************************
功能：	设置LED灯的状态
参数：	@led LED序号,1到18标识各个LED灯，0标识全部LED灯
		@sta LED状态（0或者1）
返回值：无
******************************************************************************
*/
void SN3218_LedStaSet(uint8_t led,uint8_t sta)
{
    switch(led)
    {
	case 0:
		if(sta == 0)
		{
			SN3218A_WriteCont(SN3218_LED_CTRL1_REG,SN3218_LED_CTRL3_REG,0x00);   //所有LED状态关闭
			LedCtrl1Vlaue = 0x00;
			LedCtrl2Vlaue = 0x00;
			LedCtrl3Vlaue = 0x00;
		}
		else
		{
			SN3218A_WriteCont(SN3218_LED_CTRL1_REG,SN3218_LED_CTRL3_REG,0x3f);   //打开LED状态关闭
			LedCtrl1Vlaue = 0x3f;
			LedCtrl2Vlaue = 0x3f;
			LedCtrl3Vlaue = 0x3f;

		}
		break;
    case 1:	
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
		if(sta == 0)
		{
			LedCtrl1Vlaue = LedCtrl1Vlaue&(~(1<<(led-1)));			
		}
		else
		{			
			LedCtrl1Vlaue = LedCtrl1Vlaue|(1<<(led-1));
		}
		SN3218A_Write(SN3218_LED_CTRL1_REG, LedCtrl1Vlaue);   	
        break;

    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
		if(sta == 0)
		{
			LedCtrl2Vlaue = LedCtrl2Vlaue&(~(1<<(led-7)));
		}
		else
		{
			LedCtrl2Vlaue = LedCtrl2Vlaue|(1<<(led-7));
			
		}
		SN3218A_Write(SN3218_LED_CTRL2_REG, LedCtrl2Vlaue);  
        break;
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
	case 18:
		if(sta == 0)
		{    
			LedCtrl3Vlaue = LedCtrl3Vlaue&(~(1<<(led-13)));	
		}
		else
		{
			LedCtrl3Vlaue = LedCtrl3Vlaue|(1<<(led-13));
		}
		SN3218A_Write(SN3218_LED_CTRL3_REG, LedCtrl3Vlaue);  
        break;
    default:
        break;

    }
//	SN3218A_Write(SN3218_REF_REG, 0x00);	//刷新寄存器值生效
}
/**
******************************************************************************
功能：	设置LED灯的亮度值
参数：	@led LED序号,1到18标识各个LED灯，0标识全部LED灯
		@pwm LED亮度值（0-255）
返回值：无
******************************************************************************
*/
void SN3218_LedPwmSet(uint8_t led,uint8_t pwm)
{

	if(led>18) return;
	if(led == 0)
	{
		SN3218A_WriteCont(SN3218_PWM1_REG,SN3218_PWM18_REG, pwm); 
	}
	else
	{
		SN3218A_Write(led, pwm); 
	}
//	SN3218A_Write(SN3218_REF_REG, 0x00);	//刷新寄存器值生效	
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
//void LED_LightCtrl(uint32_t ledBits, uint8_t ledVal)
//{
//  uint8_t i;
//  uint32_t bit_map = 0X00000001;

//  for (i = 1; i <= 18; i++) //一共有18路LED
//  {
//    if ((ledBits & bit_map) == bit_map) //判端是否要控制该路LED
//    {
//      SN3218A_RegWirte(i, ledVal);
//    }
//    //		else
//    //		{
//    //			if((i!=9)&&(i!=10))		//跳过中间灯控制，避免熄灭了中间灯
//    //			{
//    //				SN3218A_RegWirte(i,0);		//熄灭该路LED
//    //			}
//    //		}
//    bit_map <<= 1; //切换到下一路LED
//  }

//  SN3218A_RegWirte(Addr_DataRefresh, 0x55); //刷新寄存器数据
//}
///**
//******************************************************************************
//功能：	LED 控制各路开关
//参数：	@ledBits 每一位分别表示每一路LED(1开，0关)
//返回值：无
//******************************************************************************
//*/
//void LED_Switch(uint32_t ledBits)
//{

//  SN3218A_RegWirte(0X13, (ledBits & 0x0000003F));
//  SN3218A_RegWirte(0X14, (ledBits & 0x00000FC0) >> 6);
//  SN3218A_RegWirte(0X15, (ledBits & 0x0003F000) >> 12);
//  SN3218A_RegWirte(Addr_DataRefresh, 0x55); //刷新寄存器数据
//}
///**
//******************************************************************************
//功能：	外围灯熄灭
//参数：	无

//返回值：无
//******************************************************************************
//*/
//void LED_AroundOff(void)
//{
//  LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS, 0);
//  LED_Switch(0x300);
//}

///**
//******************************************************************************
//功能：	外围灯点亮
//参数：	@color  点亮的颜色

//返回值：无
//******************************************************************************
//*/
//void LED_AroundOn(LedColor_e color)
//{

//  LED_LightCtrl(LED_CENTRE_MAP_BITS, LED_CENTRE_LIGHT_VALUE);
//  if (color == BLUE)
//  {
//    LED_LightCtrl(LED_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //控制亮度
//    LED_Switch(LED_BLUE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//  else if (color == RED_OR_ORANGE)
//  {
//    LED_LightCtrl(LED_ORANGE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //控制亮度
//    LED_Switch(LED_ORANGE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//  else if (color == GREEN_OR_PURPLE)
//  {
//    LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //控制亮度
//    LED_Switch(LED_ORANGE_BLUE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//}

///**
//******************************************************************************
//功能：	外围灯按个点亮,用来指示音量大小
//参数：	@vol  音量大小命令值，取值范围：0-14

//返回值：无
//******************************************************************************
//*/
//const uint32_t ledVolIndexArray[8] = {0x3000, 0xF000, 0x3F000, 0x3F003, 0x3F00F, 0x3F03F, 0x3F0FF, 0x3FCFF};
//void LED_AroundVolIndex(uint8_t vol)
//{
//  if (vol > 15)
//    return;
//  LED_LightCtrl(LED_CENTRE_MAP_BITS, LED_CENTRE_LIGHT_VALUE);
//  LED_LightCtrl(ledVolIndexArray[vol / 2], LED_AROUND_LIGHT_VALUE); //控制亮度
//  LED_Switch(ledVolIndexArray[vol / 2] | LED_CENTRE_MAP_BITS);
//}

////
///****************************************************************
//*功能：非阻塞LED呼吸灯模式
//*参数：	@ledBits 需要控制的LED对应的路
//		@brightMax 亮度
//*	   	@speed 速度
//*****************************************************************/
//void LED_Breath(uint32_t ledBits, uint8_t brightMax, uint8_t speed)
//{
//  static uint8_t light = 0; //PWM值
//  static uint8_t dir;       //变化方向
//  static uint8_t time_cnt = 0;
//  uint8_t light_cut = 0;

//  time_cnt++;

//  if (time_cnt > speed)
//  {
//    time_cnt = 0;
//    LED_Switch(ledBits | LED_CENTRE_MAP_BITS); //打开所有LED灯,|LED_CENTRE_MAP_BITS为了排除中间灯的控制

//    if (light > 7)
//    {
//      light_cut = 7;
//    }
//    else
//    {
//      light_cut = light;
//    }
//    LED_LightCtrl(ledBits, light_cut); //控制亮度

//    if (light == brightMax)
//    {
//      dir = 1;
//    }
//    else if (light == 0)
//    {
//      dir = 0;
//    }

//    if (dir == 1)
//    {
//      light--;
//    }
//    else
//    {
//      light++;
//    }
//  }
//}

///****************************************************************
//*功能：流水灯
//*参数：	@bright 亮度
//*	   	@speed 速度
//*****************************************************************/
//void LED_Flow(LedColor_e color, uint8_t bright, uint8_t speed)
//{
//  uint8_t i;

//  uint32_t bits = 0x01;
//  uint32_t lastBits = 0x010000;

//  if (color == BLUE)
//  {
//    bits = 0x01;
//    lastBits = 0x010000;
//  }
//  else if (color == RED_OR_ORANGE)
//  {
//    bits = 0x02;
//    lastBits = 0x020000;
//  }

//  LED_Switch(0x00000000 | LED_CENTRE_MAP_BITS);      //先关闭所有灯
//  LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS, bright);   //控制亮度
//  LED_Switch(bits | LED_CENTRE_MAP_BITS | lastBits); //打开所有LED灯,|LED_CENTRE_MAP_BITS为了排除中间灯的控制

//  osDelay(speed);
//  LED_Switch(bits | LED_CENTRE_MAP_BITS);
//  osDelay(speed * 6);
//  for (i = 0; i < 8; i++)
//  {
//    if (i != 4) //避开中间那颗灯
//    {
//      lastBits = bits;
//    }

//    bits <<= 2;
//    if (i == 3)
//      continue;

//    LED_Switch(bits | LED_CENTRE_MAP_BITS | lastBits);
//    osDelay(speed);
//    LED_Switch(bits | LED_CENTRE_MAP_BITS);
//    osDelay(speed * 6);
//  }
//}

//void LED_CentreOn(void)
//{
//  uint16_t i = 0;
//  for (i = 0; i < LED_CENTRE_LIGHT_VALUE; i++)
//  {
//    LED_LightCtrl(LED_CENTRE_MAP_BITS, i);
//    if (i < 255)
//      i++;
//    delay_ms(5);
//  }
//}

//void LED_Key1Show(LedColor_e color)
//{
//  LED_LightCtrl(LED_KEY1_ORANGE_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //控制亮度
//  LED_LightCtrl(LED_CENTRE_MAP_BITS, LED_CENTRE_LIGHT_VALUE);
//  if (color == RED_OR_ORANGE)
//  {
//    LED_Switch(LED_KEY1_ORANGE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//  else if (color == BLUE)
//  {

//    LED_Switch(LED_KEY1_BLUE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//}

//void LED_Key3Show(LedColor_e color)
//{
//  LED_LightCtrl(LED_KEY3_ORANGE_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //控制亮度
//  LED_LightCtrl(LED_CENTRE_MAP_BITS, LED_CENTRE_LIGHT_VALUE);
//  if (color == RED_OR_ORANGE)
//  {
//    LED_Switch(LED_KEY3_ORANGE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//  else if (color == BLUE)
//  {

//    LED_Switch(LED_KEY3_BLUE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//}
