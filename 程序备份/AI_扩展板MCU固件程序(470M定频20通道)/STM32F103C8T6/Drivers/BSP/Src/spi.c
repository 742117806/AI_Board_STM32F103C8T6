
#include "spi.h"
#include "uart.h"
extern SPI_HandleTypeDef hspi1;

/* 
********************************************************************************************************* 
*  �� �� ��: SPI_RWbyte 
*  ����˵��: SPI1���ͽ���һ���ֽ�
*  ��    ��: @sdataҪ���͵��ֽ� 
*  �� �� ֵ: �յ����ֽ�
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
    



