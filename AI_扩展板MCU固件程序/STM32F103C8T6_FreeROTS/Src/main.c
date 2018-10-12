
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

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
void StartDefaultTask(void const *argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t uart1_rec;
uint8_t uart2_rec;
extern osThreadId LedTaskHandle;
extern void MacFrame_Process(uint8_t *p_source, uint8_t *p_buf);
extern uint8_t SecretKey_Process(DeviceInfo_t *p_deviceInfo);
extern osThreadId UartTaskHandle;

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
        if (wire_chnel[0] == Default_Channel)
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



//扩展板自身应答给安卓板
void FrameCmdLocalAck(FRAME_CMD_t *frameCmd,uint8_t* ack_content,uint8_t content_len)
{
    FRAME_CMD_t frameAck;
	uint8_t frameLen;
	
	memcpy((uint8_t*)&frameAck, (uint8_t*)frameCmd, 16);
	frameAck.addr_DA = 0xFF;
	frameAck.addr_GA[0] = 0xFF;
	frameAck.addr_GA[1] = 0xFF;
	frameAck.addr_GA[2] = 0xFF;
	frameAck.Ctrl.dir = 1;
	if(content_len > 0)
	{
		frameAck.DataLen = content_len+4;
		memcpy(frameAck.userData.content,ack_content,content_len);
	}
	else
	{
		frameAck.DataLen = 0;
	}
	
	
	
	frameLen = Frame_Compose((uint8_t *)&frameAck);
	vTaskSuspendAll(); //开启任务调度锁
    UartSendBytes(USART1, (uint8_t *)&frameAck, frameLen);
    xTaskResumeAll(); //关闭任务调度锁
}
//判断是否符合帧协议
//返回1正确，0错误
uint8_t FrameRouterDetect(uint8_t *rx_buff)
{
	FRAME_ROUTER_CMD_t *frameData = (FRAME_ROUTER_CMD_t*)rx_buff;					//通信帧数据结构
	FRAME_ROUTER_EXT_CMD_t* frameData_ext = (FRAME_ROUTER_EXT_CMD_t*)rx_buff;        //配网帧数据结构
	//判断帧头
	if((frameData->head_h != 0x69)||(frameData->head_l != 0x69))
	{
		return 0;
	} 
	//FrameData_74Convert()
	//判断长度是否正确
	if((frameData->len+frameData->len_c)!=0xff)
	{
		return 0;
	}
	return 1;
}

//uint8_t frameTest[256]={
//0x69,0x69,0x39,0xC6,0x00,0x29,0x02,0x04,0x02,0x00,0x01,0x00,0x01,0x24,
//0x01,0x30,0x0F,0x00,0xAC,0x24,0x00,0x29,0x02,0x06,0x10,0x1C,0x80,0xFF,
//0xFF,0xFF,0x04,0x02,0x00,0x01,0x00,0x01,0x24,0x01,0x57,0xDB,0xF8,0xCF,
//0x2D,0xD2,0x00,0x79,0x8F,0x97,0x70,0x70,0x2A,0x1A,0x8F,0xCB,0x09,0xBF,
//0x53,0xD9,0x06,0x96,0x96};
//uint8_t frameTest[256]={
//0x69,0x69,0x32,0xCD,0x01,0x29,0x02,0x24,0x30,0x0F,0x00,0xAC,0x32,0x00,0x29,
//0x02,0x06,0x10,0x1C,0x80,0xFF,0xFF,0xFF,0x04,0x02,0x00,0x01,0x00,0x00,0x32,
//0x11,0x57,0xDB,0xF8,0xCF,0x2D,0xD2,0x00,0x79,0x8F,0x97,0x70,0x70,0x2A,0x1A,
//0x8F,0xCB,0x09,0xBF,0x53,0xCE,0x0F,0x96,0x96};
uint8_t frameTest[256]={
0x69,0x69,0x33,0xCC,0x01,0x29,0x02,0x24,0x30,0x1F,0x01,0x24,0xAC,0x32,0x00,
0x29,0x02,0x06,0x10,0x1C,0x80,0xFF,0xFF,0xFF,0x04,0x02,0x00,0x01,0x00,0x00,
0x32,0x11,0x57,0xDB,0xF8,0xCF,0x2D,0xD2,0x00,0x79,0x8F,0x97,0x70,0x70,0x2A,
0x1A,0x8F,0xCB,0x09,0xBF,0x53,0x6D,0x1D,0x96,0x96};
//路由协议帧数据处理，主要针对无线和电力线载波
void FrameRouterDataProcess(uint8_t *rx_buff,uint8_t rx_len)
{
	
	FRAME_ROUTER_CMD_t *frameData = (FRAME_ROUTER_CMD_t*)rx_buff;					//通信帧数据结构
	FRAME_ROUTER_EXT_CMD_t* frameData_ext = (FRAME_ROUTER_EXT_CMD_t*)rx_buff;        //配网帧数据结构

	uint8_t *routerTable;		//路由表指针
	uint8_t *userFrame;         //应用命令帧指针
	//FRAME_CMD_t **frame_cmd;    //应用命令帧指针的指针
	uint8_t frame_type;
	uint8_t temp[256];
	

	if(FrameRouterDetect(rx_buff)==1)
	{
		if(frameData->des_addr == 30)		//属于字节的协议帧
		{
//			if(frameData->ctrl.type == 1)	//通信帧
//			{
				routerTable = &rx_buff[11];
				userFrame = &rx_buff[11+frameData->router_len];
//				UartSendBytes(USART1,routerTable,frameData->router_len);

//			}
//			else if(frameData->ctrl.type == 0) //组网帧
//			{

//				routerTable = &rx_buff[18];
//				userFrame = &rx_buff[18+frameData_ext->router_len];
//			}
	//		delay_ms(1000);
	//		frame_cmd = (FRAME_CMD_t**)&userFrame;
	//		UartSendBytes(USART1,(uint8_t*)*frame_cmd,(*frame_cmd)->DataLen+11);
			
			frame_type = userFrame[Region_CmdNumber] & 0x98;
			//memcpy(temp,userFrame,rx_len-15-frameData->router_len);
			Wireless_Buf.Wireless_PacketLength = rx_len-15-frameData->router_len;
            memcpy(rx_buff,userFrame,Wireless_Buf.Wireless_PacketLength);
			
			if (frame_type == RemoteDown_CmdFrame)
			{
				xTaskNotify(UartTaskHandle,0x10<<8, eSetValueWithOverwrite);
			}
			else if (frame_type == RemoteDown_EventFrame)
			{
				xTaskNotify(UartTaskHandle, 0x20<<8, eSetValueWithOverwrite);
			}
			else
			{
				Si4438_Receive_Start(Wireless_Channel[0]);
			}
		}
	}
	
}

//打包一帧应用数据成为路由帧
uint8_t test2[100]={0xAC,0x32,0x00,
0x29,0x02,0x06,0x10,0x1C,0x80,0xFF,0xFF,0xFF,0x04,0x02,0x00,0x01,0x00,0x00,
0x32,0x11,0x57,0xDB,0xF8,0xCF,0x2D,0xD2,0x00,0x79,0x8F,0x97,0x70,0x70,0x2A,
0x1A,0x8F,0xCB,0x09,0xBF,0x53};
uint8_t FrameRouterCompose(uint8_t desAddr,uint8_t *srcData,uint8_t srcLen)
{
    uint8_t temp[256] = {0};
	FRAME_ROUTER_CMD_t *p = (FRAME_ROUTER_CMD_t*)temp;
	uint8_t routerLen = 0;		//路由表长度（字节）
	uint16_t crc_16;
	
	p->head_h = 0x69;
	p->head_l = 0x69;
	
	p->len = srcLen + routerLen + 11;
	p->len_c = ~(p->len);
	p->ctrl.mode = 0;
	p->ctrl.type = 1;	
	p->netNum[0] = deviceInfo.addr_GA[1];
	p->netNum[1] = deviceInfo.addr_GA[2];
	p->des_addr = desAddr;
	p->src_addr = 30; //扩展板的地址
	p->routerNum.index = 0;
	p->routerNum.type = 0xF;
	p->router_len = 0;
	memcpy(&temp[11+p->router_len],srcData,srcLen);
	crc_16 = CRC16_2(temp,p->len);
	temp[p->len] = crc_16>>8;
	temp[p->len+1] = crc_16 & 0x00ff;
	temp[p->len+2] = 0x96;
	temp[p->len+3] = 0x96;
	memcpy(srcData,temp,p->len+4);
	return p->len+4;
}

//针对配网
uint8_t FrameRouterCompose_ext(uint8_t *des_mac_Addr,uint8_t *srcData,uint8_t srcLen)
{
    uint8_t temp[256] = {0};
	FRAME_ROUTER_EXT_CMD_t *p = (FRAME_ROUTER_EXT_CMD_t*)temp;
	uint8_t routerLen = 0;		//路由表长度（字节）
	uint16_t crc_16;
	
	p->head_h = 0x69;
	p->head_l = 0x69;
	
	p->len = srcLen + routerLen + 18;
	p->len_c = ~(p->len);
	p->ctrl.mode = 0;
	p->ctrl.type = 0;	
	p->netNum[0] = deviceInfo.addr_GA[1];
	p->netNum[1] = deviceInfo.addr_GA[2];
	memcpy(p->des_addr,des_mac_Addr,8);
	p->src_addr = 30; //扩展板的地址
	p->routerNum.index = 0;
	p->routerNum.type = 0xF;
	p->router_len = 0;
	memcpy(&temp[18+p->router_len],srcData,srcLen);
	crc_16 = CRC16_2(temp,p->len);
	temp[p->len] = crc_16>>8;
	temp[p->len+1] = crc_16 & 0x00ff;
	temp[p->len+2] = 0x96;
	temp[p->len+3] = 0x96;
	memcpy(srcData,temp,p->len+4);
	return p->len+4;
}






//本地命令处理
void Local_CmdProcess(FRAME_CMD_t *frameCmd)
{

    uint8_t indexType = frameCmd->userData.Index[0];
    uint16_t indexCmd = (frameCmd->userData.Index[1] << 8) + frameCmd->userData.Index[2];
	WRITE_AES_CMD_t *p_write_cmd = (WRITE_AES_CMD_t*)frameCmd->userData.content;
	uint8_t temp[10]={0};

    switch (indexType)
    {
    case 0xFF:           //初始化数据
        switch (indexCmd)
        {
		case 0x0001:		//读软件版本号
			temp[0]= Version_Number >> 8;
			temp[1]= Version_Number & 0xff;
			FrameCmdLocalAck(frameCmd,temp,2);				
		break;
        case 0xFF00:		//写群组地址和密钥
			memcpy(deviceInfo.addr_GA,p_write_cmd->addr_GA,3);
			memcpy(deviceInfo.aes,p_write_cmd->aes,16);
			STMFLASH_Write(DEVICE_INFO_BASH_ADDR,(uint16_t*)&deviceInfo,(sizeof(deviceInfo)+1)/2);		//+1和/2是为了2字节对齐
			SecretKey_Process(&deviceInfo);          //计算出密文，存放在aes_w，供加解密用
            FrameCmdLocalAck(frameCmd,0,0);	
            break;
		default:
			FrameCmdLocalAck(frameCmd,0,0);
			break;
        }
        break;
    case 0x00:

        break;
    case 0x01:
        switch (indexCmd)
        {
        case 0x0021:
            xTaskNotify(LedTaskHandle, frameCmd->userData.content[0], eSetValueWithOverwrite);
            FrameCmdLocalAck(frameCmd,0,0);
            break;
		default:
			FrameCmdLocalAck(frameCmd,0,0);
			break;
		
        }
        break;
    case 0x02:
        break;
    case 0x03:
        break;
    case 0x04:
        break;
    default:
		FrameCmdLocalAck(frameCmd,0,0);
        break;
    }
}


//串口接收处理
void UartRx_Process(UpCom_Rx_TypDef *prx_ubuf, DevicePara_TypDef *p_device)
{
    FRAME_CMD_t *frameCmd;
	uint8_t send_len;
    frameCmd = (FRAME_CMD_t *)prx_ubuf->Frame_Data;
	

    if (prx_ubuf->Rx_Status == UartRx_Finished)
    {
        if (0 == FrameData_Detect(prx_ubuf->Frame_Data, prx_ubuf->FrameTotalLen))
        {
			if(frameCmd->FameHead == HKFreamHeader)
			{
				if(frameCmd->Ctrl.dir == 0)					//主站（安卓版）下行数据
				{
					if(frameCmd->Ctrl.relayFlag == 0)	//本地（发给扩展板）
					{
						if(frameCmd->Ctrl.eventFlag == 0) //命令
						{
							Local_CmdProcess(frameCmd);
						}
						else  if(frameCmd->Ctrl.eventFlag == 1)		//事件(应答)
						{

						}

					}
					else if(frameCmd->Ctrl.relayFlag == 1)	//转发（发给电器设备）
					{
						if(frameCmd->Ctrl.eventFlag == 0) //命令
						{
							FrameCmdLocalAck(frameCmd,0,0);
							if(frameCmd->FSQ.encryptType)  //加密
							{
								send_len = Encrypt_Convert((uint8_t*)frameCmd, frameCmd->DataLen+11, 0); 
							}
							else						//不加密
							{
								send_len = frameCmd->DataLen+11;
							}
							prx_ubuf->FrameTotalLen =  send_len;
							JOINE_NET_CMD_t *joine_cmd = (JOINE_NET_CMD_t*)frameCmd->userData.content;
							if((frameCmd->userData.Index[0] == 0xff)&&
							(frameCmd->userData.Index[1] == 0xff)&&
							(frameCmd->userData.Index[2] == 0xff))
							{
								send_len = FrameRouterCompose_ext(joine_cmd->mac,prx_ubuf->Frame_Data,prx_ubuf->FrameTotalLen);
								Si4438_Transmit_Start(&Wireless_Buf, Default_Channel, (uint8_t*)frameCmd, send_len);       //串口数据无线转发
							}
							else
							{
							   send_len = FrameRouterCompose(frameCmd->addr_DA,prx_ubuf->Frame_Data,prx_ubuf->FrameTotalLen);
							   Si4438_Transmit_Start(&Wireless_Buf,  Wireless_Channel[0], (uint8_t*)frameCmd, send_len);       //串口数据无线转发
							}
							
						}
						else  if(frameCmd->Ctrl.eventFlag == 1)		//事件(应答)
						{

						}
						
					}
				}
			}
			else if(frameCmd->FameHead == AESFreamHeader)		//烧录MAC地址
			{

				MacFrame_Process(prx_ubuf->Frame_Data,HKFrame_Buf.FrameProcess_Buf);
				memcpy(deviceInfo.mac,Local_MAC_Addr +3,8);
				deviceInfo.mac_exist = 1;
				STMFLASH_Write(DEVICE_INFO_BASH_ADDR,(uint16_t*)&deviceInfo,(sizeof(deviceInfo)+1)/2);
			}
        }

        prx_ubuf->Rx_Status = UartRx_FrameHead;
    }
}





void WireLess_Process(WLS *p_wl, DevicePara_TypDef *p_device)
{
    static uint8_t Wireless_ErrCnt = 0;
	uint8_t out_len;
	uint8_t frame_type;
	

    if (WIRELESS_STATUS == Wireless_RX_Finish) //Receive Finish
    {
        WIRELESS_STATUS = Wireless_Free;
        Wireless_ErrCnt = 0;
       
		
		#ifdef Use_74dcode
		//74解码
		if (((p_wl->Wireless_RxData[Region_CmdNumber] & 0X07) != 0X07) && ((p_wl->Wireless_RxData[Region_CmdNumber] & 0X07) != 0X02)) //带编码能力的电器回复无cmd标识
		{
			FrameData_74Convert((FRAME_CMD_t*)p_wl->Wireless_RxData,p_wl->Wireless_PacketLength,&out_len,0);
			p_wl->Wireless_PacketLength = out_len;
		}
		#endif
		if(FrameRouterDetect(p_wl->Wireless_RxData)==1)
		{
			FrameRouterDataProcess(p_wl->Wireless_RxData,p_wl->Wireless_PacketLength);	
		}
		//校验帧头和CRC16
		
//        if (0 == FrameData_Detect(p_wl->Wireless_RxData, p_wl->Wireless_PacketLength))
//        {
//            frame_type = p_wl->Wireless_RxData[Region_CmdNumber] & 0x98;
//            p_device->Pending_Flag &= 0x0f;

//            if (frame_type == RemoteDown_CmdFrame)
//            {
//				xTaskNotify(UartTaskHandle,0x10<<8, eSetValueWithOverwrite);
//            }
//            else if (frame_type == RemoteDown_EventFrame)
//            {
//				xTaskNotify(UartTaskHandle, 0x20<<8, eSetValueWithOverwrite);
//            }
//            else
//			{
//                Si4438_Receive_Start(Wireless_Channel[0]);
//			}
//        }
		else
		{
			Si4438_Receive_Start(Wireless_Channel[0]);
		} 
		if (WIRELESS_STATUS == Wireless_Free)	
		{			
			Si4438_Receive_Start(Wireless_Channel[0]);
		}
    }
    else if (WIRELESS_ERROR_CODE == Wireless_CmdResponseError)
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
}

void System_8msTick_Process(void)
{
    if ((WIRELESS_STATUS == Wireless_RX_Receiving) && (!!NIRQ_RDPIN() == 0))
    {
        Si4438_Receive_Start(Wireless_Channel[0]); //Restart Receive
    }
    if (WIRELESS_ERROR_CODE == Wireless_Busy) //Channel Busy
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

void MacFrame_Process(uint8_t *p_source, uint8_t *p_buf)
{
    uint16_t crc16_val;

    if (p_source[1] == MACWrite_Cmd_Request)
    {
        if (memcmp(Local_MAC_Addr, p_source, MAC_Data_Len + 5) != 0) //只能烧一次
        {
			STMFLASH_Write(MAC_EAddr, (uint16_t *)p_source, (MAC_Data_Len + 5+1)/2);
            STMFLASH_Read(MAC_EAddr, (uint16_t *)Local_MAC_Addr, (MAC_Data_Len + 5+1)/2);
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


void MacAddr_Read(void)
{
    uint32_t delay_cnt = 0;
    do
    {
        SysDelay_Xms(20);

        UartRx_Process(&UpCom_RxBuf, &Device_ParaBuf);
        Eeprom_Read(MAC_EAddr, (uint16_t *)Local_MAC_Addr, MAC_Data_Len + 5);
        delay_cnt++;
        if (delay_cnt > 50)
        {
            delay_cnt = 0;
            //DEBUG_Printf("Please input Mac Frame\r\n");
            UartSendData(USART1, 0x0C);
        }
    } while (0 != FrameData_Detect(Local_MAC_Addr, MAC_Data_Len + 5));
	
}

void Aes_Key_Read(void)
{
    Eeprom_Read(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2);
    Secret_GKey_Flag = 1;
    memset(&aes_out[RsaByte_Size * 2], 0, 19); 
}

void LANGroup_AddrRead(void)
{
    uint8_t temp[4] = {0};
    Eeprom_Read(GD_ADDR, (uint16_t *)temp, 4 / 2);
    memcpy(LANGroup_Addr, temp, 3);
}

//密文初始化
void AES_Init(void)
{
    //计算出密文，存放在aes_w，供加解密用
    memcpy(&aes_out[3 * RsaByte_Size], LANGroup_Addr, 3);

    Rsa_Decode(aes_out);
    key_expansion(aes_out, aes_w);
    Secret_KeyOk_Flag = 1;
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
    uint8_t len;
	uint8_t mac[8]={0x03,0x00,0x01,0x02,0x03,0x04,0x05,0x06};
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
    //PowerOn_Led();
    MacAddr_Read();
    Aes_Key_Read();
	STMFLASH_Read(DEVICE_INFO_BASH_ADDR,(uint16_t *)&deviceInfo,(sizeof(deviceInfo)+1)/2);   //读出设备信息
	memcpy(deviceInfo.mac,Local_MAC_Addr +3,8);
	SecretKey_Process(&deviceInfo);          //计算出密文，存放在aes_w，供加解密用

    Get_WireLessChannel(Wireless_Channel);
    Wireless_Init();
    Si4438_Receive_Start(Wireless_Channel[0]);

    /* USER CODE END 2 */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */

    /* USER CODE BEGIN RTOS_THREADS */
    AppTaskCreate();
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    //  	I2C_Start();
    //	I2C_Write1Byte(SN3218_ADDR);
    //	I2C_Write1Byte(0x01);
    //	I2C_Write1Byte(0x02);
    //	I2C_Write1Byte(64);
    //	I2C_Stop();
    //
    //	Write_SN3218(Addr_DataRefresh, 0x55);
    //	delay_ms(1000);
    //	SN3218_Led_Clear1();
    while (1)
    {

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
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
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    /**Configure the Systick 
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
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
    htim1.Init.Prescaler = 72 - 1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000 - 1;
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
    HAL_GPIO_WritePin(GPIOA, LEDG_Pin | LEDR_Pin | SI4438_SDN_Pin | SI4438_NSS_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED_SN3218A_SDB_Pin | LED_SN3218A_SDA_Pin | LED_SN3218A_SCL_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : TOUCH2_Pin TOUCH4_Pin TOUCH1_Pin */
    GPIO_InitStruct.Pin = TOUCH2_Pin | TOUCH4_Pin | TOUCH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : LEDG_Pin LEDR_Pin SI4438_SDN_Pin SI4438_NSS_Pin */
    GPIO_InitStruct.Pin = LEDG_Pin | LEDR_Pin | SI4438_SDN_Pin | SI4438_NSS_Pin;
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
    GPIO_InitStruct.Pin = LED_SN3218A_SDB_Pin | LED_SN3218A_SDA_Pin | LED_SN3218A_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
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

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
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
