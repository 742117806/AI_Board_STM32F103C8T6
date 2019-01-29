#include "sys.h"
#include "usart.h"
#include "includes.h"				
//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
//#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////

/* 定义每个串口结构体变量 */
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART4_FIFO_EN == 1
	static UART_T g_tUart4;
	static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART5_FIFO_EN == 1
	static UART_T g_tUart5;
	static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART6_FIFO_EN == 1
	static UART_T g_tUart6;
	static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
    x = x;
}
_ttywrch(int ch)
{
    ch=ch;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
    USART1->DR = (u8) ch;
    return ch;
}



#endif

/*使用microLib的方法*/
/*
int fputc(int ch, FILE *f)
{
USART_SendData(USART1, (uint8_t) ch);

while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

   return ch;
}
int GetKey (void)  {

   while (!(USART1->SR & USART_FLAG_RXNE));

   return ((int)(USART1->DR & 0x1FF));
}
*/


/*
*********************************************************************************************************
*	函 数 名: UartVarInit
*	功能说明: 初始化串口相关的变量
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if UART1_FIFO_EN == 1
	g_tUart1.uart = USART1;						/* STM32 串口设备 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* 发送缓冲区指针 */
	g_tUart1.pRxBuf = g_RxBuf1;					/* 接收缓冲区指针 */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart1.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart1.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart1.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart1.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart1.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart1.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart1.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart1.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = USART2;						/* STM32 串口设备 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* 发送缓冲区指针 */
	g_tUart2.pRxBuf = g_RxBuf2;					/* 接收缓冲区指针 */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart2.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart2.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart2.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart2.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart2.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart2.usTxCount = 0;						/* 待发送的数据个数 */	
	g_tUart2.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart2.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart2.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = USART3;						/* STM32 串口设备 */
	g_tUart3.pTxBuf = g_TxBuf3;					/* 发送缓冲区指针 */
	g_tUart3.pRxBuf = g_RxBuf3;					/* 接收缓冲区指针 */
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart3.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart3.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart3.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart3.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart3.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart3.usTxCount = 0;						/* 待发送的数据个数 */	
	g_tUart3.SendBefor = USART3_SendBefor;		/* 发送数据前的回调函数 */
	g_tUart3.SendOver = USART3_SendOver;		/* 发送完毕后的回调函数 */
	g_tUart3.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
#endif

#if UART4_FIFO_EN == 1
	g_tUart4.uart = UART4;						/* STM32 串口设备 */
	g_tUart4.pTxBuf = g_TxBuf4;					/* 发送缓冲区指针 */
	g_tUart4.pRxBuf = g_RxBuf4;					/* 接收缓冲区指针 */
	g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart4.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart4.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart4.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart4.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart4.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart4.usTxCount = 0;						/* 待发送的数据个数 */	
	g_tUart4.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart4.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart4.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
#endif

#if UART5_FIFO_EN == 1
	g_tUart5.uart = UART5;						/* STM32 串口设备 */
	g_tUart5.pTxBuf = g_TxBuf5;					/* 发送缓冲区指针 */
	g_tUart5.pRxBuf = g_RxBuf5;					/* 接收缓冲区指针 */
	g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart5.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart5.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart5.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart5.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart5.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart5.usTxCount = 0;						/* 待发送的数据个数 */	
	g_tUart5.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart5.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart5.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
#endif


#if UART6_FIFO_EN == 1
	g_tUart6.uart = USART6;						/* STM32 串口设备 */
	g_tUart6.pTxBuf = g_TxBuf6;					/* 发送缓冲区指针 */
	g_tUart6.pRxBuf = g_RxBuf6;					/* 接收缓冲区指针 */
	g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart6.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart6.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart6.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart6.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart6.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart6.usTxCount = 0;						/* 待发送的数据个数 */	
	g_tUart6.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart6.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart6.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
#endif
}

/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-F4开发板
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

#if UART1_FIFO_EN == 1		/* 串口1 TX = PA9   RX = PA10 或 TX = PB6   RX = PB7*/

 //GPIO端口设置


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = UART1_BAUD;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口1



	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */


#endif

#if UART2_FIFO_EN == 1		/* 串口2 TX = PD5   RX = PD6 或  TX = PA2， RX = PA3  */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);  //使能USART2
    //USART1_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

    //USART1_RX	  GPIOA.3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3


    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = UART2_BAUD;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为9位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_Even;//奇校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    //使能串口2

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */

    
#endif

#if UART3_FIFO_EN == 1			/* 串口3 TX = PB10   RX = PB11 */

#endif

#if UART4_FIFO_EN == 1			/* 串口4 TX = PC10   RX = PC11 */

#endif

#if UART5_FIFO_EN == 1			/* 串口5 TX = PC12   RX = PD2 */

#endif

#if UART6_FIFO_EN == 1			/* PG14/USART6_TX , PC7/USART6_RX,PG8/USART6_RTS, PG15/USART6_CTS */

#endif
}

/*
*********************************************************************************************************
*	函 数 名: ConfigUartNVIC
*	功能说明: 配置串口硬件中断.
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

#if UART1_FIFO_EN == 1
	/* 使能串口1中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
#endif

#if UART2_FIFO_EN == 1
	/* 使能串口2中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
#endif

#if UART3_FIFO_EN == 1
	/* 使能串口3中断t */

