
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "taskApp.h"
#include "delay.h"
#include "user_call_back.h"
#include "wireless_app.h"
#include "wireless_drv.h"
#include "uart.h"
#include "stmflash.h"
#include "led.h"
#include "frame_process.h"
#include "stm32f0_eeprom.h"
#include "aes.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t uart1_rec;
uint8_t uart2_rec;

//串口接收一帧数据后处理
void UartRcvTask(void)
{
    uint16_t len, t;
    if (USART2_RX_STA & 0x8000)
    {
        len = USART2_RX_STA & 0x3fff; //得到此次接收到的数据长度
        //printf("\r\n您发送的消息为:\r\n\r\n");
        for (t = 0; t < len; t++)
        {
            UartSendData(USART2, USART2_RX_BUF[t]); //向串口1发送数据
        }

        //printf("\r\n\r\n");//插入换行
        USART2_RX_STA = 0;
    }
    if (USART_RX_STA & 0x8000)
    {
        len = USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
        //printf("\r\n您发送的消息为:\r\n\r\n");
        for (t = 0; t < len; t++)
        {
            UartSendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
        }
        Si4438_Transmit_Start(&Wireless_Buf, 10, USART_RX_BUF, len);
        //printf("\r\n\r\n");//插入换行
        USART_RX_STA = 0;
    }
}

/*
无线数据处理任务
*/
void WirelessTask(void)
{
    uint8_t recFlag = 0;
    if (WIRELESS_STATUS == Wireless_RX_Finish)
    {
        Uart1SendBytes(Wireless_Buf.Wireless_RxData, Wireless_Buf.Wireless_PacketLength);
        recFlag = 1;
        //Si4438_Receive_Start(Wireless_Channel[0]);
    }
    if (WIRELESS_STATUS == Wireless_TX_Finish)
    {
        DEBUG_Printf("Wireless_TX_Finish\r\n");
        Si4438_Receive_Start(Wireless_Channel[0]); //开始接收无线数据
    }
    else if (WIRELESS_STATUS == Wireless_RX_Failure)
    {
        WirelessRx_Timeout_Cnt = 0;
        DEBUG_Printf("Wireless_RX_Failure\r\n");
        delay_ms(30);
        Set_Property(Interrupt_Close);
        delay_ms(200);
        Si4438_Receive_Start(Wireless_Channel[0]); //开始接收无线数据
    }
//    else if ((WIRELESS_STATUS == Wireless_RX_Sync) && (WirelessRx_Timeout_Cnt > 500)) //500ms超时
//    {

//        DEBUG_Printf("Wireless_RX_Sync\r\n");
//        delay_ms(30);
//        Set_Property(Interrupt_Close);
//        delay_ms(200);
//        Si4438_Receive_Start(Wireless_Channel[0]); //开始接收无线数据
//        WirelessRx_Timeout_Cnt = 0;
//    }

    if (recFlag == 1)
    {
        Si4438_Transmit_Start(&Wireless_Buf, 22, "abcdefg", 7);
    }
}

