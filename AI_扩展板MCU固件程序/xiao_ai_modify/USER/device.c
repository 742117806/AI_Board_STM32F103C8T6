
#include "includes.h"



DeviceInfo_t deviceInfo;
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
			DebugPrintf("\r\n请烧写设备的MAC地址，格式为HEX协议帧 \"68 20 08 xx xx xx xx xx xx xx xx CRC_H CRC_L\"");
		}
		vTaskDelay(10);
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
		
	}
	else   //普通设备
	{
		DebugPrintf("\n普通设备配网");
	    xQueueSend(xQueueWirelessTx, &queueMsg, (TickType_t)10);			//直接发到无线发射任务
	}
	
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
void vVersionReport(uint8_t *cmd,uint8_t len ,uint16_t version)
{
	uint8_t temp[2]={0};
	
	temp[0] = Version_Number >> 8;
	temp[1] = Version_Number & 0xff;
	FrameCmdLocalAck(cmd,len,temp,2);
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
	if(len>225)return ;		//传入的参数列表长度过大
	deviceInfo.match.deviceNum = len;
	
	memcpy(deviceInfo.match.deviceBuff,list,len);
	STMFLASH_Write(DEVICE_MATCH_SAVE_ADDR,(uint16_t*)&deviceInfo.match,sizeof(deviceInfo.match));
}