#endif

#if UART4_FIFO_EN == 1
	/* 使能串口4中断t */

#endif

#if UART5_FIFO_EN == 1
	/* 使能串口5中断t */

#endif

#if UART6_FIFO_EN == 1
	/* 使能串口6中断t */

#endif
}




/*
*********************************************************************************************************
*	函 数 名: UartGetChar
*	功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*	形    参: _pUart : 串口设备
*			  _pByte : 存放读取数据的指针
*	返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	DISABLE_INT();
	usCount = _pUart->usRxCount;
	ENABLE_INT();

	/* 如果读和写索引相同，则返回0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
		DISABLE_INT();
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		ENABLE_INT();
		return 1;
	}
}



/*
*********************************************************************************************************
*	函 数 名: UartIRQ
*	功能说明: 供中断服务程序调用，通用串口中断处理函数
*	形    参: _pUart : 串口设备
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	/* 处理接收中断  */
	if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		_pUart->pRxBuf[_pUart->usRxWrite] = USART_ReceiveData(_pUart->uart);
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}

		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew();
			}
		}		
	}

	/* 处理发送缓冲区空中断 */
	if (USART_GetITStatus(_pUart->uart, USART_IT_TXE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);

			/* 使能数据发送完毕中断 */
			USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
		}
		else
		{
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}

	}
	/* 数据bit位全部发送完毕的中断 */
	else if (USART_GetITStatus(_pUart->uart, USART_IT_TC) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);

			/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
		}
		else
		{
			/* 正常情况下，不会进入此分支 */
			
			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}

void USART2_IRQHandler(void)                	//串口2中断服务程序
{   
	UartIRQ(&g_tUart2);
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    UartIRQ(&g_tUart1);
}


/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为UART指针
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: uart指针
*********************************************************************************************************
*/
static UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_tUart3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return &g_tUart4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return &g_tUart5;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM6)
	{
		#if UART6_FIFO_EN == 1
			return &g_tUart6;
		#else
			return 0;
		#endif
	}
	else
	{
		/* 不做任何处理 */
		return 0;
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化串口硬件，并对全局变量赋初值.
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();		/* 必须先初始化全局变量,再配置硬件 */

	InitHardUart();		/* 配置串口的硬件参数(波特率等) */

	ConfigUartNVIC();	/* 配置串口中断 */
}

/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */		

		while (1)
		{
			uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->usTxCount;
			ENABLE_INT();

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
		}			


		/* 将新数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		ENABLE_INT();
	}

	USART_ITConfig(_pUart->uart, USART_IT_TXE, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
	}

	UartSend(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	函 数 名: comSendChar
*	功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucByte: 待发送的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}


/*
*********************************************************************************************************
*	函 数 名: comGetChar
*	功能说明: 从串口缓冲区读取1字节，非阻塞。无论有无数据均立即返回
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _pByte: 接收到的数据存放在这个地址
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return 0;
	}

	return UartGetChar(pUart, _pByte);
}

/*
*********************************************************************************************************
*	函 数 名: comClearTxFifo
*	功能说明: 清零串口发送缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usTxWrite = 0;
	pUart->usTxRead = 0;
	pUart->usTxCount = 0;
}

/*
*********************************************************************************************************
*	函 数 名: comClearRxFifo
*	功能说明: 清零串口接收缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usRxWrite = 0;
	pUart->usRxRead = 0;
	pUart->usRxCount = 0;
}


/**
*********************************************************************************************************
*  函 数 名: UartSendData
*  功能说明: 串口发送1个字节函数
*  形    参: @USARTx串口ID(USART1,USART2),
		     @byte 要发送的字节
*  返 回 值: 无
*********************************************************************************************************
*/
void UartSendData(USART_TypeDef *USARTx, uint8_t byte)
{

    while ((USARTx->SR & 0X40) == 0)
        ;
    USARTx->DR = byte;
}


/**
*********************************************************************************************************
*  函 数 名: UartSendBytes
*  功能说明: 串口发送多个字节函数
*  形    参: @USARTx串口ID(USART1,USART2),
			 @buf 要发送的字节
*  返 回 值: 无
*********************************************************************************************************
*/
void UartSendBytes(USART_TypeDef *USARTx, uint8_t *buf, uint16_t len)
{
	
    uint8_t i = 0;
	//vTaskSuspendAll();
    for (i = 0; i < len; i++)
    {
        UartSendData(USARTx, *buf++);
		//comSendChar(COM1, *buf++);
    }	
	//xTaskResumeAll();

}

/* 
********************************************************************************************************* 
*  函 数 名: UartSendStr
*  功能说明: 串口发送字符串
*  形    参: @USARTx串口ID(USART1,USART2)，@str字符串缓冲区地址
*  返 回 值: 无 
********************************************************************************************************* 
*/ 
void UartSendStr(USART_TypeDef *USARTx,char *str)
{ 
    while (0 != *str)    
    {    
        UartSendData(USARTx, *str);    
        str++;    
    }    
}



