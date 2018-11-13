
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
IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
DES_DEVICE_t desDevice[224];   //目标设备路径，224个设备
//uint8_t deviceNum = 0;         //已经配网的设备个数
//uint8_t deviceBuff[224] = {0}; //224个设备

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_IWDG_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t uart1_rec;
uint8_t uart2_rec;

//ROUTER_BROTHER_RSSI_t brother_node[256];

extern osThreadId LedTaskHandle;
extern void MacFrame_Process(uint8_t *p_source);
extern uint8_t SecretKey_Process(DeviceInfo_t *p_deviceInfo);
extern osThreadId UartTaskHandle;
extern QueueHandle_t xQueue1;
extern void DevicePathSaveDes(uint8_t srcAddr, uint8_t *routerTab, uint8_t routerLen);
extern uint8_t FrameRouterCompose(
    uint8_t desAddr,
    uint8_t *srcData,
    uint8_t srcLen,
    uint8_t *outData,
    uint8_t *routerTab,
    uint8_t routerLen);

//根据家庭组切换到新的固定通讯频道上
#ifdef Use_Rx_Hop
void Get_WireLessChannel(uint8_t *wire_chnel)
{
  uint32_t temp_val = LANGroup_Addr[0] + LANGroup_Addr[1] + LANGroup_Addr[2];

  //uint32_t temp_val = 0x00 + 0x29 + 0x02;
  //uint32_t temp_val = 0x00 + 0x2A + 0x5B;
  //temp_val = 0x00 + 0x2A + 0x50;
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
void FrameCmdLocalAck(FRAME_CMD_t *frameCmd, uint8_t *ack_content, uint8_t content_len)
{
  FRAME_CMD_t frameAck;
  uint8_t frameLen;

  memcpy((uint8_t *)&frameAck, (uint8_t *)frameCmd, 16);
  frameAck.addr_DA = 0xFF;
  frameAck.addr_GA[0] = 0xFF;
  frameAck.addr_GA[1] = 0xFF;
  frameAck.addr_GA[2] = 0xFF;
  frameAck.Ctrl.dir = 1;
  if (content_len > 0)
  {
    frameAck.DataLen = content_len + 4;
    memcpy(frameAck.userData.content, ack_content, content_len);
  }
  else
  {
    frameAck.DataLen = 0;
  }

  frameLen = Frame_Compose((uint8_t *)&frameAck);

  UartSendBytes(USART1, (uint8_t *)&frameAck, frameLen);
}
//判断是否符合帧协议
//返回1正确，0错误
uint8_t FrameRouterDetect(uint8_t *rx_buff)
{
  FRAME_ROUTER_CMD_t *frameData = (FRAME_ROUTER_CMD_t *)rx_buff; //通信帧数据结构
  uint16_t crc_16;
  uint8_t crc16_h;
  uint8_t crc16_l;
  //判断帧头
  if ((frameData->head_h != 0x69) || (frameData->head_l != 0x69))
  {
    return 0;
  }
  //FrameData_74Convert()
  //判断长度是否正确
  if ((frameData->len + frameData->len_c) != 0xff)
  {
    return 0;
  }

  crc_16 = CRC16_2(&frameData->head_h, frameData->len);
  crc16_h = (uint8_t)(crc_16 >> 8);
  crc16_l = (uint8_t)(crc_16 & 0x00ff);

  if ((rx_buff[frameData->len] != crc16_h) || (rx_buff[frameData->len + 1] != crc16_l))
  {
    return 0;
  }
  return 1;
}

//判断是否为0xAC开头，0x53结尾的协议帧
uint8_t Frame1Detect(uint8_t *rx_buff,uint8_t rx_len)
{
//	FRAME_CMD_t *frameData = (FRAME_CMD_t *)rx_buff; //通信帧数据结构
//  uint16_t crc_16;
//  uint8_t crc16_h;
//  uint8_t crc16_l;
  //判断帧头
  if (rx_buff[0] != 0xAC)
  {
    return 0;
  }
	//判断帧结束字符
	if(rx_buff[rx_len-1] != 0x53)
	{
	   return 0;
	}
	 return 1;
}

//选择是否通过中继到目标设备
//如果路径有变化，需要保持，返回1
uint8_t RouterPahtSelcte(uint8_t des, uint8_t node, uint8_t rssi)
{
  uint8_t des_index, node_index;

  des_index = des - DEVICE_INDEX_OFFSET; //算出源地址所在的存储位置
  node_index = node - DEVICE_INDEX_OFFSET;
  if ((desDevice[des_index].des_rssi < 50) && (rssi >= 50)) //目标设备到扩展板的信号质量弱，并且目标设备到中继的信号比较强
  {

    if (desDevice[des_index].des_rssi < desDevice[node_index].des_rssi) //两个设备到扩展板的信号值比较
    {
      desDevice[des_index].path1.len = 1;
      desDevice[des_index].path1.addr[0] = node;
      return 1;
    }
  }
  return 0;
}

//判断是否为有效设备
uint8_t DeviceIsExit(uint8_t device_addr)
{
	uint8_t i;
	for(i=0;i<deviceInfo.deviceNum;i++)
	{
		if(device_addr == deviceInfo.deviceBuff[i])
		{
		   return 1;
		}
	}
	return 0;
}

//路由协议帧数据处理，主要针对无线和电力线载波
extern RETRY_WAITE_FOR_t retryWaiteFor;
extern osThreadId RetryTaskHandle; //等待应答重新发起命令的任务
extern osThreadId NetCreateTaskHandle;
extern osThreadId SuperviseTaskHandle;
extern uint8_t const Set_LogicAddr_Id[3];

void FrameRouterDataProcess(uint8_t *rx_buff, uint8_t rx_len)
{

  FRAME_ROUTER_SLAVE_CMD_t *frameData = (FRAME_ROUTER_SLAVE_CMD_t *)rx_buff; //通信帧数据结构
  FRAME_CMD_t *userFrame;                                                    //应用命令帧指针
  ROUTER_TAB_ACK_t routerTabAck;

  uint8_t indexType; // = frameCmd->userData.Index[0];
  uint16_t indexCmd; // = (frameCmd->userData.Index[1] << 8) + frameCmd->userData.Index[2];
  uint8_t srcAddr;
  uint8_t deviceIndex = 0; //设备位置索引

  QUEUE_WIRELESS_SEND_t queue_wireless_send;

	if((frameData->len/2) < 70)
	{
		FrameRouteData_74Convert((FRAME_ROUTER_CMD_t*)rx_buff,rx_len,&rx_len,0);
	}
  
  if (frameData->ctrl.dir == 1) //从站发来的数据
  {
    if (memcmp(frameData->des_addr, &deviceInfo.mac[4], 3) == 0)
    {
      if (frameData->ctrl.heat == 1) //心跳帧
      {
        if (frameData->router_len == frameData->routerNum.index) //当前路由级数==路由总长度，说明是源地址发出来的数据
        {

          deviceIndex = frameData->src_addr - DEVICE_INDEX_OFFSET;            //算出源地址所在的存储位置
          desDevice[deviceIndex].des_rssi = Wireless_Buf.Wireless_RSSI_Value; //缓存目的地址的信号强度值
          if ((desDevice[deviceIndex].des_rssi >= 50)&&(desDevice[deviceIndex].path1.len>0))                          //说明目的设备的到扩展板的信号良好
          {
            desDevice[deviceIndex].path1.len = 0;                                                       //直接去掉路由
            STMFLASH_Write(DEVICE_ROUTER_TAB_ADDR, (uint16_t *)desDevice, (sizeof(desDevice) + 1) / 2); //+1和/2是为了2字节对齐
          }
        }
      }

      if (frameData->routerNum.index == 0)
      {
        if (frameData->ctrl.heat == 0) //通信帧
        {
          userFrame = (FRAME_CMD_t *)&rx_buff[13 + frameData->router_len];
          if ((userFrame->Ctrl.dir == 1) && ((memcmp(userFrame->addr_GA, LANGroup_Addr, 3) == 0))) //从站，是自己的群组
          {
            memcpy(routerTabAck.table, &rx_buff[13], frameData->router_len);
            routerTabAck.len = frameData->router_len;
            srcAddr = frameData->src_addr;

            if (retryWaiteFor.frameNum == userFrame->FSQ.frameNum)
            {
              retryWaiteFor.retryCnt = 0; //清除重复发生缓存的长度，不再重发数据
              retryWaiteFor.flag = 0;
              xTaskNotify(RetryTaskHandle, (1ul << 1), eSetValueWithOverwrite);
            }

            Wireless_Buf.Wireless_PacketLength = rx_len - 17 - frameData->router_len;
            memcpy(rx_buff, userFrame, Wireless_Buf.Wireless_PacketLength);
            userFrame = (FRAME_CMD_t *)rx_buff;

            if (userFrame->FSQ.encryptType > 0) //带有加密数据帧
            {
              Wireless_Buf.Wireless_PacketLength = Encrypt_Convert(Wireless_Buf.Wireless_RxData, Wireless_Buf.Wireless_PacketLength, 1); //做解密给上位机
            }

						if (memcmp(userFrame->addr_GA, LANGroup_Addr, 3) == 0) //判断家庭组地址
						{
							if(memcmp(userFrame->userData.Index,Set_LogicAddr_Id,3)==0) // 配网
							{
								 UpUart_DataTx(&userFrame->FameHead, userFrame->DataLen + 11, 0);
							}
							else
							{
								 if(DeviceIsExit(userFrame->addr_DA) == 1)
								 {
										UpUart_DataTx(&userFrame->FameHead, userFrame->DataLen + 11, 0);
								 }
							}
						}
	
            if (userFrame->Ctrl.eventFlag == 0) //普通帧
            {
              //xTaskNotify(UartTaskHandle, UART_TASK_EVNT_WIRELESS_ACK, eSetValueWithOverwrite); //通知串口任务,发送设备回应的无线数据到核心板串口

              indexType = userFrame->userData.Index[0];
              indexCmd = (userFrame->userData.Index[1] << 8) + userFrame->userData.Index[2];

              switch (indexType)
              {
              case 0xFF:
                switch (indexCmd)
                {
                case 0xFFFF:
                  //printf("111111");
                  DevicePathSaveDes(srcAddr, routerTabAck.table, routerTabAck.len);
                  xTaskNotify(NetCreateTaskHandle, (1ul << 1), eSetValueWithOverwrite);
                  break;
                case 0xFFFE:
                  //printf("222222");
                  break;
                default:
                  break;
                }
                break;
              case 0x00: //智能电器控制回应
                switch (indexCmd)
                {
                case 0x0021:
                  //printf("33333");
                  break;
                default:
                  break;
                }

                break;
              default:
                break;
              }
            }
            else //事件帧
            {
              userFrame->Ctrl.dir = 0;
              userFrame->DataLen = 0;
              if (userFrame->FSQ.encryptType > 0)
              {
                queue_wireless_send.len = Frame_Compose((uint8_t *)userFrame);
                queue_wireless_send.len = Encrypt_Convert(&userFrame->FameHead, queue_wireless_send.len, 0); //加密
              }
							
							
              queue_wireless_send.len = FrameRouterCompose(userFrame->addr_DA,
                                                           &userFrame->FameHead,
                                                           userFrame->DataLen + 11,
                                                           queue_wireless_send.msg,
                                                           routerTabAck.table,
                                                           routerTabAck.len);
																													 
//							FrameRouteData_74Convert((FRAME_ROUTER_CMD_t*)queue_wireless_send.msg,queue_wireless_send.len,&queue_wireless_send.len,1);
              
							queue_wireless_send.toCh = Wireless_Channel[0];

              xQueueSend(xQueueWirelessTask, &queue_wireless_send, (TickType_t)10);
              xQueueSend(xQueueAckRouterTable, &routerTabAck, (TickType_t)10);
            }
          }
        }
        else //心跳帧
        {
          uint8_t des, node, rssi, i;
          uint8_t effect_flag = 0;
          if ((frameData->ctrl.dir == 1) && ((memcmp(frameData->netNum, &LANGroup_Addr[1], 2) == 0))) //从站，是自己的群组
          {
						xTaskNotify(SuperviseTaskHandle, (uint32_t)frameData->src_addr, eSetValueWithOverwrite);
					}
						queue_wireless_send.len = rx_len - 17 - frameData->router_len;

						
						if (queue_wireless_send.len > 0) //存在邻近节点的信号强度值
						{

							node = frameData->src_addr;
							for (i = 0; i < queue_wireless_send.len / 2; i++)
							{
								des = rx_buff[13 + frameData->router_len + (i * 2)];
								rssi = rx_buff[13 + frameData->router_len + (i * 2) + 1];
								effect_flag += RouterPahtSelcte(des, node, rssi);
							}
							if (effect_flag > 0) //保持路由表路径
							{
								STMFLASH_Write(DEVICE_ROUTER_TAB_ADDR, (uint16_t *)desDevice, (sizeof(desDevice) + 1) / 2); //+1和/2是为了2字节对齐
							}
						}
					
        }
      }
    }
  }
  else
  {
  }
}

//0xAC开头的协议帧处理
void Frame1DataProcess(uint8_t *rx_buff, uint8_t rx_len)
{
	uint8_t out_len;
	uint8_t indexType;
	uint16_t indexCmd;
	FRAME_CMD_t *userFrame = (FRAME_CMD_t *)rx_buff;
	QUEUE_WIRELESS_SEND_t queue_wireless_send;
	
	if(userFrame->Ctrl.c_AFN == 0)
	{
		FrameData_74Convert((FRAME_CMD_t*)rx_buff,rx_len,&out_len,0); //解码
		rx_len = out_len;		//解码后长度
	}

	if (userFrame->FSQ.encryptType > 0) //带有加密数据帧
	{
		Wireless_Buf.Wireless_PacketLength = Encrypt_Convert(rx_buff, rx_len, 1); //做解密给上位机
	}
	if (memcmp(userFrame->addr_GA, LANGroup_Addr, 3) == 0) //判断家庭组地址
	{
	
		out_len = Frame_Compose(&userFrame->FameHead);
//		if(DeviceIsExit(userFrame->addr_DA) == 1)
//		{
//			UpUart_DataTx(&userFrame->FameHead, out_len, 0);
//		}
		

			if(memcmp(userFrame->userData.Index,Set_LogicAddr_Id,3)==0) // 配网
			{
				UpUart_DataTx(&userFrame->FameHead, out_len, 0);
			}
			else
			{
				if(DeviceIsExit(userFrame->addr_DA) == 1)
				{
					UpUart_DataTx(&userFrame->FameHead, out_len, 0);
				}
			}

		
		
		if (userFrame->Ctrl.eventFlag == 0) //普通帧
		{
			if (retryWaiteFor.frameNum == userFrame->FSQ.frameNum)
			{
				retryWaiteFor.retryCnt = 0; //清除重复发生缓存的长度，不再重发数据
				retryWaiteFor.flag = 0;
				xTaskNotify(RetryTaskHandle, (1ul << 1), eSetValueWithOverwrite);
			}
						
			indexType = userFrame->userData.Index[0];
			indexCmd = (userFrame->userData.Index[1] << 8) + userFrame->userData.Index[2];
			switch (indexType)
			{
			case 0xFF:
				if( indexCmd == 0xFFFF)
				{
					lodDevice.buff[lodDevice.num] = userFrame->addr_DA;
					lodDevice.num ++;
					STMFLASH_Write(OLD_DEVICE_ADDR, (uint16_t *)&lodDevice, (sizeof(OldDevice_t) + 1) / 2); //+1和/2是为了2字节对齐
					xTaskNotify(NetCreateTaskHandle, (1ul << 1), eSetValueWithOverwrite);
				}
				break;
			default:
				break;
			}
		}
		else   //事件帧
		{
			userFrame->Ctrl.dir = 0;
			userFrame->DataLen = 0;
			if (userFrame->FSQ.encryptType > 0)
			{
				queue_wireless_send.len = Frame_Compose((uint8_t *)userFrame);
				queue_wireless_send.len = Encrypt_Convert(&userFrame->FameHead, queue_wireless_send.len, 0); //加密
			}
			
			memcpy(queue_wireless_send.msg,&userFrame->FameHead,queue_wireless_send.len);
			FrameData_74Convert((FRAME_CMD_t*)queue_wireless_send.msg,queue_wireless_send.len,&queue_wireless_send.len,1);	//编码
//			queue_wireless_send.len = FrameRouterCompose(userFrame->addr_DA,
//																								 &userFrame->FameHead,
//																								 userFrame->DataLen + 11,
//																								 queue_wireless_send.msg,
//																								 routerTabAck.table,
//																								 routerTabAck.len);
			queue_wireless_send.toCh = Wireless_Channel[0];
      
			xQueueSend(xQueueWirelessTask, &queue_wireless_send, (TickType_t)10);
		}		
	}
}

/****************************************************************
功能说明：得到串口的应用层数据帧后，针对控制通信，形成新的一帧数据
参数:
** @desAddr 要控制设备的逻辑地址
** @srcData  应用成数据
** @srcLen   应用数据长度
** @routerTab 要插入的路由表
** @routerLen 插入路由表的长度
返回值：新一帧数据的总长度
****************************************************************/
uint8_t FrameRouterCompose(
    uint8_t desAddr,
    uint8_t *srcData,
    uint8_t srcLen,
    uint8_t *outData,
    uint8_t *routerTab,
    uint8_t routerLen)
{
  //uint8_t temp[256] = {0};
  FRAME_ROUTER_MASTER_CMD_t *p = (FRAME_ROUTER_MASTER_CMD_t *)outData;

  uint16_t crc_16;
  uint8_t out_frameLen;

  out_frameLen = srcLen + routerLen + 13;
  memset(outData, 0x00, out_frameLen + 4);

  p->head_h = 0x69;
  p->head_l = 0x69;

  p->len = srcLen + routerLen + 13;
  p->len_c = ~(p->len);
  p->ctrl.mode = 0;
  p->ctrl.type = 1;
  p->netNum[0] = deviceInfo.addr_GA[1];
  p->netNum[1] = deviceInfo.addr_GA[2];
  p->des_addr = desAddr;
  memcpy(p->src_addr, &deviceInfo.mac[4], 3); //扩展板的地址
  p->routerNum.index = routerLen;             //路由当前级数 等于长度
  p->routerNum.type = 0xF;
  p->router_len = routerLen;                      //路由表长度
  memcpy(&outData[13], routerTab, p->router_len); //路由表数据
  memcpy(&outData[13 + p->router_len], srcData, srcLen);
  crc_16 = CRC16_2(outData, p->len);
  outData[p->len] = crc_16 >> 8;
  outData[p->len + 1] = crc_16 & 0x00ff;
  outData[p->len + 2] = 0x96;
  outData[p->len + 3] = 0x96;
  //memcpy(srcData, outData, p->len + 4);
  return p->len + 4;
}

/****************************************************************
功能说明：得到串口的应用层数据帧后，针对控制通信，形成新的一帧数据
参数:
** @desAddr 要控制设备的逻辑地址
** @srcData  应用成数据
** @srcLen   应用数据长度
** @routerTab 要插入的路由表
** @routerLen 插入路由表的长度
返回值：新一帧数据的总长度
****************************************************************/
uint8_t FrameRouterHeart(
    uint8_t desAddr,
    uint8_t *srcData,
    uint8_t srcLen,
    uint8_t *outData,
    uint8_t *routerTab,
    uint8_t routerLen)
{

  FRAME_ROUTER_MASTER_CMD_t *p = (FRAME_ROUTER_MASTER_CMD_t *)outData;

  uint16_t crc_16;
  uint8_t out_frameLen;

  out_frameLen = srcLen + routerLen + 13;
  memset(outData, 0x00, out_frameLen + 4);

  p->head_h = 0x69;
  p->head_l = 0x69;

  p->len = out_frameLen;
  p->len_c = ~(p->len);

  p->ctrl.mode = 0;
  p->ctrl.type = 1;
  p->ctrl.heat = 1;
  p->netNum[0] = deviceInfo.addr_GA[1];
  p->netNum[1] = deviceInfo.addr_GA[2];
  p->des_addr = desAddr;
  memcpy(p->src_addr, &deviceInfo.mac[4], 3); //扩展板的地址
  p->routerNum.index = routerLen;             //路由当前级数 等于长度
  p->routerNum.type = 0xF;
  p->router_len = routerLen;                      //路由表长度
  memcpy(&outData[13], routerTab, p->router_len); //路由表数据
  memcpy(&outData[13 + p->router_len], srcData, srcLen);
  crc_16 = CRC16_2(outData, p->len);
  outData[p->len] = crc_16 >> 8;
  outData[p->len + 1] = crc_16 & 0x00ff;
  outData[p->len + 2] = 0x96;
  outData[p->len + 3] = 0x96;

  return p->len + 4;
}

/****************************************************************
功能说明：得到串口的应用层数据帧后，针对配网，形成新的一帧数据
参数:
** @desAddrMAC 要配网设备的MAC地址
** @srcData  应用成数据
** @srcLen   应用数据长度
** @routerTab 要插入的路由表
** @routerLen 插入路由表的长度
**@outData 输出数据
返回值：新一帧数据的总长度
****************************************************************/
uint8_t FrameRouterCompose_ext(uint8_t *desAddrMAC,
                               uint8_t *srcData,
                               uint8_t srcLen,
                               uint8_t *outData,
                               uint8_t *routerTab,
                               uint8_t routerLen)
{
  uint8_t temp[256] = {0};
  FRAME_ROUTER_MASTER_EXT_CMD_t *p = (FRAME_ROUTER_MASTER_EXT_CMD_t *)temp;
  uint16_t crc_16;

  p->head_h = 0x69;
  p->head_l = 0x69;

  p->len = srcLen + routerLen + 20;
  p->len_c = ~(p->len);
  p->ctrl.mode = 0;
  p->ctrl.type = 0;
  p->netNum[0] = deviceInfo.addr_GA[1];
  p->netNum[1] = deviceInfo.addr_GA[2];
  memcpy(p->des_addr, desAddrMAC, 8);
  memcpy(p->src_addr, &deviceInfo.mac[4], 3); //扩展板的地址

  p->routerNum.index = routerLen; //路由当前级数
  p->routerNum.type = 0xF;
  p->router_len = routerLen;                   //路由表长度
  memcpy(&temp[20], routerTab, p->router_len); //路由表数据
  memcpy(&temp[20 + p->router_len], srcData, srcLen);
  crc_16 = CRC16_2(temp, p->len);
  temp[p->len] = crc_16 >> 8;
  temp[p->len + 1] = crc_16 & 0x00ff;
  temp[p->len + 2] = 0x96;
  temp[p->len + 3] = 0x96;
  memcpy(outData, temp, p->len + 4);
  return p->len + 4;
}

//判断设备是否为旧设备
uint8_t DeviceVsnJudge(uint8_t des_device,uint8_t *device_buff,uint8_t device_num)
{
	uint8_t i = 0;
	for(i=0;i<device_num;i++)
	{
		if(device_buff[i] == des_device)	//说明是旧版本
		{		
			return 1;
		}
	}
	return 0;
}



//清除不在设备列表中的多余设备地址
void OldDeviceRef(OldDevice_t *p_OldDevice,uint8_t *deviceBuff)
{

	uint8_t i,j;

	for(j=0;j<p_OldDevice->num;j++)
	{
		for(i=0;i<deviceInfo.deviceNum;i++)
		{
			if(p_OldDevice->buff[j] == deviceBuff[i])
			{
				break;
			}
		}
		if(i == deviceInfo.deviceNum)
		{
			memcpy(&p_OldDevice->buff[j],&p_OldDevice->buff[j+1],p_OldDevice->num-j);
			p_OldDevice->num--;
		}
	}
}
//本地命令处理
void Local_CmdProcess(FRAME_CMD_t *frameCmd)
{
  uint8_t indexType = frameCmd->userData.Index[0];
  uint16_t indexCmd = (frameCmd->userData.Index[1] << 8) + frameCmd->userData.Index[2];
  WRITE_AES_CMD_t *p_write_cmd = (WRITE_AES_CMD_t *)frameCmd->userData.content;
  uint8_t temp[10] = {0};

  switch (indexType)
  {
  case 0xFF: //初始化数据
    switch (indexCmd)
    {
    case 0x0001: //读软件版本号
      temp[0] = Version_Number >> 8;
      temp[1] = Version_Number & 0xff;
      FrameCmdLocalAck(frameCmd, temp, 2);
      break;
    case 0xFF00: //写群组地址和密钥
      memcpy(deviceInfo.addr_GA, p_write_cmd->addr_GA, 3);
      memcpy(deviceInfo.aes, p_write_cmd->aes, 16);
      STMFLASH_Write(DEVICE_INFO_BASH_ADDR, (uint16_t *)&deviceInfo, (sizeof(deviceInfo) + 1) / 2); //+1和/2是为了2字节对齐
      SecretKey_Process(&deviceInfo);                                                               //计算出密文，存放在aes_w，供加解密用
      FrameCmdLocalAck(frameCmd, 0, 0);
      break;
    case 0xFFFE:
      vTaskSuspendAll(); //开启任务调度锁
      memset(deviceInfo.deviceBuff, 0x00, deviceInfo.deviceNum);
      deviceInfo.deviceNum = frameCmd->DataLen - 4;
      memcpy(deviceInfo.deviceBuff, frameCmd->userData.content, deviceInfo.deviceNum);
			STMFLASH_Write(DEVICE_INFO_BASH_ADDR, (uint16_t *)&deviceInfo, (sizeof(deviceInfo) + 1) / 2);
      xTaskResumeAll(); //关闭任务调度锁
      
			OldDeviceRef(&lodDevice,deviceInfo.deviceBuff);
			
      FrameCmdLocalAck(frameCmd, 0, 0);
      break;
    default:
      FrameCmdLocalAck(frameCmd, 0, 0);
      break;
    }
    break;
  case 0x00:

    break;
  case 0x01:
    switch (indexCmd)
    {
    case 0x0021:
      //xTaskNotify(LedTaskHandle, frameCmd->userData.content[0], eSetValueWithOverwrite);
      xQueueSend(xQueueVol, &frameCmd->userData.content[0], (TickType_t)10);
      FrameCmdLocalAck(frameCmd, 0, 0);
      break;
    default:
      FrameCmdLocalAck(frameCmd, 0, 0);
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
    FrameCmdLocalAck(frameCmd, 0, 0);
    break;
  }
}



extern RETRY_WAITE_FOR_t retryWaiteFor;

//串口接收处理
void UartRx_Process(UpCom_Rx_TypDef *prx_ubuf, DevicePara_TypDef *p_device)
{
  FRAME_CMD_t *frameCmd;
  uint8_t send_len;
  frameCmd = (FRAME_CMD_t *)prx_ubuf->Frame_Data;

  //	BaseType_t xResult;
  //const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000); //10ms
  ///    uint32_t ulValue;
  ///    uint16_t keyValue;
  extern uint8_t const Set_LogicAddr_Id[3];
  extern osThreadId RetryTaskHandle;
  QUEUE_WIRELESS_SEND_t queue_temp;
  uint8_t routerTab[10] = {0x8A};
  uint8_t routerIndex = 0;
  uint8_t routerLen = 0;
  uint8_t i;
  //uint8_t send_temp[256]={0};
	QUEUE_WIRELESS_SEND_t queue_wireless_send;
  //memset(&queue_wireless_send,0x00,sizeof(QUEUE_WIRELESS_SEND_t)) ;
  if (prx_ubuf->Rx_Status == UartRx_Finished)
  {
    if (0 == FrameData_Detect(prx_ubuf->Frame_Data, prx_ubuf->FrameTotalLen))
    {
      if (frameCmd->FameHead == HKFreamHeader)
      {
        if (frameCmd->Ctrl.dir == 0) //主站（安卓版）下行数据
        {
          currentFrameNum = frameCmd->FSQ.frameNum;
          if (frameCmd->Ctrl.relayFlag == 0) //本地（发给扩展板）
          {
            if (frameCmd->Ctrl.eventFlag == 0) //命令
            {
              Local_CmdProcess(frameCmd);
            }
            else if (frameCmd->Ctrl.eventFlag == 1) //事件(应答)
            {
            }
          }
          else if (frameCmd->Ctrl.relayFlag == 1) //转发（发给电器设备）
          {
            if (frameCmd->Ctrl.eventFlag == 0) //命令
            {
              retryWaiteFor.frameNum = frameCmd->FSQ.frameNum;          //记录重发的帧序号
              memcpy(retryWaiteFor.index, frameCmd->userData.Index, 3); //记录重发的帧数据标识
              retryWaiteFor.retryCnt = 3;
              retryWaiteFor.flag = 1;

              FrameCmdLocalAck(frameCmd, 0, 0); //应答给核心板的串口数据
              if (frameCmd->FSQ.encryptType)    //加密
              {
                send_len = Encrypt_Convert((uint8_t *)frameCmd, frameCmd->DataLen + 11, 0);
              }
              else //不加密
              {
                send_len = frameCmd->DataLen + 11;
              }
              prx_ubuf->FrameTotalLen = send_len;
							JOINE_NET_CMD_t *joine_cmd = (JOINE_NET_CMD_t *)frameCmd->userData.content;

              if (memcmp(frameCmd->userData.Index, Set_LogicAddr_Id, 3) == 0) //配网帧
              {
								queue_wireless_send.len = FrameRouterCompose_ext(joine_cmd->mac,
																															 prx_ubuf->Frame_Data,
																															 prx_ubuf->FrameTotalLen,
																															 queue_wireless_send.msg,
																															 0,
																															 0);
								queue_wireless_send.toCh = Default_Channel;
								xQueueSend(xQueueWirelessTask, &queue_wireless_send, (TickType_t)10);			//直接发到无线发射任务
								osDelay(300);
                xQueueSend(xQueueNetCreateTask, prx_ubuf, (TickType_t)10);        //发到配网任务
              }
              else //控制
              {
                routerIndex = frameCmd->addr_DA - DEVICE_INDEX_OFFSET;
                routerLen = desDevice[routerIndex].path1.len;

                for (i = 0; i < routerLen; i++)
                {
                  routerTab[i] = desDevice[routerIndex].path1.addr[i];
                }
								if(DeviceVsnJudge(frameCmd->addr_DA,lodDevice.buff,lodDevice.num) == 1)
								{

									memcpy(queue_temp.msg, &frameCmd->FameHead, send_len);
									FrameData_74Convert((FRAME_CMD_t*)queue_temp.msg,send_len,&send_len,1);
									
                
								}
								else
								{
									send_len = FrameRouterCompose(frameCmd->addr_DA,
															prx_ubuf->Frame_Data,
															prx_ubuf->FrameTotalLen,
															queue_temp.msg,
															routerTab,
															routerLen);
//									FrameRouteData_74Convert((FRAME_ROUTER_CMD_t*)queue_temp.msg,send_len,&send_len,1);
									
								}
								
								queue_temp.toCh = Wireless_Channel[0];
                queue_temp.len = send_len;
								
                xQueueSend(xQueueWirelessRetryTask, &queue_temp, (TickType_t)10);
              }
            }
            else if (frameCmd->Ctrl.eventFlag == 1) //事件(应答)
            {
            }
          }
        }
      }
      else if (frameCmd->FameHead == AESFreamHeader) //烧录MAC地址
      {

        MacFrame_Process(prx_ubuf->Frame_Data);
        memcpy(deviceInfo.mac, Local_MAC_Addr + 3, 8);
        deviceInfo.mac_exist = 1;
        STMFLASH_Write(DEVICE_INFO_BASH_ADDR, (uint16_t *)&deviceInfo, (sizeof(deviceInfo) + 1) / 2);
      }
    }

    prx_ubuf->Rx_Status = UartRx_FrameHead;
  }
}

//配网成功后记录目标路径
void DevicePathSaveDes(uint8_t srcAddr, uint8_t *routerTab, uint8_t routerLen)
{
  uint8_t i = 0;
  uint8_t index = 0;
  if ((srcAddr < DEVICE_INDEX_OFFSET) || (srcAddr == 0xFF))
    return; //检测入口参数

  index = srcAddr - DEVICE_INDEX_OFFSET;
  //	desDevice[index].des_addr =srcAddr;
  desDevice[index].path1.len = routerLen;
  for (i = 0; i < routerLen; i++)
  {
    desDevice[index].path1.addr[i] = routerTab[i];
  }
  STMFLASH_Write(DEVICE_ROUTER_TAB_ADDR, (uint16_t *)desDevice, (sizeof(desDevice) + 1) / 2); //+1和/2是为了2字节对齐
}

//读取路由表
void DegicePathInit(void)
{
  STMFLASH_Read(DEVICE_ROUTER_TAB_ADDR, (uint16_t *)desDevice, (sizeof(desDevice) + 1) / 2); //+1和/2是为了2字节对齐
  //if(desDevice[0].des_addr == 0xFF)
  if ((desDevice[0].des_rssi == 0xFF) && (desDevice[1].des_rssi == 0xFF))
  {
    memset(desDevice, 0x00, sizeof(desDevice));
    STMFLASH_Write(DEVICE_ROUTER_TAB_ADDR, (uint16_t *)desDevice, (sizeof(desDevice) + 1) / 2); //+1和/2是为了2字节对齐
  }
}

//
void WireLess_Process(WLS *p_wl, DevicePara_TypDef *p_device)
{

  if (FrameRouterDetect(p_wl->Wireless_RxData) == 1) //校验帧头和CRC16
  {
		
    FrameRouterDataProcess(p_wl->Wireless_RxData, p_wl->Wireless_PacketLength);
  }
	else if(Frame1Detect(p_wl->Wireless_RxData,p_wl->Wireless_PacketLength))
	{
		Frame1DataProcess(p_wl->Wireless_RxData, p_wl->Wireless_PacketLength);
	}
  else
  {
    Si4438_Receive_Start(Wireless_Channel[0]);
  }
  if (WIRELESS_STATUS == Wireless_Free)
  {
    Si4438_Receive_Start(Wireless_Channel[0]);
  }
}

void System_8msTick_Process(void)
{
//    if ((WIRELESS_STATUS == Wireless_RX_Receiving) && (!!NIRQ_RDPIN() == 0))
//    {
//        Si4438_Receive_Start(Wireless_Channel[0]); //Restart Receive
//    }
//    if (WIRELESS_ERROR_CODE == Wireless_Busy) //Channel Busy
//    {
//        if (Wireless_Buf.Channel_Busy_cnt) // Send Ready
//        {
//            if (++Wireless_Buf.Channel_Busy_cnt > BUSY_TIMEOUT)
//            {
//                Wireless_Buf.Channel_Busy_cnt = 0;

//                //Set_Property(Interrupt_Close);
//                WIRELESS_ERROR_CODE = Wireless_NoError;
//                Si4438_Receive_Start(Wireless_Channel[0]);
//            }
//        }
//        else
//            WIRELESS_ERROR_CODE = Wireless_NoError;
//    }
//    if (WirelessRx_Timeout_Cnt)
//    {
//        if (++WirelessRx_Timeout_Cnt > WirelessRx_Timeout_Threshold)
//        {
//            WirelessRx_Timeout_Cnt = 0;

//            Si4438_Receive_Start(Wireless_Channel[0]);
//        }
//    }
//    if (Wireless_Buf.Sent_TimeOut_Cnt > 0)
//    {
//        if (++Wireless_Buf.Sent_TimeOut_Cnt > WLSent_TimeOut_Val)
//        {
//            Wireless_Buf.Sent_TimeOut_Cnt = 0;
//            WIRELESS_STATUS = Wireless_TX_Finish;
//        }
//    }
//    if (UpCom_TxBuf.Tx_Status == UartTx_End)
//    {
//        if (++UpCom_TxBuf.TxEnd_Cnt > Tx_GapTime_Size)
//        {
//            UpCom_TxBuf.TxEnd_Cnt = 0;
//            UpCom_TxBuf.Tx_Status = UartTx_Finished;
//        }
//    }
//    if (UpCom_RxBuf.Over_time_count)
//    {
//		//printf("UpCom_RxBuf.Over_time_count = %d\r\n",UpCom_RxBuf.Over_time_count);
//        if (++UpCom_RxBuf.Over_time_count > Up_TimeOut_Val)
//        {
//            UpCom_RxBuf.Over_time_count = 0;
//            UpCom_RxBuf.Rx_Status = UartRx_FrameHead;
//			//printf(" UpCom_RxBuf.Over_time_count = 0");
//        }
//    }
#ifdef Use_Rout
  if (Frame_Wait_Cnt)
  {
    Frame_Wait_Cnt++;
  }
#endif
}

void MacFrame_Process(uint8_t *p_source)
{
  uint16_t crc16_val;
  uint8_t send_buff[100];

  if (p_source[1] == MACWrite_Cmd_Request)
  {
    if (memcmp(Local_MAC_Addr, p_source, MAC_Data_Len + 5) != 0) //只能烧一次
    {
      STMFLASH_Write(MAC_EAddr, (uint16_t *)p_source, (MAC_Data_Len + 5 + 1) / 2);
      STMFLASH_Read(MAC_EAddr, (uint16_t *)Local_MAC_Addr, (MAC_Data_Len + 5 + 1) / 2);
      if (memcmp(Local_MAC_Addr, p_source, MAC_Data_Len + 5) == 0)
      {
        send_buff[1] = MACWrite_Yes_Response;
        send_buff[3] = 0xff;
      }
      else
      {
        send_buff[1] = MACWrite_NO_Response;
        send_buff[3] = 0x00;
      }
    }
    else
    {
      send_buff[1] = MACWrite_Yes_Response;
      send_buff[3] = 0xff;
    }
    send_buff[0] = AESFreamHeader;
    send_buff[2] = 1;
    crc16_val = CRC16_2(send_buff, 4);
    send_buff[4] = crc16_val >> 8;
    send_buff[5] = crc16_val & 0xff;

    UpUart_DataTx(send_buff, 6, 0);
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
  //uint8_t len;
  //uint8_t mac[8]={0x03,0x00,0x01,0x02,0x03,0x04,0x05,0x06};
  //uint8_t test_temp[256]={0x69,0x69,0x14,0xEB,0x81,0x2A,0x5B,0x00,0x00,0x14,0x85,0x0F,0x00,0xAC,0x89,0x00,0x05,0x2B,0x05,0x65,0x2E,0xCC,0x96,0x96};
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
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	HAL_IWDG_Refresh(&hiwdg);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_UART_Receive_IT(&huart1, &uart1_rec, 1);
  HAL_UART_Receive_IT(&huart2, &uart2_rec, 1);
  SN3218_Init();
  //PowerOn_Led();
  MacAddr_Read();
  Aes_Key_Read();
	STMFLASH_Read(OLD_DEVICE_ADDR, (uint16_t *)&lodDevice, (sizeof(OldDevice_t) + 1) / 2); //+1和/2是为了2字节对齐
	if(lodDevice.num == 0xFF)
	{
		lodDevice.num = 0;
		STMFLASH_Write(OLD_DEVICE_ADDR, (uint16_t *)&lodDevice, (sizeof(OldDevice_t) + 1) / 2); //+1和/2是为了2字节对齐
	}
  STMFLASH_Read(DEVICE_INFO_BASH_ADDR, (uint16_t *)&deviceInfo, (sizeof(deviceInfo) + 1) / 2); //读出设备信息
  memcpy(deviceInfo.mac, Local_MAC_Addr + 3, 8);
  SecretKey_Process(&deviceInfo); //计算出密文，存放在aes_w，供加解密用
  DegicePathInit();
  Get_WireLessChannel(Wireless_Channel);
  Wireless_Init();
  Si4438_Receive_Start(Wireless_Channel[0]);
	AppTaskCreate();
  //len = FrameRouterCompose(0x85,test_temp,9);
  //UartSendBytes(USART1,test_temp,len);
  //FrameRouterDataProcess(test_temp,24);

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
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */

  
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

  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		//Si4438_Transmit_Start(&Wireless_Buf, Wireless_Channel[0], "wireless send test",strlen("wireless send test") );
		//delay_ms(100);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* IWDG init function */
static void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 4000;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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
  HAL_GPIO_WritePin(GPIOB, SI4438_RX_Pin|SI4438_TX_Pin|LED_SN3218A_SDB_Pin|LED_SN3218A_SDA_Pin 
                          |LED_SN3218A_SCL_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : SI4438_RX_Pin SI4438_TX_Pin */
  GPIO_InitStruct.Pin = SI4438_RX_Pin|SI4438_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

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
