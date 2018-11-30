

#include  "sn3218_drv.h"

#define LedBright_Select 1
uint8_t LedCtrl1Vlaue=0x3f;
uint8_t LedCtrl2Vlaue=0x3f;
uint8_t LedCtrl3Vlaue=0x3f;
//
/*
*********************************************************************************************************
*  �� �� ��: SN3218_PinConfig
*  ����˵��: SN3218�ܽų�ʼ��
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void SN3218_PinConfig(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��
}

/**
******************************************************************************
���ܣ�	ģ��I2C
������	��
����ֵ����
******************************************************************************
*/
//I2C��ʼ
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

//I2Cֹͣ
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

//I2Cд����
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
���ܣ�	�Ĵ�������
������	@regAddr �Ĵ�����ַ
		@value �Ĵ���ֵ
����ֵ����
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
���ܣ�	�Ĵ���������ַ����
������	@reg_startAddr �Ĵ濪ʼ����ַ
		@reg_endAddr �Ĵ���������ַ
		@value �Ĵ���ֵ
����ֵ����
******************************************************************************
*/
void SN3218A_WriteCont(uint8_t reg_startAddr,uint8_t reg_endAddr, uint8_t value)
{
    uint8_t i=0;
    uint8_t reg_num = 0;

    if(reg_endAddr < reg_startAddr) return; //������������

    reg_num =  (reg_endAddr - reg_startAddr)+1;  //�����Ĵ����ĸ���

    I2C_Start();
    I2C_Write1Byte(SN3218_ADDR);		//������ַ

    I2C_Write1Byte(reg_startAddr);      //�Ĵ�����ʼ��ַ
    for(i=0; i< reg_num; i++)
    {
        I2C_Write1Byte(value);              //д���ֵ
    }
    I2C_Stop();
}

void SN3218_RegRef(void)
{
	SN3218A_Write(SN3218_REF_REG, 0x00);	//ˢ�¼Ĵ���ֵ��Ч
}


/**
******************************************************************************
���ܣ�	SN3218A ��ʼ��
������	��
����ֵ����
******************************************************************************
*/
void SN3218_Init(void)
{

    SN3218_PinConfig();  	//������ӹܽ�����
    SN3218_PowerOn();		//�ϵ�
    delay_ms(10);

    SN3218A_Write(SN3218_REST_REG, 0x01);	//��λ�Ĵ���ΪĬ��ֵ
    SN3218A_Write(SN3218_OFF_REG, 0x01);    //�Ĵ�����
    SN3218A_Write(SN3218_LED_CTRL1_REG, LedCtrl1Vlaue);   	//��ʼ������LED�Ŀ���״̬
    SN3218A_Write(SN3218_LED_CTRL2_REG, LedCtrl2Vlaue);
    SN3218A_Write(SN3218_LED_CTRL3_REG, LedCtrl3Vlaue);
    SN3218A_WriteCont(SN3218_PWM1_REG,SN3218_PWM18_REG,LED_AROUND_LIGHT_VALUE);   //��ʼ������LED��PWMֵ
	SN3218_RegRef();	//ˢ�¼Ĵ���ֵ��Ч
}


