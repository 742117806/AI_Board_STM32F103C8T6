#include "stm32f1xx.h"
#include "led.h"
#include "frame_process.h"
#include "sn3218a_i2c.h"
#include "delay.h"


#define LedBright_Select                    1

LED_Control_TypDef LED_Buf;

uint8_t const LEDBright[32]=
{
	0,  1,  2,  4,  6,  10, 13, 18,
  22, 28, 33, 39, 46, 53, 61, 69,
  78, 86, 96, 106,116,126,138,149,
  161,173,186,199,212,226,240,255    
};



/*
uint8_t const LEDBrightUp_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   4,   4,   4,   5,   5,   
  5,   6,   6,   7,   7,   8,   8,   9,   
  9,   10,  10,  11,  11,  12,  12,  13,  
  14,  15,  16,  17,  18,  19,  20,  21,  
  22,  23,  24,  25,  26,  27,  28,  29,
  30,  32,  34,  36,  38,  40,  42,  44,  
  46,  48,  50,  52,  54,  57,  60,  63,  
};


uint8_t const LEDBrightDown_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   4,   4,   4,   5,   5,   
  5,   6,   6,   7,   7,   8,   8,   9,   
  9,   10,  10,  11,  11,  12,  12,  13,  
  14,  15,  16,  17,  18,  19,  20,  21,  
  22,  23,  24,  25,  26,  27,  28,  29,
  30,  32,  34,  36,  38,  40,  42,  44,  
  46,  48,  50,  52,  54,  57,  60,  63,    
};

*/
 
#if (LedBright_Select == 1)
uint8_t const LEDBrightUp_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   3,   4,   4,   4,   5,   
  5,   5,   6,   6,   6,   7,   7,   7,   
  8,   8,   8,   9,   9,   10,  10,  11,  
  11,  12,  12,  13,  13,  14,  14,  15,  
  15,  16,  16,  17,  17,  18,  18,  19,
  19,  20,  20,  21,  21,  22,  22,  23,  
  23,  24,  25,  26,  27,  28,  29,  31,  

};


uint8_t const LEDBrightDown_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   3,   4,   4,   4,   5,   
  5,   5,   6,   6,   6,   7,   7,   7,   
  8,   8,   8,   9,   9,   10,  10,  11,  
  11,  12,  12,  13,  13,  14,  14,  15,  
  15,  16,  16,  17,  17,  18,  18,  19,
  19,  20,  20,  21,  21,  22,  22,  23,  
  23,  24,  25,  26,  27,  28,  29,  31, 

   
};

#elif (LedBright_Select == 2)

uint8_t const LEDBrightUp_64Level[64]=
{
  1,   1,   1,   1,   1,   1,   2,   2,
  2,   2,   2,   3,   3,   3,   3,   3,   
  4,   4,   4,   4,   5,   5,   5,   5,   
  6,   6,   6,   7,   7,   7,   8,   8,  
  9,   9,   10,  10,  11,  12,  13,  14,  
  15,  16,  17,  18,  19,  20,  21,  22,
  23,  24,  25,  26,  27,  28,  29,  30,  
  31,  32,  33,  34,  35,  36,  37,  38,  
};



uint8_t const LEDBrightDown_64Level[64]=
{
  1,   1,   1,   1,   1,   1,   2,   2,
  2,   2,   2,   3,   3,   3,   3,   3,   
  4,   4,   4,   4,   5,   5,   5,   5,   
  6,   6,   6,   7,   7,   7,   8,   8,  
  9,   9,   10,  10,  11,  12,  13,  14,  
  15,  16,  17,  18,  19,  20,  21,  22,
  23,  24,  25,  26,  27,  28,  29,  30,  
  31,  32,  33,  34,  35,  36,  37,  38,   
};

#else
uint8_t const LEDBrightUp_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   4,   4,   4,   5,   5,   
  5,   6,   6,   7,   7,   8,   8,   9,   
  9,   10,  10,  11,  11,  12,  12,  13,  
  14,  15,  16,  17,  18,  19,  20,  21,  
  22,  23,  24,  25,  26,  27,  28,  29,
  30,  32,  34,  36,  38,  40,  42,  44,  
  46,  48,  50,  52,  54,  57,  60,  63,  
};


uint8_t const LEDBrightDown_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   4,   4,   4,   5,   5,   
  5,   6,   6,   7,   7,   8,   8,   9,   
  9,   10,  10,  11,  11,  12,  12,  13,  
  14,  15,  16,  17,  18,  19,  20,  21,  
  22,  23,  24,  25,  26,  27,  28,  29,
  30,  32,  34,  36,  38,  40,  42,  44,  
  46,  48,  50,  52,  54,  57,  60,  63,    
};

