#ifndef __SN3218_DRV_H
#define __SN3218_DRV_H

#include "includes.h"


//SN3218�ܽ�������ö���
#define LED_SN3218A_SDB_Pin 		GPIO_Pin_7   //���ػ��ܽ�
#define LED_SN3218A_SDB_GPIO_Port 	GPIOB
#define LED_SN3218A_SDA_Pin 		GPIO_Pin_8   //���ݹܽ�
#define LED_SN3218A_SDA_GPIO_Port 	GPIOB
#define LED_SN3218A_SCL_Pin 		GPIO_Pin_9   //ʱ�ӹܽ�
#define LED_SN3218A_SCL_GPIO_Port 	GPIOB


#define SN3218A_SDB_H() 	GPIO_SetBits(LED_SN3218A_SDB_GPIO_Port, LED_SN3218A_SDB_Pin)
#define SN3218A_SDB_L() 	GPIO_ResetBits(LED_SN3218A_SDB_GPIO_Port, LED_SN3218A_SDB_Pin)

#define I2C_SDA_H() 		GPIO_SetBits(LED_SN3218A_SDA_GPIO_Port, LED_SN3218A_SDA_Pin)
#define I2C_SDA_L() 		GPIO_ResetBits(LED_SN3218A_SDA_GPIO_Port, LED_SN3218A_SDA_Pin)

#define I2C_SDA_RD() 		GPIO_ReadInputDataBit(LED_SN3218A_SDA_GPIO_Port, LED_SN3218A_SDA_Pin)

#define I2C_SCL_H() 		GPIO_SetBits(LED_SN3218A_SCL_GPIO_Port, LED_SN3218A_SCL_Pin)
#define I2C_SCL_L() 		GPIO_ResetBits(LED_SN3218A_SCL_GPIO_Port, LED_SN3218A_SCL_Pin)

#define SN3218A_POWER_ON() 	SN3218A_SDB_H()
#define SN3218A_POWER_OFF() SN3218A_SDB_L()

#define SN3218_PowerOn() 	SN3218A_SDB_H()
#define SN3218_PowerOff() 	SN3218A_SDB_L()


//ģ��I2C��������
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write1Byte(uint8_t buf);
void I2c_Init(void);


//#include "peri_includes.h"
//#include "frame_process.h"
/****************************** SN3218 *******************************/

#define SN3218_ADDR 0xa8	//������ַ

//�Ĵ�����ַ��Ӧ����

#define	SN3218_OFF_REG		0x00	//�ضϼĴ������ر�����ͨ��
#define	SN3218_PWM1_REG		0x01	//LED1 PWM�Ĵ�����ַ
#define	SN3218_PWM2_REG 	0x02
#define	SN3218_PWM3_REG		0x03
#define	SN3218_PWM4_REG		0x04
#define	SN3218_PWM5_REG		0x05
#define	SN3218_PWM6_REG		0x06
#define	SN3218_PWM7_REG		0x07
#define	SN3218_PWM8_REG		0x08
#define	SN3218_PWM9_REG		0x09
#define	SN3218_PWM10_REG	0x0A
#define	SN3218_PWM11_REG	0x0B
#define	SN3218_PWM12_REG	0x0C
#define	SN3218_PWM13_REG	0x0D
#define	SN3218_PWM14_REG	0x0E
#define	SN3218_PWM15_REG	0x0F
#define	SN3218_PWM16_REG	0x10
#define	SN3218_PWM17_REG	0x11 //LED17 PWM�Ĵ�����ַ
#define SN3218_PWM18_REG	0x12 //LED18 PWM�Ĵ�����ַ
#define	SN3218_LED_CTRL1_REG	0x13
#define	SN3218_LED_CTRL2_REG	0x14
#define	SN3218_LED_CTRL3_REG	0x15
#define	SN3218_REF_REG			0x16         //ˢ�¼Ĵ���
#define	SN3218_REST_REG 		0x17		//�������мĴ���ΪĬ��ֵ

#define SN3218_LED1_ON_DATA	(1<<0)
#define SN3218_LED2_ON_DATA	(1<<1)
#define SN3218_LED3_ON_DATA	(1<<2)
#define SN3218_LED4_ON_DATA	(1<<3)
#define SN3218_LED5_ON_DATA	(1<<4)
#define SN3218_LED6_ON_DATA	(1<<5)

#define SN3218_LED7_ON_DATA	(1<<0)
#define SN3218_LED8_ON_DATA	(1<<1)
#define SN3218_LED9_ON_DATA	(1<<2)
#define SN3218_LED10_ON_DATA	(1<<3)
#define SN3218_LED11_ON_DATA	(1<<4)
#define SN3218_LED12_ON_DATA	(1<<5)

#define SN3218_LED13_ON_DATA	(1<<0)
#define SN3218_LED14_ON_DATA	(1<<1)
#define SN3218_LED15_ON_DATA	(1<<2)
#define SN3218_LED16_ON_DATA	(1<<3)
#define SN3218_LED17_ON_DATA	(1<<4)
#define SN3218_LED18_ON_DATA	(1<<5)



