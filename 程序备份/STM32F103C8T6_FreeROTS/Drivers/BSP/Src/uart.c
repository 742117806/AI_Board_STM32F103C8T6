//实现与串口相关的功能函数

#include "uart.h"
#include "cmsis_os.h"

/*
**********************************************************************************************************
                      函数声明
**********************************************************************************************************
*/

/*
**********************************************************************************************************
                       变量
**********************************************************************************************************
*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern SemaphoreHandle_t xSemaphore_uartTx;

///*
//*********************************************************************************************************
//*  函 数 名: Uart1SendData
//*  功能说明: 串口1发送1个字节函数
//*  形    参: @byte 要发送的字节
//*  返 回 值: 无
//*********************************************************************************************************
//*/
//void Uart1SendData(uint8_t byte)
//{
//
//    while ((USART1->SR & 0X40) == 0)
//        ;
//         USART1->DR = byte;
//}

///*
//*********************************************************************************************************
//*  函 数 名: Uart1SendBytes
//*  功能说明: 串口1发送多个字节函数
//*  形    参: @buf 发送是缓冲区地址，@len 发送字节长度
//*  返 回 值: 无
//*********************************************************************************************************
//*/
//void Uart1SendBytes(uint8_t *buf, uint16_t len)
//{
//    uint8_t i = 0;
//    for (i = 0; i < len; i++)
//    {
//        Uart1SendData(*buf++);
//    }
//}

/*
*********************************************************************************************************
*  函 数 名: UartSendData
*  功能说明: 串口发送1个字节函数
*  形    参: @USARTx串口ID(USART1,USART2),@byte 要发送的字节
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
*  形    参: @USARTx串口ID(USART1,USART2)，
			 @buf 发送是缓冲区地址，
			 @len 发送字节长度
*  返 回 值: 无
*********************************************************************************************************
*/
void UartSendBytes(USART_TypeDef *USARTx, uint8_t *buf, uint16_t len)
{
    uint8_t i = 0;
    xSemaphoreTake(xSemaphore_uartTx, portMAX_DELAY);
    for (i = 0; i < len; i++)
    {
        UartSendData(USARTx, *buf++);
    }
    xSemaphoreGive(xSemaphore_uartTx);
}

/**
*********************************************************************************************************
*  函 数 名: UartSendStr
*  功能说明: 串口发送字符串
*  形    参: @USARTx串口ID(USART1,USART2)，
			 @str字符串缓冲区地址
*  返 回 值: 无
*********************************************************************************************************
*/
void UartSendStr(USART_TypeDef *USARTx, char *str)
{
    while (0 != *str)
    {
        UartSendData(USARTx, *str);
        str++;
    }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART2 and Loop until the end of transmission */

    UartSendData(USART1, ch);

    //		HAL_NVIC_DisableIRQ(USART2_IRQn);
    //	HAL_UART_Transmit(&husart2, (uint8_t *)&ch, 1,0xFFFF);
    //	HAL_NVIC_EnableIRQ(USART2_IRQn);

    return ch;
}

/*
*********************************************************************************************************
*  函 数 名: UpUart_RX_INT_Process
*  功能说明: 串口接收上位机（或者PC端）发来的数据
*  形    参: @rx_data 串口数据，@pu_buf 存放数据的缓存区地址
*  返 回 值: 无
*********************************************************************************************************
*/

