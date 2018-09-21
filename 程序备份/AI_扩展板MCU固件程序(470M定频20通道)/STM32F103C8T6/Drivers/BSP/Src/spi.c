
#include "spi.h"
#include "uart.h"
extern SPI_HandleTypeDef hspi1;

/* 
********************************************************************************************************* 
*  函 数 名: SPI_RWbyte 
*  功能说明: SPI1发送接收一个字节
*  形    参: @sdata要发送的字节 
*  返 回 值: 收到的字节
********************************************************************************************************* 
*/ 
uint8_t rdata=0;
uint8_t SPI_RWbyte(uint8_t sdata)
{   
	                
		                 
//	uint8_t retry=0;                
//    while((SPI1->SR&1<<1)==0)
//    {
//        retry++;
//        if(retry>200)return 0;
//    }              
//    SPI1->DR=sdata;          
//    retry=0;
//    while((SPI1->SR&1<<0)==0) 
//    {
//        retry++;
//        if(retry>200)return 0;
//    }                                  
//    return SPI1->DR;
	HAL_SPI_TransmitReceive(&hspi1,&sdata,&rdata,1,1000);
	return   rdata;
	
}
    