#define Addr_All_Switch 0x00
#define Addr_LED1_Blue 0X01
#define Addr_LED1_Green 0X02
#define Addr_LED1_Red 0X03

#define Addr_PWM1_6_Switch 0x13
#define Addr_PWM7_12_Switch 0x14
#define Addr_PWM13_18_Switch 0x15

#define Addr_DataRefresh 0x16
#define Addr_Reset 0x17

#define SteadyMode 0x10
#define BreatheMode 0x01
#define RunMode 0x80

#define OFFtoON_ONtoOFF 0x00
#define ONtoOFF_OFFtoON 0x01
#define OFFtoON_OFFtoON 0x02
#define ONtoOFF_ONtoOFF 0x03

#define Off_Status 0x00
#define OfftoOn_Status 0x10
#define On_Status 0x20
#define OntoOff_Status 0x30

#define Serier_End 0x00
#define Serier_Start1 0x01
#define Serier_Start2 0x02
#define Serier_Start3 0x03

#define RunLed_Size 12
#define CW_CW 0x00
#define CCW_CCW 0x01
#define CW_CCW 0X02
#define CCW_CW 0X03

#define First_Direction 0x00
#define Second_Direction 0x10

typedef enum
{
    Series1_1 = 0x11,
    Series1_2 = 0x12,
    Series1_3 = 0x14,

    Series2_12Same = 0x13,
    Series2_13Same = 0x15,
    Series2_23Same = 0x16,
    Series3_123Same = 0x17,

    Series2_12 = 0x20,
    Series2_21 = 0x21,

    Series2_13 = 0x22,
    Series2_31 = 0x23,

    Series2_23 = 0x24,
    Series2_32 = 0x25,

    Series3_123 = 0x30,
    Series3_132 = 0x31,
    Series3_213 = 0x32,
    Series3_231 = 0x33,
    Series3_312 = 0x34,
    Series3_321 = 0x35,
} Led_Series_TypeDef;

#define LED_AROUND_LIGHT_VALUE 2  //LED�Ƶ�����ֵ
#define LED_CENTRE_LIGHT_VALUE 30 //LED�Ƶ�����ֵ

#define LED_KEY1_ORANGE_MAP_BITS 0x0A      //����1ָʾ�Ƴ�ɫ
#define LED_KEY1_BLUE_MAP_BITS 0x05        //����1ָʾ����ɫ
#define LED_KEY1_ORANGE_BLUE_MAP_BITS 0x0F //����1ָʾ�Ƴ�ɫ+��ɫ

#define LED_KEY2_ORANGE_MAP_BITS 0xA0
#define LED_KEY2_BLUE_MAP_BITS 0x50
#define LED_KEY2_ORANGE_BLUE_MAP_BITS 0xF0

#define LED_KEY3_ORANGE_MAP_BITS 0x2800
#define LED_KEY3_BLUE_MAP_BITS 0x1400
#define LED_KEY3_ORANGE_BLUE_MAP_BITS 0x3C00

#define LED_KEY4_ORANGE_MAP_BITS 0x28000
#define LED_KEY4_BLUE_MAP_BITS 0x14000
#define LED_KEY4_ORANGE_BLUE_MAP_BITS 0x3C000

#define LED_ORANGE_BLUE_MAP_BITS 0x3FCFF //ȫ����Χ��

#define LED_ORANGE_MAP_BITS 0x2A8AA //��Χ��ɫ��
#define LED_BLUE_MAP_BITS 0x15455   //��Χ��ɫ��
#define LED_ALL_MAP_BITS 0X3FFFF    //���е�

#define LED_CENTRE_MAP_BITS 0x300

typedef enum
{
    Colourless = 0,
    Blue,
    Orange,
    Purple,
} Led_Color_TypeDef;

typedef struct
{
    uint16_t Delay_Time;
    uint16_t Delay_Cnt;
    uint8_t SecondBrightVal;
    uint8_t Cycle_Flag;
} LED_Steady_TypDef;

typedef struct
{
    uint8_t Breathe_Mode; //���ֽ���ģʽ�����ֽ���״̬
    uint16_t On_Time;
    uint16_t Off_Time;
    uint8_t Inc_Time;
    uint8_t Dec_Time;
    uint16_t Delay_Cnt;
} LED_Breathe_TypDef;

typedef struct
{
    uint8_t Run_Mode;
    uint8_t Run_Step;
    uint8_t Last_Number;
    uint8_t Run_Number;
    uint8_t Direction;
    uint8_t Color_Change_Cnt;
    uint16_t Run_Gap;
    uint16_t Delay_Cnt;
} Led_Run_TypDef;

