#ifndef __SPI_H
#define __SPI_H
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
							  
//////////////////////////////////////////////////////////////////////////////////
#define	SI4438_CS 		PAout(15)  		//W25QXX的片选信号
#define	SI4438_SDN 		PAout(11)  		//W25QXX的片选信号 

#define WIRELESS_NSEL_H()  GPIO_SetBits(GPIOA,GPIO_Pin_15)	
#define WIRELESS_NSEL_L()  GPIO_ResetBits(GPIOA,GPIO_Pin_15)

#define SDN_H()  GPIO_SetBits(GPIOA,GPIO_Pin_11)	
#define SDN_L()  GPIO_ResetBits(GPIOA,GPIO_Pin_11)

#define SI4438_PA_RX_H()	   GPIO_SetBits(GPIOB,GPIO_Pin_12)	
#define SI4438_PA_RX_L()       GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define SI4438_PA_TX_H()	   GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define SI4438_PA_TX_L()       GPIO_ResetBits(GPIOB,GPIO_Pin_13)
 				  	    													  
void SPI1_Init(void);			 //初始化SPI口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI_RWbyte(u8 TxData);//SPI总线读写一个字节	
void SI4438_PingConfig(void); 
#endif