void UpUart_RX_INT_Process(uint8_t rx_data, UpCom_Rx_TypDef *pu_buf)
{
    static uint8_t Len_Cnt;
    switch (pu_buf->Rx_Status)
    {
    case UartRx_FrameHead:
        if ((rx_data == HKFreamHeader) && Secret_GKey_Flag) //公钥未烧入，不接收HK帧
        {
            pu_buf->Frame_Data[0] = rx_data;
            pu_buf->FrameTotalLen = 1;
            Len_Cnt = 0;
            pu_buf->Over_time_count = 1;
            pu_buf->Rx_Status = UartRx_FrameAddr;
        }
        else if (rx_data == AESFreamHeader)
        {
            pu_buf->Frame_Data[0] = rx_data;
            pu_buf->Over_time_count = 1;
            pu_buf->Rx_Status = UartRx_AesCmd;
        }
        break;

    case UartRx_FrameAddr:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        pu_buf->FrameTotalLen++;
        if (++Len_Cnt >= LogicAddr_Len)
            pu_buf->Rx_Status = UartRx_FrameSeq;
        break;

    case UartRx_FrameSeq:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        pu_buf->FrameTotalLen++;
        pu_buf->Rx_Status = UartRx_DataCmd;
        break;

    case UartRx_DataCmd:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        pu_buf->FrameTotalLen++;
        pu_buf->Rx_Status = UartRx_Datalen;
        break;

    case UartRx_Datalen:
        if (rx_data > HKData_LenMax)
        {
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = UartRx_FrameHead;
        }
        else
        {
            pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
            pu_buf->FrameTotalLen++;
            Len_Cnt = 0;
            if (rx_data == 0)
                pu_buf->Rx_Status = UartRx_FrameCs;
            else
                pu_buf->Rx_Status = UartRx_Data;
        }
        break;

    case UartRx_Data:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        pu_buf->FrameTotalLen++;
        if (++Len_Cnt >= pu_buf->Frame_Data[Region_DataLenNumber])
        {
            Len_Cnt = 0;
            pu_buf->Rx_Status = UartRx_FrameCs;
        }

        break;

    case UartRx_FrameCs:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        pu_buf->FrameTotalLen++;
        if (++Len_Cnt >= 2)
            pu_buf->Rx_Status = UartRx_FrameEnd;
        break;

    case UartRx_FrameEnd:
        if (rx_data == HKFreamEnd) //判断结束符是否正确
        {
            //UpCom_RXINT_DIS(); //关中断
            pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
            pu_buf->FrameTotalLen++;
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = UartRx_Finished;
        }
        else
        {
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = UartRx_FrameHead;
        }
        break;

    ///////////////////////////接收AES帧 //////////////////
    case UartRx_AesCmd:                                                      //正在接收数据命令
        if (rx_data == GKWrite_Cmd_Request || rx_data == MACWrite_Cmd_Request) //只能接受烧公钥命令
        {
            pu_buf->Frame_Data[1] = rx_data;
            pu_buf->Rx_Status = UartRx_Aeslen;
        }
        else
        {
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = UartRx_FrameHead;
        }
        break;

    case UartRx_Aeslen: //正在接收长度字节
        if (rx_data == 0)
        {
            pu_buf->Frame_Data[2] = rx_data;
            pu_buf->FrameTotalLen = 3;
            pu_buf->Rx_Status = UartRx_AesFrameCs1;
        }
        else if (rx_data > GK_Data_Len) //只能接受烧公钥命令
        {
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = UartRx_FrameHead; //接收错误，返回到接收帧头的起始状态
        }

        else
        {
            pu_buf->Frame_Data[2] = rx_data;
            pu_buf->FrameTotalLen = 3;
            pu_buf->Rx_Status = UartRx_AesData;
        }
        break;

    case UartRx_AesData: //正在接收数据
        pu_buf->Frame_Data[pu_buf->FrameTotalLen++] = rx_data;
        if (pu_buf->FrameTotalLen >= pu_buf->Frame_Data[2] + 3)
            pu_buf->Rx_Status = UartRx_AesFrameCs1;
        break;

    case UartRx_AesFrameCs1: //正在接收校验和
        pu_buf->Frame_Data[pu_buf->FrameTotalLen++] = rx_data;
        pu_buf->Rx_Status = UartRx_AesFrameCs2;
        break;

    case UartRx_AesFrameCs2: //正在接收校验和
        pu_buf->Frame_Data[pu_buf->FrameTotalLen++] = rx_data;
        //UART_RXDIS();    //停止接收
        //UpCom_RXINT_DIS(); //关中断
        pu_buf->Over_time_count = 0;
        pu_buf->Rx_Status = UartRx_Finished;
        break;

    default:
        break;
    }
}