typedef struct
{
    uint8_t mode; //ģʽ,
    uint8_t Current_BrightVal[3];
    uint8_t Change_Flag[3];
    LED_Steady_TypDef Led_Steady_Para[3];
    LED_Breathe_TypDef Led_Breathe_Para[3];
    Led_Series_TypeDef Series_Mode;
    uint16_t Series_Gap; //��϶
    uint16_t Series_Gap_cnt;
    uint8_t Breathe_StartStep;
    Led_Color_TypeDef SecondLedColor;
    Led_Run_TypDef SecondLed_Run_Para;
} LED_Control_TypDef;

typedef struct
{
    uint8_t Touch_Val;
    uint8_t Touch_State; //ÿ������״̬���ĸ���8�֣�ÿλ��ʾһ��
    //////////////////////
    uint8_t Led_Mode5Volume;   //ģʽ5������ֵ
    uint8_t Led_Mode5DelayCnt; //ģʽ5��ʱʱ�����
    uint8_t Led_LastCotrolPara;
    //////////////////////////
    uint8_t Led_CotrolPara;
    uint8_t Led4_CotrolPara;
    uint8_t MiddleLed_Brightness;
    uint8_t MiddleLed_Contorl;
    uint8_t Led_Cotrol_DisFlag;
    uint8_t Pending_Flag;   //bit 0: UCOM����֡  bit 1: DCOM����֡   bit2: WireLess����֡
    uint32_t UpReport_Flag; //��ͬ��λ����ͬ���ϱ�����
    //uint8_t FashionData_Buf[Frame_LenMAX];   //��ͨѶ����
} DevicePara_TypDef;

typedef enum KEY_LED_
{
    KEY1_ON_ORANGE = 0x10,
    KEY1_ON_BLUE,
    KEY2_ON_ORANGE,
    KEY2_ON_BLUE,
    KEY3_ON_ORANGE,
    KEY3_ON_BLUE,
    KEY4_ON_ORANGE,
    KEY4_ON_BLUE,
    KEY_RES,
} KEY_LED_e;

typedef enum LED_INDEX_MODE_
{
    LED_INDEX_MODE_AROUND_KEY1_ON = 0x0100,
    LED_INDEX_MODE_AROUND_KEY1_OFF = 0x0200,
    LED_INDEX_MODE_AROUND_KEY2_ON = 0x0300,
    LED_INDEX_MODE_AROUND_KEY2_OFF = 0x0400,
    LED_INDEX_MODE_AROUND_KEY3_ON = 0x0500,
    LED_INDEX_MODE_AROUND_KEY3_OFF = 0x0600,
    LED_INDEX_MODE_AROUND_KEY4_ON = 0x0700,
    LED_INDEX_MODE_AROUND_KEY4_OFF = 0x0800,
} LED_INDEX_MODE_e;

/************************* �ⲿ���� ***************************/
extern LED_Control_TypDef LED_Buf;

/************************* �ⲿ���� ***************************/
void SN3218_Init(void);
void LED_SetDuty(Led_Color_TypeDef color, uint8_t const brightness);
void LED_SetALed(uint8_t Number, Led_Color_TypeDef color, uint8_t const brightness);
void LED_Clear(uint8_t para);
void Led_Breathe(Led_Color_TypeDef color, uint8_t breathe_gap);
void Led_Water(Led_Color_TypeDef color, uint8_t water_gap, uint8_t brightness);

void Led_Mode1(Led_Color_TypeDef color);
void Led_Mode2(Led_Color_TypeDef color);
void Led_Mode3(Led_Color_TypeDef color);
//void Led_Mode4(Led_Color_TypeDef color);
void Led_Mode4(DevicePara_TypDef *p_fashionpara);
void Led_Mode5(DevicePara_TypDef *p_device, Led_Color_TypeDef colour);

void LedMiddle_Gradually_Brighten(Led_Color_TypeDef color);
void LedMiddle_Gradually_Dark(Led_Color_TypeDef color);

void SN3218A_Write(uint8_t regAddr, uint8_t value);
void LED_LightCtrl(uint32_t ledBits, uint8_t ledVal);
//��Χ��Ϩ��
//void LED_AroundOff(void);
void SN3218_RegRef(void);
void SN3218_LedStaSet(uint8_t led,uint8_t sta);
void SN3218_LedPwmSet(uint8_t led,uint8_t pwm);
void SN3218A_WriteCont(uint8_t reg_startAddr,uint8_t reg_endAddr, uint8_t value);

//��Χ�Ƶ���
//void LED_AroundOn(LedColor_e color);
//void LED_AroundVolIndex(uint8_t vol);
//void LED_Switch(uint32_t ledBits);
//void LED_CentreOn(void);
//void LED_Key1Show(LedColor_e color);
//void LED_Key3Show(LedColor_e color);
//void LED_Breath(uint32_t ledBits, uint8_t brightMax, uint8_t speed);
//void LED_Flow(LedColor_e color, uint8_t bright, uint8_t speed);

#endif