//根据家庭组切换到新的固定通讯频道上
#ifdef Use_Rx_Hop
void Get_WireLessChannel(uint8_t *wire_chnel)
{
   uint32_t temp_val = LANGroup_Addr[0] + LANGroup_Addr[1] + LANGroup_Addr[2];
	
   //uint32_t temp_val = 0x00 + 0x29 + 0x02;
   //uint32_t temp_val = 0x00 + 0x2A + 0x5B;
    if (temp_val == 0)
    {
        wire_chnel[0] = Default_Channel;
        wire_chnel[1] = Default_Channel;
    }
    else
    {
        wire_chnel[0] = (temp_val & 0x1f) << 1; //共32个信道组，每个信道组有两个信道
        wire_chnel[1] = wire_chnel[0] + 1;
		if(wire_chnel[0] == Default_Channel)
		{
			wire_chnel[0] = wire_chnel[0] + 2;
			wire_chnel[1] = wire_chnel[0] + 1;
		} 
    }

	
  
    RF_RX_HOP_CONTROL_12[7] = Channel_Frequency_Index[wire_chnel[0]]; //放入群组的主频道
    RF_RX_HOP_CONTROL_12[8] = Channel_Frequency_Index[wire_chnel[1]]; //放入群组的备用频道
}
#else
void Get_WireLessChannel(uint8_t *wire_chnel)
{
	wire_chnel[0] = Default_Channel;
	wire_chnel[1] = Default_Channel;
}
#endif
//LED旋转灯测试
void PowerOn_Led(void)
{
    //Fashion_ParaBuf.Led4_CotrolPara = 0x43;
    //LED_SetALed(4, Purple, 63);
    LED_SetDuty(Purple, Alwayslight_Brightness);
    LED_SetALed(4, Purple, Alwayslight_Brightness);
    SysDelay_Xms(2000);

    LED_Clear(0);
}

void TouchLED_ALL(uint8_t flag)
{
	if(flag == 1)
	{
		LED_SetDuty(Purple, Alwayslight_Brightness);
		LED_SetALed(4, Purple, Alwayslight_Brightness);
	}
	else
	{
	   LED_Clear(0);
	}
}

void UpCom_Process(UpCom_Rx_TypDef *prx_ubuf, DevicePara_TypDef *p_device)
{

    if (prx_ubuf->Rx_Status == UartRx_Finished)
    {
        //prx_ubuf->Rx_Status = UartRx_FrameHead;		//避免数据没有处理完被覆盖
        if (0 == FrameData_Detect(prx_ubuf->Frame_Data, prx_ubuf->FrameTotalLen))
        {
            p_device->Pending_Flag &= 0xf0; //
            LEDR_INVERSE();

            if (prx_ubuf->Frame_Data[0] == AESFreamHeader)
                p_device->Pending_Flag |= 0x01;
            else
            {
                uint8_t frame_type = prx_ubuf->Frame_Data[Region_CmdNumber] & 0x98;

                if (frame_type == RemoteUp_CmdFrame)
                {
                    p_device->Pending_Flag |= 0x02;
#ifdef Use_Rout
                    Dis_TryFlag = 1;
                    prx_ubuf->Frame_Data[Region_SeqNumber] &= 0x7f;
#endif
                }

                else if (frame_type == Local_EventFrame || frame_type == RemoteUp_EventFrame)
                    p_device->Pending_Flag |= 0x03;
                else if (frame_type == Local_CmdFrame)
                    p_device->Pending_Flag |= 0x04;
                else
                {
                    // UpCom_RXINT_EN();
                }
            }
        }
        else
        {
            // UpCom_RXINT_EN();
        }
		
    }
}

