
#include "includes.h"



DeviceInfo_t deviceInfo;

Device_Match_t sleep_device;     //低功耗设备
DeviceMatcheCurrentState_t     DeviceMatcheCurrentState={0,0};
DeviceMatcheCurrentState_t     DeviceCtrlCurrentState={0,0};

/*
*********************************************************************************************************
*  函 数 名: vDeviceInfoInit
*  功能说明: 设备信息初始化+设备MAC地址烧录
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vDeviceInfoInit(void)
{
	uint8_t delay_cnt = 0;
	
    STMFLASH_Read(DEVICE_INFO_SAVE_ADDR,(uint16_t*)&deviceInfo,sizeof(deviceInfo)/2);
	while(deviceInfo.mac_frame.mac_h != AESFreamHeader)
	{
					
		vUartAesProcess(&sUart1Rx);
		delay_cnt ++;
		if(delay_cnt>100)
		{
			delay_cnt = 0;
			//DebugPrintf("\r\n请烧写设备的MAC地址，格式为HEX协议帧 \"68 20 08 xx xx xx xx xx xx xx xx CRC_H CRC_L\"");
			//UseComSendBytes("\r\n请烧写设备的MAC地址，格式为HEX协议帧 \"68 20 08 xx xx xx xx xx xx xx xx CRC_H CRC_L\"",
			//sizeof("\r\n请烧写设备的MAC地址，格式为HEX协议帧 \"68 20 08 xx xx xx xx xx xx xx xx CRC_H CRC_L\""));
			UartSendData(USART1,0x0C);
		}
		delay_ms(10);
	}
}

/*
*********************************************************************************************************
*  函 数 名: uxIsLowPowerDevice
*  功能说明: 判断是否为低功耗设备
*  形    参: 无
*  返 回 值: 1低功耗设备，0非低功耗设备
*********************************************************************************************************
*/
uint8_t uxIsLowPowerDevice(uint8_t mac_byte7)
{
   	uint8_t lowPowerflag = 0;
	
	lowPowerflag = mac_byte7&0x30;
	lowPowerflag >>= 4;
	if(lowPowerflag == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*  函 数 名: vLowPowerDeviceWakeUp
*  功能说明: 低功耗设备发射无线数据唤醒
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vLowPowerDeviceWakeUp(uint8_t ch)
{
	uint8_t i = 0;
	for(i=0;i<20;i++)
	{
		vWirelessSendBytes(ch,(uint8_t*)"123", 3);
		delay_ms(25);
	}
}

/*
*********************************************************************************************************
*  函 数 名: vDeviceMatchNet
*  功能说明: 发送设备配网无线数据
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void  vDeviceMatchNet(uint8_t *buff,uint8_t len)
{
	QUEUE_WIRELESS_SEND_t queueMsg;
	uint8_t *mac = &buff[Region_DataValNumber];
	
	queueMsg.len = FrameRouterCompose_ext(mac, //配网设备的MAC
						  buff,                                        //配网命令数据
						  len,                                          //配网命令长度
						  queueMsg.msg,                                 //缓存配网命令的邮箱
						  0,                                            //路由表
						  0);                                            //路由表长度
	queueMsg.toCh = Default_Channel;
	
	
	if(uxIsLowPowerDevice(*(mac+7)) == 1)				//低功耗设备
	{
		DebugPrintf("\n低功耗设备配网");	
		LowPowerDeviceWakeUp(Default_Channel);		//唤醒低功耗无线设备
	}
	else   //普通设备
	{
		DebugPrintf("\n普通设备配网");
	    
	}
	xQueueSend(xQueueWirelessTx, &queueMsg, (TickType_t)10);			//直接发到无线发射任务
	DebugSendBytes(queueMsg.msg,queueMsg.len);
}

/**
*********************************************************************************************************
*  函 数 名: vVersionReport
*  功能说明: 读取软件版本号
*  形    参: @cmd 接收到的命令数据
			 @len接收到的命令长度
			 @version 要上报的版本号
*  返 回 值: 无
*********************************************************************************************************
*/
void vVersionReport(uint8_t *cmd,uint8_t len ,uint32_t version)
{
	uint8_t temp[3]={0};
	temp[0] = Version_Number >> 16;
	temp[1] = (Version_Number >> 8)& 0x0000ff;
	temp[2] = Version_Number & 0x0000ff;
	FrameCmdLocalAck(cmd,len,temp,3);
}

/**
*********************************************************************************************************
*  函 数 名: vDevicMacReport
*  功能说明: 读取设备MAC地址
*  形    参: @cmd 接收到的命令数据
			 @len接收到的命令长度
			 @mac 要上报的版本号
*  返 回 值: 无
*********************************************************************************************************
*/
void vDeviceMacReport(uint8_t *cmd,uint8_t len ,uint8_t *mac)
{	
	FrameCmdLocalAck(cmd,len,mac,8);
}


/**
*********************************************************************************************************
*  函 数 名: vDeviceListSave
*  功能说明: 保存设备列表
*  形    参: @cmd 接收到的命令数据
			 @len接收到的命令长度
			 @version 要上报的版本号
*  返 回 值: 无
*********************************************************************************************************
*/
void vDeviceListSave(uint8_t *list,uint8_t len)
{	 
	if(len>DEVICE_NUM_MAX)return ;		//传入的参数列表长度过大
	deviceInfo.match.deviceNum = len;
	memset(deviceInfo.match.deviceBuff,0x00,DEVICE_NUM_MAX);
	memcpy(deviceInfo.match.deviceBuff,list,len);
	STMFLASH_Write(DEVICE_MATCH_SAVE_ADDR,(uint16_t*)&deviceInfo.match,sizeof(deviceInfo.match));
}


/**
*********************************************************************************************************
*  函 数 名: vDeviceIsExistList
*  功能说明: 设备是否在设备列表中
*  形    参: @list 设备列表
			 @desAddr目标设备地址
*  返 回 值: 无
*********************************************************************************************************
*/
uint8_t vDeviceIsExistList(Device_Match_t *list,uint8_t desAddr)
{
    uint8_t i;
	for(i = 0; i < list->deviceNum; i++)
	{
		if(desAddr == list->deviceBuff[i])
		{
			return 1;  //存在
		}
	}
	return 0;       //不存在
}



/**
*********************************************************************************************************
*  函 数 名: AES_Init
*  功能说明: 密文初始化
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void AES_Init(void)
{
	//计算出密文，存放在aes_w，供加解密用
	memcpy(&aes_out[2*RsaByte_Size],deviceInfo.aes_field.aes,16);
	memcpy(&aes_out[3*RsaByte_Size],deviceInfo.aes_field.addr_GA,3);
	
	Rsa_Decode(aes_out);  
	key_expansion(aes_out, aes_w);  
}

/**
*********************************************************************************************************
*  函 数 名: CurrentDeviceMatche
*  功能说明: 正在配网设备次数级数
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void  CurrentDeviceMatche(uint8_t addr,DeviceMatcheCurrentState_t *current_device)
{
    if(addr == current_device->addr)
	{
	   current_device->MatchCnt ++;
	}
	else
	{
	   current_device->MatchCnt = 0;
	}	
}