#endif




//中间灯
/*
uint8_t const MidBrightUp_64Level[64]=
{
  0,   1,   2,   3,   4,   5,   6,   7,
  8,   9,  10,  11,  12,  13,  14,  15,
  17,  19,  21,  23,  25,  27,  29,  31,
  33,  35,  37,  40,  43,  45,  48,  51,  
  54,  57,  60,  63,  66,  69,  72,  76,  
  80,  84,  88,  92,  96,  100, 104, 108, 
  113, 118, 123, 128, 133, 138, 144, 150, 
  157, 165, 175, 187, 201, 217, 235, 255   
};



uint8_t const MidBrightDown_64Level[64]=
{
  0,   1,   2,   3,   4,   5,   6,   7,
  8,   9,  10,  11,  12,  13,  14,  15,
  17,  19,  21,  23,  25,  27,  29,  31,
  33,  35,  37,  40,  43,  45,  48,  51,  
  54,  57,  60,  63,  66,  69,  72,  76,  
  80,  84,  88,  92,  96,  100, 104, 108, 
  113, 118, 123, 128, 133, 138, 144, 150, 
  157, 165, 175, 187, 201, 217, 235, 255  
};

*/

uint8_t const MidBrightUp_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   4,   4,   4,   5,   5,   
  5,   6,   6,   7,   7,   8,   8,   9,   
  9,   10,  10,  11,  11,  12,  12,  13,  
  14,  15,  16,  17,  18,  19,  20,  21,  
  22,  23,  24,  25,  26,  27,  28,  29,
  30,  31,  32,  33,  34,  45,  36,  37,
  38,  39,  40,  41,  42,  43,  44,  45,
  //46,  48,  50,  52,  54,  57,  60,  63,   
};



uint8_t const MidBrightDown_64Level[64]=
{
  1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   3,   4,   4,   4,   5,   5,   
  5,   6,   6,   7,   7,   8,   8,   9,   
  9,   10,  10,  11,  11,  12,  12,  13,  
  14,  15,  16,  17,  18,  19,  20,  21,  
  22,  23,  24,  25,  26,  27,  28,  29,
  30,  31,  32,  33,  34,  45,  36,  37,
  38,  39,  40,  41,  42,  43,  44,  45,
  
  //46,  48,  50,  52,  54,  57,  60,  63,  
};


/*
uint8_t const LEDBrightDown_64Level[64]=
{
  0,   1,   2,   3,   4,   5,   6,   7,
  8,   9,   10,  11,  12,  13,  14,  15,
  16,  17,  18,  19,  20,  21,  22,  23,  
  24,  25,  26,  27,  28,  29,  30,  31,
  32,  33,  34,  35,  36,  37,  38,  39,  
  40,  41,  42,  43,  44,  45,  46,  47, 
  48,  49,  50,  51,  52,  53,  54,  55,
  56,  57,  58,  59,  60,  61,  62,  63,  
};
*/

void Write_SN3218(uint8_t addr, uint8_t cmd)
{
  I2C_Start();
  I2C_Write1Byte(SN3218_ADDR);
  I2C_Write1Byte(addr);
  I2C_Write1Byte(cmd);
  I2C_Stop();
}

void SN3218_Refresh(void)
{
  Write_SN3218(Addr_DataRefresh, 0x55);
}

void SN3218_Off(void)
{
  Write_SN3218(Addr_All_Switch, 0x00);
}

//para 1: 周围LED清除   0： 所有LED清除
void SN3218_Led_Clear(uint8_t para)
{
  uint8_t i;
  
  I2C_Start();
  I2C_Write1Byte(SN3218_ADDR);
  I2C_Write1Byte(0x01);
  for(i = 0; i < 18; i++)
  {
    if(para)
    {
      if((i == 8) && (Device_ParaBuf.Led4_CotrolPara & 0x01))
        I2C_Write1Byte(Device_ParaBuf.MiddleLed_Brightness);
      else if((i == 9) && (Device_ParaBuf.Led4_CotrolPara & 0x02))
        I2C_Write1Byte(Device_ParaBuf.MiddleLed_Brightness);
      else
       I2C_Write1Byte(0x00);
    }
    else
      I2C_Write1Byte(0x00);
  }
    
  I2C_Stop();
  Write_SN3218(Addr_DataRefresh, 0x55);   
}