void WireLess_Process(WLS *p_wl, DevicePara_TypDef *p_device)
{
    static uint8_t Wireless_ErrCnt = 0;

    // Get_WireLessChannel(Wireless_Channel);

    if (WIRELESS_ERROR_CODE == Wireless_CmdResponseError)
    {
        WIRELESS_ERROR_CODE = Wireless_NoError;
        /*Si4438_Receive_Start(Wireless_Channel[0]);   //Start Receive*/
        if (++Wireless_ErrCnt > 10)
            WIRELESS_ERROR_CODE = Wireless_InitError;
    }

    if (WIRELESS_ERROR_CODE == Wireless_ExecuteError) //Channel Busy
    {
        WIRELESS_STATUS = Wireless_Free;

        Si4438_Receive_Start(Wireless_Channel[0]); //Start Receive
    }

    if (WIRELESS_STATUS == Wireless_TX_Finish)
    {
        WIRELESS_STATUS = Wireless_Free;
        Wireless_ErrCnt = 0;
        //LEDR_INVERSE();
        Si4438_Receive_Start(Wireless_Channel[0]); //Start Receive
    }

    else if (WIRELESS_STATUS == Wireless_RX_Failure)
    {
        WIRELESS_STATUS = Wireless_Free;
        Wireless_ErrCnt = 0;
        Res_FifoInfo(RESET_RECEIVE);
        //Si4438_Receive_Start(Wireless_Channel[0]);         //Start Receive
    }

    else if (WIRELESS_STATUS == Wireless_RX_Finish) //Receive Finish
    {
        WIRELESS_STATUS = Wireless_Free;
        Wireless_ErrCnt = 0;

        if (0 == FrameData_Detect(p_wl->Wireless_RxData, p_wl->Wireless_PacketLength))
        {
#ifdef Use_74dcode
            if (((p_wl->Wireless_RxData[Region_CmdNumber] & 0X07) != 0X07) && ((p_wl->Wireless_RxData[Region_CmdNumber] & 0X07) != 0X02)) //带编码能力的电器回复无cmd标识
            {
                uint8_t Datacnt = Region_DataAFNNumber, Realcnt = 0;
                volatile uint16_t dest;
                uint8_t Datalength = (p_wl->Wireless_PacketLength - 9) / 2; //从数据区到校验区开始解码

                while (Datalength--)
                {
                    dest = (p_wl->Wireless_RxData[Datacnt]);
                    dest = dest << 8;
                    dest |= p_wl->Wireless_RxData[Datacnt + 1];                //接收到的是单位1Byte的74码数据，处理成2Byte解码使用
                    p_wl->Wireless_RealData[Realcnt++] = decode16_74bit(dest); //解码成真实数据
                    Datacnt += 2;
                }
                memset(&p_wl->Wireless_RxData[Region_DataAFNNumber], (uint8_t)0, p_wl->Wireless_PacketLength); //把接收到的编码帧清除，否则会有无用数据在帧的后缀出现
                memmove(&p_wl->Wireless_RxData[Region_DataAFNNumber], p_wl->Wireless_RealData, Realcnt);       //把真实长度的数据放到接收数据buf中
                memset(p_wl->Wireless_RealData, (uint8_t)0, Realcnt);
                p_wl->Wireless_PacketLength = Realcnt + Region_DataAFNNumber; //数据校验区加协议区总长度
            }
#endif

            uint8_t frame_type = p_wl->Wireless_RxData[Region_CmdNumber] & 0x98;
#ifdef Use_Rout
            uint8_t rout_flag = p_wl->Wireless_RxData[Region_SeqNumber] & 0x80;
            uint8_t Remain_series = p_wl->Wireless_RxData[Region_DataAFNNumber + p_wl->Wireless_RxData[Region_DataLenNumber] + 1];
#endif

            p_device->Pending_Flag &= 0x0f;
            LEDG_INVERSE();
#ifdef Use_Rout
            if ((frame_type == RemoteDown_CmdFrame) && (!rout_flag || (rout_flag && !Remain_series)))
                p_device->Pending_Flag |= 0x10;
            else if ((frame_type == RemoteDown_EventFrame) && (!rout_flag || (rout_flag && !Remain_series)))
                p_device->Pending_Flag |= 0x20;
#else
            if (frame_type == RemoteDown_CmdFrame)
                p_device->Pending_Flag |= 0x10;
            else if (frame_type == RemoteDown_EventFrame)
                p_device->Pending_Flag |= 0x20;
#endif
            else
                Si4438_Receive_Start(Wireless_Channel[0]);
        }
        else if (p_wl->Wireless_RxData[Region_HeaderNumber] == HKFreamHeader) //帧校验错误时但为正确格式，尝试解码纠错
        {
#ifdef Use_74dcode
            //无线帧校验错误尝试解码恢复数据
            if (((p_wl->Wireless_RxData[Region_CmdNumber] & 0X07) != 0X07) && ((p_wl->Wireless_RxData[Region_CmdNumber] & 0X07) != 0X02)) //带编码能力的电器回复无cmd标识，可以尝试解码恢复
            {
                uint8_t Datacnt = Region_DataAFNNumber, Realcnt = 0;
                volatile uint16_t dest;
                uint8_t Datalength = (p_wl->Wireless_PacketLength - 9) / 2; //从数据区到校验区开始解码

                while (Datalength--)
                {
                    dest = (p_wl->Wireless_RxData[Datacnt]);
                    dest = dest << 8;
                    dest |= p_wl->Wireless_RxData[Datacnt + 1];                //接收到的是单位1Byte的74码数据，处理成2Byte解码使用
                    p_wl->Wireless_RealData[Realcnt++] = decode16_74bit(dest); //解码成真实数据
                    Datacnt += 2;
                }
                memset(&p_wl->Wireless_RxData[Region_DataAFNNumber], (uint8_t)0, p_wl->Wireless_PacketLength); //把接收到的编码帧清除，否则会有无用数据在帧的后缀出现
                memmove(&p_wl->Wireless_RxData[Region_DataAFNNumber], p_wl->Wireless_RealData, Realcnt);       //把真实长度的数据放到接收数据buf中
                memset(p_wl->Wireless_RealData, (uint8_t)0, Realcnt);
                p_wl->Wireless_PacketLength = Realcnt + Region_DataAFNNumber; //数据校验区加协议区总长度
            }
            else //不带编码能力的电器回复校验错误直接返回
            {
                Si4438_Receive_Start(Wireless_Channel[0]);
                return;
            }
#endif
#ifdef Use_74dcode
            //校验解码后的帧是否正确
            if (0 == FrameData_Detect(p_wl->Wireless_RxData, p_wl->Wireless_PacketLength))
            {
                uint8_t frame_type = p_wl->Wireless_RxData[Region_CmdNumber] & 0x98;
#ifdef Use_Rout
                uint8_t rout_flag = p_wl->Wireless_RxData[Region_SeqNumber] & 0x80;
                uint8_t Remain_series = p_wl->Wireless_RxData[Region_DataAFNNumber + p_wl->Wireless_RxData[Region_DataLenNumber] + 1];
#endif

                p_device->Pending_Flag &= 0x0f;
                LEDG_INVERSE();
#ifdef Use_Rout
                if ((frame_type == RemoteDown_CmdFrame) && (!rout_flag || (rout_flag && !Remain_series)))
                    p_device->Pending_Flag |= 0x10;
                else if ((frame_type == RemoteDown_EventFrame) && (!rout_flag || (rout_flag && !Remain_series)))
                    p_device->Pending_Flag |= 0x20;
#else
                if (frame_type == RemoteDown_CmdFrame)
                    p_device->Pending_Flag |= 0x10;
                else if (frame_type == RemoteDown_EventFrame)
                    p_device->Pending_Flag |= 0x20;
#endif
                else
                    Si4438_Receive_Start(Wireless_Channel[0]);
            }
            else
                Si4438_Receive_Start(Wireless_Channel[0]);
#else
            Si4438_Receive_Start(Wireless_Channel[0]);
#endif
        }
        else
            Si4438_Receive_Start(Wireless_Channel[0]);
    }
}

