#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25Q64/NRF24L01
//SPI口初始化
//这里针是对SPI1的初始化

u16 SI4438_TYPE;					//定义W25QXX芯片型号
//uint8_t SPI1_Rx_Buf[4096];
//uint8_t SPI1_Tx_Buf[4096];
/**
  * @brief  Configures the SPI1 Peripheral.
  * @param  None
  * @retval None
  */
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* Peripheral Clock Enable -------------------------------------------------*/
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO,  ENABLE );//复用时钟使能 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI2时钟使能 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);   //用PB3，PB4，PA15做普通IO，PA13&14用于SWD调试	
    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);   //管脚重映射功能SPI1


    /* SPI GPIO Configuration --------------------------------------------------*/
    /*
      * PB4 --> SPI1 MISO Mode: GPIO_Mode_IPU
      * PB3 --> SPI1 SCK  Mode: GPIO_Mode_AF_PP
      * PB5 --> SPI1 MOSI Mode: GPIO_Mode_AF_PP
      */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB

    SPI_Cmd(SPI2, DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第一个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 10;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE);


//	SPI_RWbyte(0xff);//启动传输		
}
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频
//SPI_BaudRatePrescaler_8   8分频
//SPI_BaudRatePrescaler_16  16分频
//SPI_BaudRatePrescaler_256 256分频

void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    SPI1->CR1&=0XFFC7;
    SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度
    SPI_Cmd(SPI1,ENABLE);

}

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节

u8 SPI_RWbyte(u8 TxData)
{
    u8 retry=0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        retry++;
        if(retry>200)return 0;
    }
    SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
    retry=0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
    {
        retry++;
        if(retry>200)return 0;
    }
    return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}




//初始化SPI FLASH的IO口
void SI4438_PingConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);//PORTA时钟使能
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);   //用PB3，PB4，PA15做普通IO，PA13&14用于SWD调试

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_15;  // PA11 PA15 推挽
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;  // SI4438_PA_RX_PIN,   SI4438_PA_TX_PIN
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SI4438_CS=1;							//SPI FLASH不选中
    SPI1_Init();
}


