void SN3218_On(void)
{
  Write_SN3218(Addr_All_Switch, 0x01);
  Write_SN3218(Addr_PWM1_6_Switch, 0x3f);
  Write_SN3218(Addr_PWM7_12_Switch, 0x3f);
  Write_SN3218(Addr_PWM13_18_Switch, 0x3f);
}






void LED_Clear(uint8_t para)
{
  SN3218_Off();
  SN3218_Led_Clear(para);
  SN3218_On();
}
void SN3218_Init(void)
{

 // SN3218_PwInit();
  I2c_Init();
  SN3218_PowerOn();
  
  LED_Clear(0);
}


void Led4_Set(uint8_t led4_para)
{
  uint8_t color_val = led4_para & 0x03;
  
  /*
  if(color_val == 0x00)
  {
    I2C_Write1Byte(0x00);
    I2C_Write1Byte(0x00);
  }
  */
  if((color_val == 0x01) || (color_val == 0x00))
  {
    I2C_Write1Byte(Device_ParaBuf.MiddleLed_Brightness);
    I2C_Write1Byte(0x00);
  }
  else if(color_val == 0x02)
  {
    I2C_Write1Byte(0x00);
    I2C_Write1Byte(Device_ParaBuf.MiddleLed_Brightness); 
  }
  else if(color_val == 0x03)
  {
    I2C_Write1Byte(Device_ParaBuf.MiddleLed_Brightness);
    I2C_Write1Byte(Device_ParaBuf.MiddleLed_Brightness); 
  }
}


void LED_SetDuty(Led_Color_TypeDef color, uint8_t const brightness)
{
  uint8_t i;
  
  I2C_Start();
  I2C_Write1Byte(SN3218_ADDR);
  I2C_Write1Byte(0x01);
  
  switch(color)
  {
  case Colourless:
    for(i = 0; i < 9; i++)
    {
      if(i == 4)
      {
        Led4_Set(Device_ParaBuf.Led4_CotrolPara);
      }
      else
      {
        I2C_Write1Byte(0x00);
        I2C_Write1Byte(0x00);
      }
    }
    break;
    
  case Blue:
    for(i = 0; i < 9; i++)
    {
      if(i == 4)
      {
        Led4_Set(Device_ParaBuf.Led4_CotrolPara);
      }
      else
      {
        I2C_Write1Byte(brightness);
        I2C_Write1Byte(0x00);
      }
    }
    break;
  
  case Orange:
    for(i = 0; i < 9; i++)
    {
      if(i == 4)
      {
        Led4_Set(Device_ParaBuf.Led4_CotrolPara);
      }
      else
      {
        I2C_Write1Byte(0x00);
        I2C_Write1Byte(brightness);
      }
    }
    break;
      
  case Purple:
    for(i = 0; i < 9; i++)
    {
      if(i == 4)
      {
        Led4_Set(Device_ParaBuf.Led4_CotrolPara);
      }
      else
      {
        I2C_Write1Byte(brightness);
        I2C_Write1Byte(brightness);
      }
    }
    break;
      
  default:
    break;  
  }
  I2C_Stop();
  Write_SN3218(Addr_DataRefresh, 0x55);   
}



void LED_SetALed(uint8_t Number, Led_Color_TypeDef color, uint8_t brightness)
{
  uint8_t start_addr;
  
  if(Number > 8) Number = 8;
  start_addr = Number + Number + 1;

    
  I2C_Start();
  I2C_Write1Byte(SN3218_ADDR);
  I2C_Write1Byte(start_addr);
  switch(color)
  {
  case Colourless:
    I2C_Write1Byte(0x00);
    I2C_Write1Byte(0x00);
    break;
    
  case Blue:
    I2C_Write1Byte(brightness);
    I2C_Write1Byte(0x00);
    break;
    
  case Orange:
    I2C_Write1Byte(0x00);
    I2C_Write1Byte(brightness);
    break; 
    
  case Purple:
     I2C_Write1Byte(brightness);
     I2C_Write1Byte(brightness);
    break;
  
  default:
    break; 
  }
  I2C_Stop();
  SN3218_Refresh();  
}



