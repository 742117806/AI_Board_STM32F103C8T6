#include "stm32f1xx.h"
#include "stm32f0_eeprom.h"
#include "stmflash.h"

void Eeprom_ErasePage(uint32_t page_addr)
{
}

void Eeprom_WriteHalfWord(uint32_t addr, uint16_t dat)
{
}

uint16_t Eeprom_ReadHalfWord(uint32_t addr)
{
  return 0;
}

//start_addrĞëÊÇ2µÄ±¶Êı
void Eeprom_Write(uint32_t start_addr, uint16_t *p_buf, uint16_t len)
{
  STMFLASH_Write(start_addr, p_buf, len / 2 + 1);
}

void Eeprom_Read(uint32_t start_addr, uint16_t *p_buf, uint16_t len)
{
  STMFLASH_Read(start_addr, p_buf, len / 2 + 1);
}

uint16_t Ktemp[2];
void Eeprom_Init(void)
{
}
