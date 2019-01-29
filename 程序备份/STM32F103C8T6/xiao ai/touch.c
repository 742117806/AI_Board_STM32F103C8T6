#include "touch.h"




void Touch1_Detect(DevicePara_TypDef *p_device)
{
  //uint8_t Touch1_this_state, resval = Key_NoVal;
  uint8_t Touch1_this_state;
  static uint8_t Touch1_LastState = 1, Touch1Dis_Flag, Touch1_FilterCnt;
  static uint16_t Touch1_Cnt;
  
  Touch1_this_state = Touch1_RDPIN();

  if(Touch1_this_state) 
  {
    if(Touch1_this_state != Touch1_LastState) 
    {
      if(++Touch1_FilterCnt > Touch_FilterTime) 
      {
        Touch1Dis_Flag = 0;
        p_device->Touch_State |= 0x01;
        //if((Touch1_Cnt > Touch_ShortTime) && (Touch1_Cnt < Touch_LongTime))   
        if(Touch1_Cnt < Touch_LongTime)
        {                          
          //resval = Key_ShortVal;          
          p_device->Touch_Val = Key1_ShortVal;
        } 
        Touch1_LastState = Touch1_this_state;   
      }
        
    }
    else
    {
      Touch1_FilterCnt = 0;
    }
  }
  else
  {
    if(Touch1_this_state != Touch1_LastState) 
    {
      if(++Touch1_FilterCnt > Touch_FilterTime) 
      {
        p_device->Touch_State |= 0x10;
        Touch1_FilterCnt = 0;
        Touch1_Cnt = 0;
        Touch1_LastState = Touch1_this_state;   
      } 
    }  
    else
    {
      Touch1_FilterCnt = 0;
      if(!Touch1Dis_Flag)
      {
        if(++Touch1_Cnt >= Touch_LongTime)
        {
          Touch1_Cnt = Touch_LongTime;
          //resval = Key_LongVal;    
          p_device->Touch_Val = Key1_LongVal;
          Touch1Dis_Flag = 1;
        } 
      } 
    }           
  }     
  //return resval;
}

#if(0)
void Touch2_Detect(DevicePara_TypDef *p_device)
{
  //uint8_t Touch2_this_state, resval = Key_NoVal;
  uint8_t Touch2_this_state;
  static uint8_t Touch2_LastState = 1, Touch2Dis_Flag, Touch2_FilterCnt;
  static uint16_t Touch2_Cnt;
  
  Touch2_this_state = Touch2_RDPIN();

  if(Touch2_this_state) 
  {
    if(Touch2_this_state != Touch2_LastState) 
    {
      if(++Touch2_FilterCnt > Touch_FilterTime) 
      {
        Touch2Dis_Flag = 0;
        p_device->Touch_State |= 0x02;
        //if((Touch2_Cnt > Touch_ShortTime) && (Touch2_Cnt < Touch_LongTime))   
        if(Touch2_Cnt < Touch_LongTime)
        {                          
          //resval = Key_ShortVal;         
          p_device->Touch_Val = Key2_ShortVal;
        } 
        Touch2_LastState = Touch2_this_state;   
      }
        
    }
    else
    {
      Touch2_FilterCnt = 0;
    }
  }
  else
  {
    if(Touch2_this_state != Touch2_LastState) 
    {
      if(++Touch2_FilterCnt > Touch_FilterTime) 
      {
        p_device->Touch_State |= 0x20;
        Touch2_FilterCnt = 0;
        Touch2_Cnt = 0;
        Touch2_LastState = Touch2_this_state;   
      } 
    }  
    else
    {
      Touch2_FilterCnt = 0;
      if(!Touch2Dis_Flag)
      {
        if(++Touch2_Cnt >= Touch_LongTime)
        {
          Touch2_Cnt = Touch_LongTime;
          //resval = Key_LongVal;     
          p_device->Touch_Val = Key2_LongVal;
          Touch2Dis_Flag = 1;
        } 
      } 
    }           
  }     
  //return resval;
}

#else
void Touch2_Detect(DevicePara_TypDef *p_device)
{
  uint8_t Touch2_this_state;
  static uint8_t Touch2_LastState = 1, Touch2_FilterCnt;
  
  Touch2_this_state = Touch2_RDPIN();

  if(Touch2_this_state) 
  {
    if(Touch2_this_state != Touch2_LastState) 
    {
      if(++Touch2_FilterCnt > Touch_FilterTime) 
      {
        Touch2_FilterCnt = 0;
        Touch2_LastState = Touch2_this_state;   
      }   
    }
    else
    {
      Touch2_FilterCnt = 0;
    }
  }
  else
  {
    if(Touch2_this_state != Touch2_LastState) 
    {
      if(++Touch2_FilterCnt > Touch_FilterTime) 
      {
        Touch2_FilterCnt = 0;
        Touch2_LastState = Touch2_this_state;   
        p_device->Touch_Val = Key2_ShortVal;
        //return 1;
      } 
    }  
    else
    {
      Touch2_FilterCnt = 0;
    }           
  }     
  //return 0;
}
#endif











