#include "rout.h"
#include "stm32f0_usart.h"
#include "stm32f0_eeprom.h"

//uint8_t Frame_WaitResponse_Flag;
uint8_t Dis_TryFlag;
uint8_t Try_Cnt;
uint16_t Frame_Wait_Cnt;
uint16_t Frame_WaitOver_Time;

HKRout_Tab_TypDef Rout_TempTab;
HKPath_Try_TypDef Rout_TryBuf;
uint8_t RoutRegion[3 + RoutSeries_Size];
uint8_t RoutFrame_buf[255];
HKRout_Tab_TypDef Rout_Eeprom_TempBuf[64];
//uint32_t LogicAddr_Flag[8];

/*
uint8_t Frame_RoutRegion_Add(uint8_t *p_source, uint8_t *pp) 
{
  uint8_t ComposeFrame_Len;
  uint16_t crc16_val;
  
  pp[2] = p_source[Region_AddrNumber];   
  pp[1] = pp[0];                         
  memmove(&p_source[Region_DataAFNNumber + p_source[Region_DataLenNumber]], pp, pp[0] + 3);
  ComposeFrame_Len = Region_DataAFNNumber + p_source[Region_DataLenNumber] + (RoutSeries_Size + 3);
  crc16_val = CRC16_2(p_source,ComposeFrame_Len);  
  p_source[ComposeFrame_Len] = crc16_val >> 8;
  ComposeFrame_Len++;
  p_source[ComposeFrame_Len] = crc16_val & 0xff;
  ComposeFrame_Len++;
  p_source[ComposeFrame_Len] = HKFreamEnd;  
  ComposeFrame_Len++;
  
  return ComposeFrame_Len;
}
*/

uint8_t Frame_RoutRegion_Add(uint8_t *p_source, uint8_t len, uint8_t *pp)
{
  uint8_t ComposeFrame_Len;
  uint16_t crc16_val;

  p_source[Region_SeqNumber] |= 0x80;

  p_source[Region_AddrNumber] = pp[2 + pp[0]];

  pp[1] = pp[0];
  //memmove(&p_source[Region_DataAFNNumber + p_source[Region_DataLenNumber]], pp, (RoutSeries_Size + 3));
  //ComposeFrame_Len = Region_DataAFNNumber + p_source[Region_DataLenNumber] + (RoutSeries_Size + 3);
  memmove(&p_source[len - 3], pp, (RoutSeries_Size + 3)); //由于有加密帧，所以要这样加上路由表
  ComposeFrame_Len = len + RoutSeries_Size;
  crc16_val = CRC16_2(p_source, ComposeFrame_Len);
  p_source[ComposeFrame_Len] = crc16_val >> 8;
  ComposeFrame_Len++;
  p_source[ComposeFrame_Len] = crc16_val & 0xff;
  ComposeFrame_Len++;
  p_source[ComposeFrame_Len] = HKFreamEnd;
  ComposeFrame_Len++;

  return ComposeFrame_Len;
}

uint8_t Frame_RoutRegion_Delete(uint8_t *p_source)
{
  uint8_t ComposeFrame_Len;
  uint16_t crc16_val;

  //p_source[Region_SeqNumber] &= 0x7F;
  ComposeFrame_Len = Region_DataAFNNumber + p_source[Region_DataLenNumber];
  crc16_val = CRC16_2(p_source, ComposeFrame_Len);
  p_source[ComposeFrame_Len] = crc16_val >> 8;
  ComposeFrame_Len++;
  p_source[ComposeFrame_Len] = crc16_val & 0xff;
  ComposeFrame_Len++;
  p_source[ComposeFrame_Len] = HKFreamEnd;
  ComposeFrame_Len++;

  return ComposeFrame_Len;
}