void System_8msTick_Process(void)
{
    if ((WIRELESS_STATUS == Wireless_RX_Receiving) && (!!NIRQ_RDPIN() == 0))
    {
        Si4438_Receive_Start(Wireless_Channel[0]); //Restart Receive
    }
    if ((WIRELESS_ERROR_CODE == Wireless_Busy)) //Channel Busy
    {
        if (Wireless_Buf.Channel_Busy_cnt) // Send Ready
        {
            if (++Wireless_Buf.Channel_Busy_cnt > BUSY_TIMEOUT)
            {
                Wireless_Buf.Channel_Busy_cnt = 0;

                //Set_Property(Interrupt_Close);
                WIRELESS_ERROR_CODE = Wireless_NoError;
                Si4438_Receive_Start(Wireless_Channel[0]);
            }
        }
        else
            WIRELESS_ERROR_CODE = Wireless_NoError;
    }
    if (WirelessRx_Timeout_Cnt)
    {
        if (++WirelessRx_Timeout_Cnt > WirelessRx_Timeout_Threshold)
        {
            WirelessRx_Timeout_Cnt = 0;

            Si4438_Receive_Start(Wireless_Channel[0]);
        }
    }
    if (Wireless_Buf.Sent_TimeOut_Cnt > 0)
    {
        if (++Wireless_Buf.Sent_TimeOut_Cnt > WLSent_TimeOut_Val)
        {
            Wireless_Buf.Sent_TimeOut_Cnt = 0;
            WIRELESS_STATUS = Wireless_TX_Finish;
        }
    }
    if (UpCom_TxBuf.Tx_Status == UartTx_End)
    {
        if (++UpCom_TxBuf.TxEnd_Cnt > Tx_GapTime_Size)
        {
            UpCom_TxBuf.TxEnd_Cnt = 0;
            UpCom_TxBuf.Tx_Status = UartTx_Finished;
        }
    }
    if (UpCom_RxBuf.Over_time_count)
    {
        if (++UpCom_RxBuf.Over_time_count > Up_TimeOut_Val)
        {
            UpCom_RxBuf.Over_time_count = 0;
            UpCom_RxBuf.Rx_Status = UartRx_FrameHead;
        }
    }
#ifdef Use_Rout
    if (Frame_Wait_Cnt)
    {
        Frame_Wait_Cnt++;
    }
#endif
}

