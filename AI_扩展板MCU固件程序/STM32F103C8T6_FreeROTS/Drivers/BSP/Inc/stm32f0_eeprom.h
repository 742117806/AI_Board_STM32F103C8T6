#ifndef STM32F0_EEPROM_H
#define STM32F0_EEPROM_H

#include "stm32f1xx.h"
#include "stmflash.h"



//#define STM32_FLASH_SIZE 	64 	 
#define Page_Size                      1024
             		
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 			//STM32 FLASH����ʼ��ַ

#define DEVICE_INFO_EEPROM_ADDR_BASE      (STM32_FLASH_END-(2*1024))		//���2K
#define Eeprom_StartAddr   DEVICE_INFO_EEPROM_ADDR_BASE  

#define KEY_StartAddr      	Eeprom_StartAddr		//�����Կ��ַ
#define KEY_StartLen		16
#define AD_ADDR				(KEY_StartAddr+KEY_StartLen)
#define AD_LEN		        2
#define GD_ADDR	            (AD_ADDR+AD_LEN)
#define GD_LEN	           46
#define MAC_EAddr          (GD_ADDR+GD_LEN)
//#define LANG_StartAddr     50
#define Rout_StartAddr     STM32_FLASH_END-1024           //��1K����ʼ����·�ɱ�Ĵ洢




//FLASH������ֵ
//#define FLASH_KEY1               0X45670123
//#define FLASH_KEY2               0XCDEF89AB


//#define PROGRAMMING_EN()           (FLASH->CR |= 1<<0)   //���ʹ��
//#define PROGRAMMING_DIS()          (FLASH->CR &= ~(1<<0))

//#define PAGE_ERASE_EN()            (FLASH->CR |= 1<<1)    //ҳ����
//#define PAGE_ERASE_DIS()           (FLASH->CR &= ~(1<<1))

//#define PAGE_ERASE_START()         (FLASH->CR |= 1<<6)


//#define Flash_Unlock()             {FLASH->KEYR = FLASH_KEY1; FLASH->KEYR = FLASH_KEY2;}
//#define Flash_Lock()               (FLASH->CR |= 1<<7)

//#define Flash_Busy()               (FLASH->SR & 1<<0)





extern uint16_t Ktemp[2];

/************************* �ⲿ���� ***************************/

void Eeprom_ErasePage(uint32_t page_addr);
void Eeprom_Write(uint32_t start_addr,uint16_t *p_buf,uint16_t len);
void Eeprom_Read(uint32_t start_addr,uint16_t *p_buf,uint16_t len);
void Eeprom_Init(void);









/*
void STMFLASH_Unlock(void);					  	//FLASH����
void STMFLASH_Lock(void);					  	//FLASH����
u8 STMFLASH_GetStatus(void);				  	//���״̬
u8 STMFLASH_WaitDone(u16 time);				  	//�ȴ���������
u8 STMFLASH_ErasePage(u32 paddr);			  	//����ҳ
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat);	//д�����
u16 STMFLASH_ReadHalfWord(u32 faddr);		  	//��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);
*/

















#endif