/*
void RoutPath_Read(uint8_t node)
{
  Eeprom_Read(Rout_StartAddr + node*sizeof(Rout_TempTab),(uint16_t *)&Rout_TempTab, sizeof(Rout_TempTab)/2);
  if(Rout_TempTab.Path_Number > RoutPath_Size) 
    Rout_TempTab.Path_Number = 0;
}
*/
void RoutPath_Read(uint8_t node)
{
  uint8_t RoutNumber_Shang;
  uint8_t RoutNumber_YuShu;

  RoutNumber_Shang = node >> 6;
  RoutNumber_YuShu = node & 0x3f;

  Eeprom_Read(Rout_StartAddr + 1024 * RoutNumber_Shang, (uint16_t *)&Rout_Eeprom_TempBuf, 512);
  memmove(&Rout_TempTab, &Rout_Eeprom_TempBuf[RoutNumber_YuShu], 16);
  if (Rout_TempTab.Path_Number > RoutPath_Size)
    Rout_TempTab.Path_Number = 0;
}

void RoutPath_Save(HKRout_Tab_TypDef *p_tab, uint8_t *pp, uint8_t node)
{
  uint8_t i, same_flag = 0;
  uint8_t series = pp[0];
  uint8_t RoutNumber_Shang;
  uint8_t RoutNumber_YuShu;

  RoutNumber_Shang = node >> 6;
  RoutNumber_YuShu = node & 0x3f;
  //if(p_tab->Path_Number > RoutPath_Size)
  //p_tab->Path_Number = 0;

  RoutPath_Read(node);

  for (i = 0; i < p_tab->Path_Number; i++)
  {
    if (series == p_tab->Path_Tab[i][0])
    {
      if (memcmp(&p_tab->Path_Tab[i][1], &pp[3], series) == 0)
      {
        same_flag = i + 1;
        break;
      }
    }
  }
  if (same_flag)
  {
    if (same_flag == 1)
      return;
    if (same_flag == 2)
    {
      memmove(p_tab->Path_Tab[1], p_tab->Path_Tab[0], RoutSeries_Size + 1);
    }
    else if (same_flag == 3)
    {
      memmove(p_tab->Path_Tab[2], p_tab->Path_Tab[1], RoutSeries_Size + 1);
      memmove(p_tab->Path_Tab[1], p_tab->Path_Tab[0], RoutSeries_Size + 1);
    }
    p_tab->Path_Tab[0][0] = series;
    memmove(&p_tab->Path_Tab[0][1], &pp[3], series);
  }

  else
  {
    for (i = RoutPath_Size - 1; i > 0; i--)
    {
      memmove(p_tab->Path_Tab[i], p_tab->Path_Tab[i - 1], RoutSeries_Size + 1);
    }
    p_tab->Path_Tab[0][0] = series;
    memmove(&p_tab->Path_Tab[0][1], &pp[3], series);
    if (++p_tab->Path_Number > RoutPath_Size)
      p_tab->Path_Number = RoutPath_Size;
  }
  memmove(&Rout_Eeprom_TempBuf[RoutNumber_YuShu], &Rout_TempTab, 16);
  //__disable_interrupt();
  Eeprom_ErasePage(Rout_StartAddr + 1024 * RoutNumber_Shang);
  Eeprom_Write(Rout_StartAddr + 1024 * RoutNumber_Shang, (uint16_t *)&Rout_Eeprom_TempBuf, 512);
  //__enable_interrupt();
}

void GetRout_FromRoutTab(HKRout_Tab_TypDef *pr, uint8_t *pp, uint8_t n)
{
  pp[0] = pr->Path_Tab[n][0];
  memmove(&pp[3], &pr->Path_Tab[n][1], pp[0]);
}