/*
void Led_Breathe(Led_Color_TypeDef color, uint8_t breathe_gap)
{
  static uint8_t Breathe_Cnt;
  static uint8_t Cnt_Direction;
  static uint8_t Breathe_Gap_Cnt;
  
  if(++Breathe_Gap_Cnt >= breathe_gap)
  {
    Breathe_Gap_Cnt = 0;
    if(!Cnt_Direction)
    { 
      if(++Breathe_Cnt >= 63)
      {
        Breathe_Cnt = 63;
        Cnt_Direction = 1;
      }
      LED_SetDuty(color, LEDBrightUp_64Level[Breathe_Cnt]);
    }
    else
    {
      if(Breathe_Cnt > 0) 
        Breathe_Cnt -= 1;
      //if(Breathe_Cnt == 0)
      else
      {
        Cnt_Direction = 0;
      }
      LED_SetDuty(color, LEDBrightDown_64Level[Breathe_Cnt]);
    }
    
  }
  
}
*/


void Led_Breathe(Led_Color_TypeDef color, uint8_t breathe_gap)
{
  static uint8_t Breathe_Cnt;
  static uint8_t Breathe_State;
  static uint8_t Breathe_Gap_Cnt;
  
  if(++Breathe_Gap_Cnt >= breathe_gap)
  {
    Breathe_Gap_Cnt = 0;
    if(Breathe_State == 0)  
    { 
      LED_SetDuty(color, LEDBrightUp_64Level[Breathe_Cnt & 0x3f]);
      if(++Breathe_Cnt > 63)
      { 
        if(Breathe_AlBrigth != 0)
        {
          Breathe_State = 1;
          Breathe_Cnt = 0;
        }   
        else
        {
          Breathe_State = 2;
          Breathe_Cnt = 63;
        }   
      }
      
    }
    
    else if(Breathe_State == 1)  
    {
      LED_SetDuty(color, LEDBrightUp_64Level[63]);
      if(++Breathe_Cnt >= Breathe_AlBrigth)
      {
        Breathe_State = 2;
        Breathe_Cnt = 63; 
      }
    }
    else if(Breathe_State == 2)
    {
      LED_SetDuty(color, LEDBrightDown_64Level[Breathe_Cnt & 0x3f]);
      if(Breathe_Cnt > 0) 
        Breathe_Cnt --;
      else
      {
        //Breathe_Cnt = 0;
        if(Breathe_AlExting != 0)
        {
          Breathe_State = 3;
        }   
        else
        {
          Breathe_State = 0; 
        } 
      }
    }
    

    else  
    {
      LED_SetDuty(color, 1);
      if(++Breathe_Cnt >= Breathe_AlExting)
      {
        Breathe_State = 0;
        Breathe_Cnt = 0; 
      }
    }
    
  }
  
}








void LedMiddle_Gradually_Brighten(Led_Color_TypeDef color)
{
  uint8_t i;
  
  for(i = 0; i < 64; i++)
  {
    LED_SetALed(4, color, LEDBrightUp_64Level[i]);
    SysDelay_Xms(8*Breathe_Gap);
  }
}

void LedMiddle_Gradually_Dark(Led_Color_TypeDef color)
{
  uint8_t i;
  
  for(i = 64; i > 0; i--)
  {
    LED_SetALed(4, color, LEDBrightDown_64Level[i-1]);
    SysDelay_Xms(8*BreatheMiddle_Gap);
  }
}

void Led_Water(Led_Color_TypeDef color, uint8_t brightness, uint8_t water_gap)
{
  static uint8_t Water_Gap_Cnt;
  static uint8_t Water_Number;
  uint8_t last_number;
  
  if(++Water_Gap_Cnt >= water_gap)
  {
    Water_Gap_Cnt = 0;
    
    if(++Water_Number >= 9)Water_Number = 0;
    if(Water_Number == 4) Water_Number = 5;
    if(Water_Number)
    {
      last_number = Water_Number - 1;
      if(last_number == 4) last_number = 3;
    }   
    else 
      last_number = 8;
    
    LED_SetALed(last_number, color, 0);
    LED_SetALed(Water_Number, color, brightness);
  }
}


void Led_Mode0(void)
{
  LED_Clear(1);
}


void Led_Mode1(Led_Color_TypeDef color)
{
  Led_Breathe(color, Breathe_Gap);
}


void Led_Mode2(Led_Color_TypeDef color)
{
  LED_SetDuty(color, Alwayslight_Brightness);
} 


void Led_Mode3(Led_Color_TypeDef color)
{
  Led_Water(color, Water_Brightness, Water_Gap);
}


/*
void Led_Mode4(Led_Color_TypeDef color)
{
   LED_SetALed(4, color, MiddleLed_Brightness); 
}
*/