void Led_Process(DevicePara_TypDef *p_device, Led_Color_TypeDef colour)
{

    if ((p_device->Touch_State & 0x10) == 0x10)
    {
        p_device->Touch_State &= ~0x10;
        p_device->Led_Cotrol_DisFlag |= 0x01;
        LED_Clear(1);
        LED_SetALed(0, colour, Alwayslight_Brightness);
        LED_SetALed(1, colour, Alwayslight_Brightness);
    }
    else if ((p_device->Touch_State & 0x01) == 0x01)
    {
        p_device->Touch_State &= ~0x01;
        p_device->Led_Cotrol_DisFlag &= ~0x01;
        LED_SetALed(0, colour, 0);
        LED_SetALed(1, colour, 0);
    }
#if (0)
    if ((p_device->Touch_State & 0x20) == 0x20)
    {
        p_device->Touch_State &= ~0x20;
        p_device->Led_Cotrol_DisFlag |= 0x02;
        LED_Clear(1);
        LED_SetALed(2, colour, SN3218_PWM_DUTY);
        LED_SetALed(3, colour, SN3218_PWM_DUTY);
    }
    else if ((p_device->Touch_State & 0x02) == 0x02)
    {
        p_device->Touch_State &= ~0x02;
        p_device->Led_Cotrol_DisFlag &= ~0x02;
        LED_SetALed(2, colour, 0);
        LED_SetALed(3, colour, 0);
    }
#endif
    if ((p_device->Touch_State & 0x40) == 0x40)
    {
        p_device->Touch_State &= ~0x40;
        p_device->Led_Cotrol_DisFlag |= 0x04;
        LED_Clear(1);
        LED_SetALed(5, colour, Alwayslight_Brightness);
        LED_SetALed(6, colour, Alwayslight_Brightness);
    }
    else if ((p_device->Touch_State & 0x04) == 0x04)
    {
        p_device->Touch_State &= ~0x04;
        p_device->Led_Cotrol_DisFlag &= ~0x04;
        LED_SetALed(5, colour, 0);
        LED_SetALed(6, colour, 0);
    }
#if (0)
    if ((p_device->Touch_State & 0x80) == 0x80)
    {
        p_device->Touch_State &= ~0x80;
        p_device->Led_Cotrol_DisFlag |= 0x08;
        LED_Clear(1);
        LED_SetALed(7, colour, SN3218_PWM_DUTY);
        LED_SetALed(8, colour, SN3218_PWM_DUTY);
    }
    else if ((p_device->Touch_State & 0x08) == 0x08)
    {
        p_device->Touch_State &= ~0x08;
        p_device->Led_Cotrol_DisFlag &= ~0x08;
        LED_SetALed(7, colour, 0);
        LED_SetALed(8, colour, 0);
    }
#endif
    if (!p_device->Led_Cotrol_DisFlag)
    {
        uint8_t led_setmode, color_val;
        Led_Color_TypeDef led_color;

        led_setmode = (p_device->Led_CotrolPara & 0xf0) >> 4;
        color_val = p_device->Led_CotrolPara & 0x03;
        if (color_val == 0x00)
            led_color = Colourless;
        else if (color_val == 0x01)
            led_color = Blue;
        else if (color_val == 0x02)
            led_color = Orange;
        else if (color_val == 0x03)
            led_color = Purple;
        switch (led_setmode)
        {
        case Led_SetMode1:
            Led_Mode1(led_color);
            break;

        case Led_SetMode2:
            Led_Mode2(led_color);
            break;

        case Led_SetMode3:
            Led_Mode3(led_color);
            break;

        case Led_SetMode5:
            Led_Mode5(p_device, Purple);
            break;

        default:
            break;
        }
    }
    Led_Mode4(p_device);
}
void MacFrame_Process(uint8_t *p_source, uint8_t *p_buf)
{
    uint16_t crc16_val;

    if (p_source[1] == MACWrite_Cmd_Request)
    {
        if (memcmp(Local_MAC_Addr, p_source, MAC_Data_Len + 5) != 0) //只能烧一次
        {
            Eeprom_ErasePage(MAC_EAddr);
            Eeprom_Write(MAC_EAddr, (uint16_t *)p_source, MAC_Data_Len + 5);
            Eeprom_Read(MAC_EAddr, (uint16_t *)Local_MAC_Addr, MAC_Data_Len + 5);
            
            if (memcmp(Local_MAC_Addr, p_source, MAC_Data_Len + 5) == 0)
            {
                p_buf[1] = MACWrite_Yes_Response;
                p_buf[3] = 0xff;
            }
            else
            {
                p_buf[1] = MACWrite_NO_Response;
                p_buf[3] = 0x00;
            }
        }
        else
        {
            p_buf[1] = MACWrite_Yes_Response;
            p_buf[3] = 0xff;
        }
        p_buf[0] = AESFreamHeader;
        p_buf[2] = 1;
        crc16_val = CRC16_2(p_buf, 4);
        p_buf[4] = crc16_val >> 8;
        p_buf[5] = crc16_val & 0xff;

        UpUart_DataTx(p_buf, 6, &UpCom_TxBuf);
    }
}