void GetRout_FromLogicTab(HKPath_Try_TypDef *p_ps, uint8_t *pp)
{
  uint8_t node_number = p_ps->Node_Number;

  if (p_ps->Current_Try_Series == 0)
  {
    p_ps->Current_Try_Series = 1;
  }
  while (1)
  {
    if (p_ps->Current_Try_Series == 1)
    {
      if ((p_ps->Current_Try_Number1 < node_number) && (p_ps->LogicAddr_Tab[p_ps->Current_Try_Number1] != p_ps->Distance_Addr))
        break;
    }
    if (p_ps->Current_Try_Series == 2)
    {
      if ((p_ps->Current_Try_Number1 < node_number) && (p_ps->Current_Try_Number1 != p_ps->Current_Try_Number2) && (p_ps->LogicAddr_Tab[p_ps->Current_Try_Number1] != p_ps->Distance_Addr) && (p_ps->LogicAddr_Tab[p_ps->Current_Try_Number2] != p_ps->Distance_Addr))
        break;
    }
    p_ps->Current_Try_Number1++;
    if (p_ps->Current_Try_Number1 >= node_number)
    {
      p_ps->Current_Try_Number1 = 0;
      if (p_ps->Current_Try_Series == 1)
      {
        p_ps->Current_Try_Number2 = 0;
        p_ps->Current_Try_Series = 2;
      }
      else if (p_ps->Current_Try_Series == 2)
      {
        if (++p_ps->Current_Try_Number2 >= node_number)
        {
          p_ps->Current_Try_Series = 1;
          //p_ps->Current_Try_Number2 = 0;
        }
      }
    }
  }
  if (p_ps->Current_Try_Series == 1)
  {
    pp[0] = 1;
    pp[3] = p_ps->LogicAddr_Tab[p_ps->Current_Try_Number1];
  }
  else if (p_ps->Current_Try_Series == 2)
  {
    pp[0] = 2;
    pp[3] = p_ps->LogicAddr_Tab[p_ps->Current_Try_Number1];
    pp[4] = p_ps->LogicAddr_Tab[p_ps->Current_Try_Number2];
  }
  p_ps->Current_Try_Number1++;
}

extern uint8_t const Set_LogicAddr_Id[3];

void Path_Try_Process(uint8_t *p_source, uint8_t *pp, HKPath_Try_TypDef *p_ps, HKRout_Tab_TypDef *pr)
{
  uint8_t get_rout_succeed_flag = 0;
  uint8_t send_len;

  if (Frame_Wait_Cnt > Frame_WaitOver_Time)
  {
    Try_Cnt++;
    if ((Try_Cnt > Try_Cnt_Max) || Dis_TryFlag)
    {
      Try_Cnt = 0;
      Frame_Wait_Cnt = 0;
    }
    else
    {
      if (pr->Current_Path_Number < pr->Path_Number)
      {
        GetRout_FromRoutTab(pr, pp, pr->Current_Path_Number);
        pr->Current_Path_Number++;
        get_rout_succeed_flag = 1;
      }
      else if (p_ps->Node_Number > 1)
      {
        GetRout_FromLogicTab(p_ps, pp);
        get_rout_succeed_flag = 1;
      }
      if (get_rout_succeed_flag)
      {
        send_len = Frame_RoutRegion_Add(p_source, p_ps->Frame_Len, pp);
        //Frame_WaitOver_Time = 75* (pp[0]+1);
        if (memcmp(&p_source[Region_DataIDNumber], Set_LogicAddr_Id, 3) == 0)
          Frame_WaitOver_Time = 75 * (pp[0] + 1) + Network_Delay; //8秒
        else
          Frame_WaitOver_Time = 75 * (pp[0] + 1);
        Frame_Wait_Cnt = 1;
        WIRELESS_ERROR_CODE = Si4438_Transmit_Start(&Wireless_Buf, Wireless_Channel[0], p_source, send_len);
      }
      else
      {
        Frame_Wait_Cnt = 0;
      }
    }
  }
}

/*
void Receive_LogicAddrTab_Process(uint8_t *p_logic, HKPath_Try_TypDef *p_ps)
{
  p_ps->Node_Number = p_logic[0];
  memmove(p_ps->LogicAddr_Tab, &p_logic[1], p_ps->Node_Number);
}
*/

void Receive_LogicAddrTab_Process(uint8_t *p_source, HKPath_Try_TypDef *p_ps)
{
  p_ps->Node_Number = p_source[Region_DataLenNumber] - 4;
  memmove(p_ps->LogicAddr_Tab, &p_source[Region_DataValNumber], p_ps->Node_Number);
}