void Led_Mode4(DevicePara_TypDef *p_device)
{
  static uint8_t MiddleLed_BreatheCnt;
  static uint8_t MiddleLed_Gap_Cnt;
  
  if((p_device->MiddleLed_Contorl & 0x0f) == 0) return;
  
  if(++MiddleLed_Gap_Cnt >= BreatheMiddle_Gap)
  {
    Led_Color_TypeDef color;
    uint8_t color_val = p_device->Led4_CotrolPara & 0x03;
    if((color_val == 0x00) || (color_val == 0x01))
      color = Blue;
    else if(color_val == 0x02)
      color = Orange;
    else if(color_val == 0x03)
      color = Purple;
    MiddleLed_Gap_Cnt = 0;
  
    if(p_device->MiddleLed_Contorl == 0x01)  
    {
      p_device->MiddleLed_Brightness = MidBrightUp_64Level[MiddleLed_BreatheCnt & 0x3f];
      LED_SetALed(4, color, p_device->MiddleLed_Brightness);
      if(MiddleLed_BreatheCnt >= 63)
      {
        MiddleLed_BreatheCnt = 63;
        p_device->MiddleLed_Contorl = 0x10;  
      }
      else
        MiddleLed_BreatheCnt++;
    }
    else if(p_device->MiddleLed_Contorl == 0x02)  
    {
      p_device->MiddleLed_Brightness = MidBrightDown_64Level[MiddleLed_BreatheCnt & 0x3f];
      LED_SetALed(4, color, p_device->MiddleLed_Brightness);
      if(MiddleLed_BreatheCnt == 0)
      {
        p_device->MiddleLed_Contorl = 0x00;  
      } 
      else
        MiddleLed_BreatheCnt--;
    }
  }   
}


void Led_Mode5(DevicePara_TypDef *p_device, Led_Color_TypeDef colour)
{
  uint8_t volume = p_device->Led_Mode5Volume;
  
  
  if(p_device->Led_Mode5DelayCnt == 0)
  {
    if(volume < 2)
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
    }
    else if(volume < 4)
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
      LED_SetALed(7, colour, Alwayslight_Brightness);
    }
    else if(volume < 6)
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
      LED_SetALed(7, colour, Alwayslight_Brightness);
      LED_SetALed(8, colour, Alwayslight_Brightness);
    }
    else if(volume < 8)
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
      LED_SetALed(7, colour, Alwayslight_Brightness);
      LED_SetALed(8, colour, Alwayslight_Brightness);
      LED_SetALed(0, colour, Alwayslight_Brightness);
    }
    else if(volume < 10)
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
      LED_SetALed(7, colour, Alwayslight_Brightness);
      LED_SetALed(8, colour, Alwayslight_Brightness);
      LED_SetALed(0, colour, Alwayslight_Brightness);
      LED_SetALed(1, colour, Alwayslight_Brightness);
    }
    else if(volume < 12)
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
      LED_SetALed(7, colour, Alwayslight_Brightness);
      LED_SetALed(8, colour, Alwayslight_Brightness);
      LED_SetALed(0, colour, Alwayslight_Brightness);
      LED_SetALed(1, colour, Alwayslight_Brightness);
      LED_SetALed(2, colour, Alwayslight_Brightness);
    }
    else if(volume < 14)
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
      LED_SetALed(7, colour, Alwayslight_Brightness);
      LED_SetALed(8, colour, Alwayslight_Brightness);
      LED_SetALed(0, colour, Alwayslight_Brightness);
      LED_SetALed(1, colour, Alwayslight_Brightness);
      LED_SetALed(2, colour, Alwayslight_Brightness);
      LED_SetALed(3, colour, Alwayslight_Brightness);
    }
    else
    {
      LED_SetALed(6, colour, Alwayslight_Brightness);
      LED_SetALed(7, colour, Alwayslight_Brightness);
      LED_SetALed(8, colour, Alwayslight_Brightness);
      LED_SetALed(0, colour, Alwayslight_Brightness);
      LED_SetALed(1, colour, Alwayslight_Brightness);
      LED_SetALed(2, colour, Alwayslight_Brightness);
      LED_SetALed(3, colour, Alwayslight_Brightness);
      LED_SetALed(5, colour, Alwayslight_Brightness);
    }
  }
  if(++p_device->Led_Mode5DelayCnt > LedMode5_Time)
  {
    LED_Clear(1);
    p_device->Led_CotrolPara = p_device->Led_LastCotrolPara;
  }
}