uint8_t Remain_series;
void RxData_Process(HKFrame_TypDef *p_framebuf, DevicePara_TypDef *p_device)
{

    if (p_device->Pending_Flag & 0x0f)
    {
        uint8_t pending_val = p_device->Pending_Flag & 0x0f;

        if (pending_val == 0x01)
        {
            if (UpCom_TxBuf.Tx_Status != UartTx_Finished)
                return;
#ifdef USE_USAT_RSAKey
            AesFrame_Process(UpCom_RxBuf.Frame_Data);
#endif
            MacFrame_Process(UpCom_RxBuf.Frame_Data, p_framebuf->FrameProcess_Buf);
        }
        else
        {
            if (pending_val == 0x02)
            {
                if ((WIRELESS_STATUS == Wireless_TX_Sending))
                //UpUart_DataTx(p, 5, &UpCom_TxBuf);
#ifdef Use_Rout
                    if ((UpCom_TxBuf.Tx_Status != UartTx_Finished) || (WIRELESS_STATUS == Wireless_TX_Sending))
                        return;
#else
                    if ((UpCom_TxBuf.Tx_Status != UartTx_Finished) || (WIRELESS_STATUS == Wireless_TX_Sending))
                        return;
#endif
                //Frame_Process(Up_RxBuf.Frame_Data, Up_RxBuf.FrameTotalLen, p_framebuf, p_device);
            }
            else if (pending_val == 0x03)
            {
                //Frame_Process(Up_RxBuf.Frame_Data, Up_RxBuf.FrameTotalLen, p_framebuf, p_device);
            }
            else if (pending_val == 0x04)
            {
                if (UpCom_TxBuf.Tx_Status != UartTx_Finished)
                    return;
            }

            Frame_Process(UpCom_RxBuf.Frame_Data, UpCom_RxBuf.FrameTotalLen, p_framebuf, p_device);
        }
        p_device->Pending_Flag &= ~0x0f;
        // UpCom_RXINT_EN();
    }

    if (p_device->Pending_Flag & 0xf0)
    {
        uint8_t pending_val = p_device->Pending_Flag & 0xf0;
#ifdef Use_Rout
        uint8_t rout_flag = Wireless_Buf.Wireless_RxData[Region_SeqNumber] & 0x80;
        //uint8_t Remain_series = Wireless_Buf.Wireless_RxData[Region_DataAFNNumber + Wireless_Buf.Wireless_RxData[Region_DataLenNumber] + 1];
        Remain_series = Wireless_Buf.Wireless_RxData[Wireless_Buf.Wireless_PacketLength - 3 - 5];
#endif

#ifdef Use_Rout
        if (rout_flag && Remain_series)
        {
            p_device->Pending_Flag &= ~0xf0;
            Si4438_Receive_Start(Wireless_Channel[0]);
            return;
        }
#endif
        if (pending_val == 0x10)
        {
            if (UpCom_TxBuf.Tx_Status != UartTx_Finished)
                return;
        }
        else if (pending_val == 0x20)
        {
            if ((UpCom_TxBuf.Tx_Status != UartTx_Finished) || (WIRELESS_STATUS == Wireless_TX_Sending))
                return;
        }

        Frame_Process(Wireless_Buf.Wireless_RxData, Wireless_Buf.Wireless_PacketLength, p_framebuf, p_device);
        p_device->Pending_Flag &= ~0xf0;

        if (WIRELESS_STATUS != Wireless_TX_Sending)
            Si4438_Receive_Start(Wireless_Channel[0]);
    }
}