/**
******************************************************************************
���ܣ�	����LED�Ƶ�״̬
������	@led LED���,1��18��ʶ����LED�ƣ�0��ʶȫ��LED��
		@sta LED״̬��0����1��
����ֵ����
******************************************************************************
*/
void SN3218_LedStaSet(uint8_t led,uint8_t sta)
{
    switch(led)
    {
	case 0:
		if(sta == 0)
		{
			SN3218A_WriteCont(SN3218_LED_CTRL1_REG,SN3218_LED_CTRL3_REG,0x00);   //����LED״̬�ر�
			LedCtrl1Vlaue = 0x00;
			LedCtrl2Vlaue = 0x00;
			LedCtrl3Vlaue = 0x00;
		}
		else
		{
			SN3218A_WriteCont(SN3218_LED_CTRL1_REG,SN3218_LED_CTRL3_REG,0x3f);   //��LED״̬�ر�
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
//	SN3218A_Write(SN3218_REF_REG, 0x00);	//ˢ�¼Ĵ���ֵ��Ч
}
/**
******************************************************************************
���ܣ�	����LED�Ƶ�����ֵ
������	@led LED���,1��18��ʶ����LED�ƣ�0��ʶȫ��LED��
		@pwm LED����ֵ��0-255��
����ֵ����
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
//	SN3218A_Write(SN3218_REF_REG, 0x00);	//ˢ�¼Ĵ���ֵ��Ч	
}


/**
******************************************************************************
���ܣ�	LED ����
������	@ledBits ÿһλ�ֱ��ʾÿһ·LED(1��ʾѡ��0��ʾ��ѡ��)
		@ledVal �Ĵ���ֵ
����ֵ����
******************************************************************************
*/
//
//void LED_LightCtrl(uint32_t ledBits, uint8_t ledVal)
//{
//  uint8_t i;
//  uint32_t bit_map = 0X00000001;

//  for (i = 1; i <= 18; i++) //һ����18·LED
//  {
//    if ((ledBits & bit_map) == bit_map) //�ж��Ƿ�Ҫ���Ƹ�·LED
//    {
//      SN3218A_RegWirte(i, ledVal);
//    }
//    //		else
//    //		{
//    //			if((i!=9)&&(i!=10))		//�����м�ƿ��ƣ�����Ϩ�����м��
//    //			{
//    //				SN3218A_RegWirte(i,0);		//Ϩ���·LED
//    //			}
//    //		}
//    bit_map <<= 1; //�л�����һ·LED
//  }

//  SN3218A_RegWirte(Addr_DataRefresh, 0x55); //ˢ�¼Ĵ�������
//}
///**
//******************************************************************************
//���ܣ�	LED ���Ƹ�·����
//������	@ledBits ÿһλ�ֱ��ʾÿһ·LED(1����0��)
//����ֵ����
//******************************************************************************
//*/
//void LED_Switch(uint32_t ledBits)
//{

//  SN3218A_RegWirte(0X13, (ledBits & 0x0000003F));
//  SN3218A_RegWirte(0X14, (ledBits & 0x00000FC0) >> 6);
//  SN3218A_RegWirte(0X15, (ledBits & 0x0003F000) >> 12);
//  SN3218A_RegWirte(Addr_DataRefresh, 0x55); //ˢ�¼Ĵ�������
//}
///**
//******************************************************************************
//���ܣ�	��Χ��Ϩ��
//������	��

//����ֵ����
//******************************************************************************
//*/
//void LED_AroundOff(void)
//{
//  LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS, 0);
//  LED_Switch(0x300);
//}

///**
//******************************************************************************
//���ܣ�	��Χ�Ƶ���
//������	@color  ��������ɫ

//����ֵ����
//******************************************************************************
//*/
//void LED_AroundOn(LedColor_e color)
//{

//  LED_LightCtrl(LED_CENTRE_MAP_BITS, LED_CENTRE_LIGHT_VALUE);
//  if (color == BLUE)
//  {
//    LED_LightCtrl(LED_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //��������
//    LED_Switch(LED_BLUE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//  else if (color == RED_OR_ORANGE)
//  {
//    LED_LightCtrl(LED_ORANGE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //��������
//    LED_Switch(LED_ORANGE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//  else if (color == GREEN_OR_PURPLE)
//  {
//    LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //��������
//    LED_Switch(LED_ORANGE_BLUE_MAP_BITS | LED_CENTRE_MAP_BITS);
//  }
//}

///**
//******************************************************************************
//���ܣ�	��Χ�ư�������,����ָʾ������С
//������	@vol  ������С����ֵ��ȡֵ��Χ��0-14

//����ֵ����
//******************************************************************************
//*/
//const uint32_t ledVolIndexArray[8] = {0x3000, 0xF000, 0x3F000, 0x3F003, 0x3F00F, 0x3F03F, 0x3F0FF, 0x3FCFF};
//void LED_AroundVolIndex(uint8_t vol)
//{
//  if (vol > 15)
//    return;
//  LED_LightCtrl(LED_CENTRE_MAP_BITS, LED_CENTRE_LIGHT_VALUE);
//  LED_LightCtrl(ledVolIndexArray[vol / 2], LED_AROUND_LIGHT_VALUE); //��������
//  LED_Switch(ledVolIndexArray[vol / 2] | LED_CENTRE_MAP_BITS);
//}

////
///****************************************************************
//*���ܣ�������LED������ģʽ
//*������	@ledBits ��Ҫ���Ƶ�LED��Ӧ��·
//		@brightMax ����
//*	   	@speed �ٶ�
//*****************************************************************/
//void LED_Breath(uint32_t ledBits, uint8_t brightMax, uint8_t speed)
//{
//  static uint8_t light = 0; //PWMֵ
//  static uint8_t dir;       //�仯����
//  static uint8_t time_cnt = 0;
//  uint8_t light_cut = 0;

//  time_cnt++;

//  if (time_cnt > speed)
//  {
//    time_cnt = 0;
//    LED_Switch(ledBits | LED_CENTRE_MAP_BITS); //������LED��,|LED_CENTRE_MAP_BITSΪ���ų��м�ƵĿ���

//    if (light > 7)
//    {
//      light_cut = 7;
//    }
//    else
//    {
//      light_cut = light;
//    }
//    LED_LightCtrl(ledBits, light_cut); //��������

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
//*���ܣ���ˮ��
//*������	@bright ����
//*	   	@speed �ٶ�
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

//  LED_Switch(0x00000000 | LED_CENTRE_MAP_BITS);      //�ȹر����е�
//  LED_LightCtrl(LED_ORANGE_BLUE_MAP_BITS, bright);   //��������
//  LED_Switch(bits | LED_CENTRE_MAP_BITS | lastBits); //������LED��,|LED_CENTRE_MAP_BITSΪ���ų��м�ƵĿ���

//  osDelay(speed);
//  LED_Switch(bits | LED_CENTRE_MAP_BITS);
//  osDelay(speed * 6);
//  for (i = 0; i < 8; i++)
//  {
//    if (i != 4) //�ܿ��м��ǿŵ�
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
//  LED_LightCtrl(LED_KEY1_ORANGE_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //��������
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
//  LED_LightCtrl(LED_KEY3_ORANGE_BLUE_MAP_BITS, LED_AROUND_LIGHT_VALUE); //��������
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