void Touch3_Detect(DevicePara_TypDef *p_device)
{
  //uint8_t Touch3_this_state, resval = Key_NoVal;
  uint8_t Touch3_this_state;
  static uint8_t Touch3_LastState = 1, Touch3Dis_Flag, Touch3_FilterCnt;
  static uint16_t Touch3_Cnt;
  
  Touch3_this_state = Touch3_RDPIN();

  if(Touch3_this_state) 
  {
    if(Touch3_this_state != Touch3_LastState) 
    {
      if(++Touch3_FilterCnt > Touch_FilterTime) 
      {
        Touch3Dis_Flag = 0;
        p_device->Touch_State |= 0x04;
        //if((Touch3_Cnt > Touch_ShortTime) && (Touch3_Cnt < Touch_LongTime))   
        if(Touch3_Cnt < Touch_LongTime)
        {
          //resval = Key_ShortVal;         
          p_device->Touch_Val = Key3_ShortVal;
        } 
        Touch3_LastState = Touch3_this_state;   
      }
        
    }
    else
    {
      Touch3_FilterCnt = 0;
    }
  }
  else
  {
    if(Touch3_this_state != Touch3_LastState) 
    {
      if(++Touch3_FilterCnt > Touch_FilterTime) 
      {
        p_device->Touch_State |= 0x40;
        Touch3_FilterCnt = 0;
        Touch3_Cnt = 0;
        Touch3_LastState = Touch3_this_state;   
      } 
    }  
    else
    {
      Touch3_FilterCnt = 0;
      if(!Touch3Dis_Flag)
      {
        if(++Touch3_Cnt >= Touch_LongTime)
        {
          Touch3_Cnt = Touch_LongTime;
          //resval = Key_LongVal;     
          p_device->Touch_Val = Key3_LongVal;
          Touch3Dis_Flag = 1;
        } 
      } 
    }           
  }     
  //return resval;
}



#if(0)
void Touch4_Detect(DevicePara_TypDef *p_device)
{
  //uint8_t Touch4_this_state, resval = Key_NoVal;
  uint8_t Touch4_this_state;
  static uint8_t Touch4_LastState = 1, Touch4Dis_Flag, Touch4_FilterCnt;
  static uint16_t Touch4_Cnt;
  
  Touch4_this_state = Touch4_RDPIN();

  if(Touch4_this_state) 
  {
    if(Touch4_this_state != Touch4_LastState) 
    {
      if(++Touch4_FilterCnt > Touch_FilterTime) 
      {
        Touch4Dis_Flag = 0;
        p_device->Touch_State |= 0x08;
        //if((Touch4_Cnt > Touch_ShortTime) && (Touch4_Cnt < Touch_LongTime))  
        if(Touch4_Cnt < Touch_LongTime)
        {                          
          //resval = Key_ShortVal;          
          p_device->Touch_Val = Key4_ShortVal;
        } 
        Touch4_LastState = Touch4_this_state;   
      }
        
    }
    else
    {
      Touch4_FilterCnt = 0;
    }
  }
  else
  {
    if(Touch4_this_state != Touch4_LastState) 
    {
      if(++Touch4_FilterCnt > Touch_FilterTime) 
      {
        p_device->Touch_State |= 0x80;
        Touch4_FilterCnt = 0;
        Touch4_Cnt = 0;
        Touch4_LastState = Touch4_this_state;   
      } 
    }  
    else
    {
      Touch4_FilterCnt = 0;
      if(!Touch4Dis_Flag)
      {
        if(++Touch4_Cnt >= Touch_LongTime)
        {
          Touch4_Cnt = Touch_LongTime;
          //resval = Key_LongVal;     
          p_device->Touch_Val = Key4_LongVal;
          Touch4Dis_Flag = 1;
        } 
      } 
    }           
  }     
  //return resval;
}

#else
void Touch4_Detect(DevicePara_TypDef *p_device)
{
  uint8_t Touch4_this_state;
  static uint8_t Touch4_LastState = 1, Touch4_FilterCnt;
  
  Touch4_this_state = Touch4_RDPIN();

  if(Touch4_this_state) 
  {
    if(Touch4_this_state != Touch4_LastState) 
    {
      if(++Touch4_FilterCnt > Touch_FilterTime) 
      {
        Touch4_FilterCnt = 0;
        Touch4_LastState = Touch4_this_state;   
      }   
    }
    else
    {
      Touch4_FilterCnt = 0;
    }
  }
  else
  {
    if(Touch4_this_state != Touch4_LastState) 
    {
      if(++Touch4_FilterCnt > Touch_FilterTime) 
      {
        Touch4_FilterCnt = 0;
        Touch4_LastState = Touch4_this_state;   
        p_device->Touch_Val = Key4_ShortVal;
        //return 1;
      } 
    }  
    else
    {
      Touch4_FilterCnt = 0;
    }           
  }     
  //return 0;
}

#endif