void Touch_Process(DevicePara_TypDef *p_device)
{
    if (!p_device->Touch_Val)
    {
        Touch1_Detect(p_device);
        Touch2_Detect(p_device);
        Touch3_Detect(p_device);
        Touch4_Detect(p_device);
        if (p_device->Touch_Val)
            p_device->UpReport_Flag |= (1 << Key_Event);
    }
}

void MacAddr_Read(void)
{
	uint32_t delay_cnt = 0;
    do
    {
        SysDelay_Xms(20);

        UpCom_Process(&UpCom_RxBuf, &Device_ParaBuf);
        RxData_Process(&HKFrame_Buf, &Device_ParaBuf);
        Eeprom_Read(MAC_EAddr, (uint16_t *)Local_MAC_Addr, MAC_Data_Len + 5);
		delay_cnt ++;
		if(delay_cnt > 50)
		{
			delay_cnt = 0;
			//DEBUG_Printf("Please input Mac Frame\r\n"); 
			UartSendData(USART1,0x0C);
		}
    } while (0 != FrameData_Detect(Local_MAC_Addr, MAC_Data_Len + 5));
}

void Aes_Key_Read(void)
{
#ifdef USE_USAT_RSAKey
    do
    {
        SysDelay_Xms(1000);
        UpCom_Process(&UpCom_RxBuf, &Device_ParaBuf);
        RxData_Process(&HKFrame_Buf, &Device_ParaBuf);
        Eeprom_Read(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2);
    } while (aes_out[AesBuf_Size - 2] != Secret_Key_Yes);
#else
    Eeprom_Read(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2);

#endif

    Secret_GKey_Flag = 1;
#if (0) //??????
    if (Secret_Key_Yes == aes_out[AesBuf_Size - 1])
    {
        memmove(LANGroup_Addr, &aes_out[RsaByte_Size * 3], 3);
        Rsa_Decode(aes_out);
        key_expansion(aes_out, aes_w);
        Secret_KeyOk_Flag = 1;
    }
#else
    memset(&aes_out[RsaByte_Size * 2], 0, 19); //???Buf?0
#endif
}

void LANGroup_AddrRead(void)
{
	uint8_t temp[4]={0};
	Eeprom_Read(GD_ADDR,(uint16_t*)temp,4/2);
	memcpy(LANGroup_Addr,temp,3);
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  delay_init(72);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

    HAL_TIM_Base_Start_IT(&htim1);
    HAL_UART_Receive_IT(&huart1, &uart1_rec, 1);
    HAL_UART_Receive_IT(&huart2, &uart2_rec, 1);
	SN3218_Init();
	PowerOn_Led();
	MacAddr_Read();
	Aes_Key_Read();
	LANGroup_AddrRead();

	Get_WireLessChannel(Wireless_Channel);
    Wireless_Init();
    Si4438_Receive_Start(Wireless_Channel[0]);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    DEBUG_Printf("System loop Start\r\n"); 
    while (1)
    {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
//        UartRcvTask();
//        WirelessTask();
   
#ifdef USE_IWDG
        IWDG_FEED();
#endif
        while (!TICK_8ms)
            ;

        TICK_8ms = 0;
        System_8msTick_Process();

        WireLess_Process(&Wireless_Buf, &Device_ParaBuf);
        UpCom_Process(&UpCom_RxBuf, &Device_ParaBuf);
        RxData_Process(&HKFrame_Buf, &Device_ParaBuf);
		if(UpCom_RxBuf.Rx_Status == UartRx_Finished)
		{
			UpCom_RxBuf.Rx_Status = UartRx_FrameHead;     //再次进入串口中断接收状态
		}
        Touch_Process(&Device_ParaBuf);
        if (UpCom_TxBuf.Tx_Status == UartTx_Finished)
        {
            UpReport_Process(&HKFrame_Buf, &Device_ParaBuf);
        }

        Led_Process(&Device_ParaBuf, Orange);
#ifdef Use_Rout
        //Path_Try_Process(RoutFrame_buf, RoutRegion, &Rout_TryBuf, &Rout_TempTab);
#endif
      
    }
	

  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LEDG_Pin|LEDR_Pin|SI4438_SDN_Pin|SI4438_NSS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_SN3218A_SDB_Pin|LED_SN3218A_SDA_Pin|LED_SN3218A_SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TOUCH2_Pin TOUCH4_Pin TOUCH1_Pin */
  GPIO_InitStruct.Pin = TOUCH2_Pin|TOUCH4_Pin|TOUCH1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LEDG_Pin LEDR_Pin SI4438_SDN_Pin SI4438_NSS_Pin */
  GPIO_InitStruct.Pin = LEDG_Pin|LEDR_Pin|SI4438_SDN_Pin|SI4438_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SI4438_nIRQ_Pin */
  GPIO_InitStruct.Pin = SI4438_nIRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SI4438_nIRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TOUCH3_Pin */
  GPIO_InitStruct.Pin = TOUCH3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TOUCH3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_SN3218A_SDB_Pin LED_SN3218A_SDA_Pin LED_SN3218A_SCL_Pin */
  GPIO_InitStruct.Pin = LED_SN3218A_SDB_Pin|LED_SN3218A_SDA_Pin|LED_SN3218A_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
