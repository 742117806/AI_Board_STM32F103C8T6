
#include "includes.h"

/********************邮箱相关定义和说明***************************/
QueueHandle_t xQueueWirelessTx = NULL;		//无线任务邮箱句柄
QueueHandle_t xQueueLedTask = NULL;		//LED任务邮箱句柄
QueueHandle_t xQueueNetTask = NULL;		//NET任务邮箱句柄


/********************互斥信号量定义*******************************/
SemaphoreHandle_t xSemWireless = NULL;


/*
*********************************************************************************************************
*  函 数 名: vTouchProcess
*  功能说明: 按键按下发送事件命令
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void KeyUpReportCmd(uint8_t keyVal, uint8_t longFlag)
{
    uint8_t temp[50]= {0x00};
    uint8_t frameLen = 0;
    temp[Region_CmdNumber] |= FCMD_DIR_BIT|FCMD_EVNT_BIT;	//帧传输方向1从站 ,事件帧
    temp[Region_DataLenNumber] = 0x05;					//帧数据长度
    memset(&temp[Region_AddrNumber],0xFF,4);
    temp[Region_SeqNumber] |=  (frameNume&0x0f);   		//帧序号
    frameNume ++;
    temp[Region_DataAFNNumber] = keyVal;    			//按键键值
    memcpy(&temp[Region_DataIDNumber],CMD_STATUE[3],3); //数据标识
    temp[Region_DataValNumber] =  longFlag;   	 		//长按标志
    frameLen = Frame_Compose(temp);
    UseComSendBytes(temp,frameLen);
}

/*
*********************************************************************************************************
*  函 数 名: vTouchRelease
*  功能说明: 按键松手后执行流程
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vTouchRelease(eKEY_VALUE key)
{

    switch(key)
    {
    case KEY_NONE:		//没有按键
        //DebugPrintf("\nKEY_NONE ");
        break;
    case KEY1_PRES_S:		//短按
        //DebugPrintf("\nKEY1_PRES_S 释放");
        LedDispKey1(LED_COLOR_ALL,OFF);
        break;
    case KEY1_PRES_L:		//长按
        //DebugPrintf("\nKEY1_PRES_L 释放");
        LedDispKey1(LED_COLOR_ALL,OFF);
        break;
    case KEY2_PRES_S:		//短按
        //DebugPrintf("\nKEY2_PRES_S 释放");
        break;
    case KEY2_PRES_L:		//长按
        break;
    case KEY3_PRES_S:		//短按
        //DebugPrintf("\nKEY3_PRES_S 释放");
        LedDispKey3(LED_COLOR_ORANG,OFF);
        break;
    case KEY3_PRES_L:		//长按
        break;
    case KEY4_PRES_S:		//短按
        //DebugPrintf("\nKEY4_PRES_S 释放");
        break;
    case KEY4_PRES_L:		//长按

        break;
    default:
        break;
    }
}


/*
*********************************************************************************************************
*  函 数 名: vTouchProcess
*  功能说明: 按键执行流程
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vTouchProcess(eKEY_VALUE key_now)
{

    static   eKEY_VALUE key_last  = KEY_NONE;   //上次按键



    switch(key_now)
    {
    case KEY_NONE:		//按键已经按下
        //DebugPrintf("\n按键已经按下");
        break;
    case KEY1_PRES_S:		//短按
        //DebugPrintf("\nKEY1_PRES_S");
        KeyUpReportCmd(0x01,0);
		SN3218_LedStaSet(0,OFF);		//所有LED灭
        LedDispKey1(LED_COLOR_ORANG,ON);
        key_last = key_now;
       
        break;
    case KEY1_PRES_L:		//长按
        //DebugPrintf("\nKEY1_PRES_L");
        KeyUpReportCmd(0x01,0x01);
        key_last = key_now;
        break;
    case KEY2_PRES_S:		//短按
        KeyUpReportCmd(0x02,0);
        key_last = key_now;
		led_delay  = 0;
        break;
    case KEY2_PRES_L:		//长按
        key_last = key_now;
        break;
    case KEY3_PRES_S:		//短按
        KeyUpReportCmd(0x04,0);
		SN3218_LedStaSet(0,OFF);		//所有LED灭
        LedDispKey3(LED_COLOR_ORANG,ON);
        key_last = key_now;
        
        break;
    case KEY3_PRES_L:		//长按
        key_last = key_now;
        break;
    case KEY4_PRES_S:		//短按
        KeyUpReportCmd(0x08,0);
        key_last = key_now;
		led_delay  = 0;
        break;
    case KEY4_PRES_L:		//长按
        key_last = key_now;
        break;
    case KEY_ALL_REL:		//所有按键都释放了
        //DebugPrintf("\n所有按键都释放了");
        vTouchRelease(key_last);     //执行按键释放后
        key_last = KEY_ALL_REL;
        break;
    }

}

/**
*********************************************************************************************************
*  函 数 名: vLedProcess
*  功能说明: LED执行流程
*  形    参: @dat 控制LED模式的数据
*  返 回 值: 无
*********************************************************************************************************
*/
void vLedAroundDeal(eLedColor color)
{
	switch(color)
	{
	case INVALID:
		break;
	case BLUE:
		LED_AroundStaSet(LED_COLOR_BLUE,ON);
		break;
    case RED_OR_ORANGE:
		LED_AroundStaSet(LED_COLOR_ORANG,ON);
		break;
		
    case GREEN_OR_PURPLE:
		LED_AroundStaSet(LED_COLOR_ALL,ON);
		break;
    case WHITE:
	
		break;
	}
		
}

/**
*********************************************************************************************************
*  函 数 名: uxLecColorConvert
*  功能说明: 对应协议定义的颜色和LED驱动定义的颜色
*  形    参: @color 协议定义传来的颜色
*  返 回 值: 无
*********************************************************************************************************
*/
LED_Color_t uxLecColorConvert(eLedColor color)
{
	LED_Color_t led_color = LED_COLOR_ALL;
	switch(color)
	{
	case INVALID:
		break;
	case BLUE:
		led_color = LED_COLOR_BLUE;
		break;
    case RED_OR_ORANGE:
		led_color = LED_COLOR_ORANG;
		break;
		
    case GREEN_OR_PURPLE:
		led_color =  LED_COLOR_ALL;
		break;
    case WHITE:
	    led_color =  LED_COLOR_ALL;
		break;
	}
	return led_color;
}


/**
*********************************************************************************************************
*  函 数 名: vLedProcess
*  功能说明: LED执行流程
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vLedProcess(void)
{
	BaseType_t xResult;
    uint8_t Msg;
	
    LedFunc_t *ledFunc ;
    uint8_t color;
    uint8_t vol;
	uint8_t mode;

	
	
	xResult = xQueueReceive(xQueueLedTask,    		/* 消息队列句柄 */
								&Msg, 				/* 存储接收到的数据到变量queueMsg */
								(TickType_t)5);     /* 设置阻塞时间5个tick */
	if (xResult == pdPASS) 							/* 成功接收，并通过串口将数据打印出来 */
	{
		ledFunc = (LedFunc_t*)&Msg;
		color = ledFunc->ledColor;
		vol = ledFunc->ledColor;
		mode = ledFunc->cmdType;
		led_set_mode = mode;
		led_set_color = uxLecColorConvert((eLedColor)color);
		led_delay  = 0;
		switch(mode)
		{
		case AROUND_OFF:    //外围灯熄灭
			LED_AroundStaSet(LED_COLOR_ALL,OFF);
			break;
		case AROUND_BREATH: //外围灯呼吸模式
			LED_AroundStaSet(LED_COLOR_ALL,OFF);
			break;
		case AROUND_ON:     //外围灯点亮
			vLedAroundDeal((eLedColor)color);
			break;
		case AROUND_FLOW:   //外围灯流水模式
			LED_AroundStaSet(LED_COLOR_ALL,OFF);
			break;
		case CENTRE_MODE:   //中间灯模式
			LED_CenterStaSet(ON);
			break;
		case AROUND_INDEX:  //外围灯按个点亮
			LedDispVol(vol);
			break;
		default:
			break;
		}
	}

}


/**
*********************************************************************************************************
*  函 数 名: vLedCtrlModeLoop
*  功能说明: LED循环执行 （主要控制流水灯和呼吸灯）
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vLedCtrlModeLoop(uint8_t mode)
{
	if(mode == AROUND_BREATH)		//外围灯呼吸模式
	{
		LED_AroundBreath(led_set_color,2);
		 
	}
	else if(mode == AROUND_FLOW) //外围灯流水模式
	{
	   LED_AroundFlow(led_set_color,2);
	}
	else
	{
		led_delay ++;
		if(led_delay > 50)		//3秒
		{
			led_delay = 0;
			LED_AroundStaSet(LED_COLOR_ALL,OFF); 
		}
	}
}


/*
*********************************************************************************************************
*  函 数 名: vAppWirelessInit
*  功能说明: 无线模块初始化
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vAppWirelessInit(void)
{
	Get_WireLessChannel(Wireless_Channel);
    if(Wireless_Init() == Wireless_InitError)
    {

    }
    
    vExtiInit();
    Si4438_Receive_Start(Wireless_Channel[0]);
}

/**
*********************************************************************************************************
*  函 数 名: vWirelessSendBytes
*  功能说明: 无线数据发送
*  形    参: @ch 发送通道
	         @buff 发送数据
			 @len 数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void vWirelessSendBytes(uint8_t ch, uint8_t *buff, uint8_t len)
{
    uint8_t delay_cnt = 0;

    xSemaphoreTake(xSemWireless,1000);			//获取无线资源占用互斥信号量

    Si4438_Transmit_Start(&Wireless_Buf, ch, buff, len);
    while(WIRELESS_STATUS != Wireless_TX_Finish)
    {
        delay_cnt ++;
        if(delay_cnt > 50)break;
        vTaskDelay(2);
    };

    xSemaphoreGive(xSemWireless);			//释放无线资源占用互斥信号量
    DebugPrintf("\ndelay_cnt = %d",delay_cnt);   //一般耗时30ms
    Si4438_Receive_Start(Wireless_Channel[0]);
}




/**
*********************************************************************************************************
*  函 数 名: vWirelessRecvProcess
*  功能说明: 无线接收后处理
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void vWirelessRecvProcess(void)
{
    if(WIRELESS_STATUS == Wireless_RX_Finish)
    {

        //DebugSendBytes(Wireless_Buf.Wireless_RxData,Wireless_Buf.Wireless_PacketLength);
		vWirelessFrameDeal(&Wireless_Buf);
        xSemaphoreTake(xSemWireless,1000);			//获取无线资源占用互斥信号量
        Si4438_Receive_Start(Wireless_Channel[0]); //重新开始接收无线数据
        xSemaphoreGive(xSemWireless);			//释放无线资源占用互斥信号量
    }

}

/**
*********************************************************************************************************
*  函 数 名: vFrameEncrypt
*  功能说明: 加密一帧协议数据
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void vFrameEncrypt(uint8_t *buff,uint8_t len,QUEUE_WIRELESS_SEND_t *queueMsg)
{

	queueMsg->len = len;
	queueMsg->toCh = Wireless_Channel[0];
	memcpy(queueMsg->msg,buff,len);
	
	//加密
	if((buff[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
	{
	     DebugPrintf("\n需要加密");
	     Encrypt_Convert(queueMsg->msg,len , &queueMsg->len, 1);   //加密
	}
	else
	{
	    DebugPrintf("\n不用加密");
	}
}
/**
*********************************************************************************************************
*  函 数 名: v
*  功能说明: 设备配网帧整理
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void vFrameDeviceNetReorganize(uint8_t *buff,uint8_t len,QUEUE_WIRELESS_SEND_t *queueMsg)
{
	uint8_t *mac = &buff[Region_DataValNumber];
	
	
	queueMsg->len = FrameRouterCompose_ext(mac, //配网设备的MAC
						  buff,                                        //配网命令数据
						  len,                                          //配网命令长度
						  queueMsg->msg,                                 //缓存配网命令的邮箱
						  0,                                            //路由表
						  0);                                            //路由表长度
					
	queueMsg->toCh = Default_Channel;
	
	
	if(uxIsLowPowerDevice(*(mac+7)) == 1)				//低功耗设备
	{
		DebugPrintf("\n低功耗设备配网");	
		LowPowerDeviceWakeUp(Default_Channel);		//唤醒低功耗无线设备
		
	}
	else   //普通设备
	{
		DebugPrintf("\n普通设备配网");
	    
	}
	
	
	xQueueSend(xQueueWirelessTx, queueMsg, (TickType_t)10);			//直接发到无线发射任务	
	//vTaskDelay(300);												//等待设备配网成功应答
	//xQueueSend(xQueueNetTask,queueMsg, (TickType_t)10);			//发到配网任务	
}

/**
*********************************************************************************************************
*  函 数 名: v
*  功能说明: 设备控制帧整理
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void vFrameDeviceCtrlReorganize(QUEUE_WIRELESS_SEND_t *queueMsg)
{
	uint8_t reSendCnt = 0;
	uint32_t value;
	BaseType_t xResult; 
	wait_frameNum = queueMsg->msg[Region_SeqNumber] & 0x0f;     //等待的帧号
	while(reSendCnt<1)
	{
		DebugPrintf("\n设备控制");	
		xQueueSend(xQueueWirelessTx, queueMsg, (TickType_t)10);			//直接发到无线发射任务	
		xResult = xTaskNotifyWait(0x00000000,0x00000001,&value,400);
		if(xResult == pdPASS)
		{
			if(value & 0x00000001)
			{
				break;
			}
		}
		reSendCnt ++;
	}
	if(reSendCnt == 1)
	{
		DebugPrintf("\n旧协议控制超时");
		reSendCnt = 0;
		while(reSendCnt<1)
		{
			DebugPrintf("\n新协议控制");
			queueMsg->len = FrameRouterCompose(queueMsg->msg[Region_AddrNumber], //目的设备地址
					  queueMsg->msg,                                        //配网命令数据
					  queueMsg->len,                                          //配网命令长度
					  queueMsg->msg,                                 //缓存配网命令的邮箱
					  0,                                            //路由表
					  0);      
			xQueueSend(xQueueWirelessTx, queueMsg, (TickType_t)10);			//直接发到无线发射任务	
			xResult = xTaskNotifyWait(0x00000000,0x00000001,&value,400);			
			if(xResult == pdPASS)
			{
				if(value & 0x00000001)
				{
					break;
				}
			}
			reSendCnt++;
		}
	}
}



/**
*********************************************************************************************************
*  函 数 名: vWirelessRecvProcess
*  功能说明: 串口远程控制命令帧处理
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void vFrameUartRemoteCmdDeal(uint8_t *buff,uint8_t len)
{
	QUEUE_WIRELESS_SEND_t queueMsg;
	uint8_t out_len = 0;
	uint8_t reSendCnt = 0;
	uint32_t value;
	BaseType_t xResult; 

	FrameCmdLocalAck(buff,len,0,0);         							//应答
    queueMsg.len = len;
	memcpy(queueMsg.msg,buff,len);
	
	//是否加密
	if((buff[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
	{
	     DebugPrintf("\n需要加密");
	     Encrypt_Convert(queueMsg.msg,queueMsg.len , &queueMsg.len, 1);   //加密
	}
	else
	{
	    DebugPrintf("\n不用加密");
	}
	
	
	if(memcmp(&buff[Region_DataIDNumber],CMD_INIT[0],3)==0)  	//设备配网
	{
		if(uxIsLowPowerDevice(buff[Region_DataValNumber+7]) == 1)				//判断设备的MAC最后1位低功耗设备 
		{
			DebugPrintf("\n低功耗设备配网");	
			LowPowerDeviceWakeUp(Default_Channel);		//唤醒低功耗无线设备
		}
		else   //普通设备
		{
			DebugPrintf("\n普通设备配网");
			
		}
	
		DebugPrintf("\n设备配网");
//      vFrameDeviceNetReorganize(buff,len,&queueMsg);
//		vTaskDelay(300);												//等待设备配网成功应答	
//		xQueueSend(xQueueNetTask,&queueMsg, (TickType_t)10);			//发到配网任务
		wait_frameNum = queueMsg.msg[Region_SeqNumber] & 0x0f;     //等待的帧号
		reSendCnt = 0;
		while(reSendCnt<1)
		{
		    queueMsg.toCh = Default_Channel;
			xQueueSend(xQueueNetTask,&queueMsg, (TickType_t)10);			//发到无线配网任务
			xResult = xTaskNotifyWait(0x00000000,0x00000002,&value,400);		//等待配网回应

			if(xResult == pdPASS)                                         //接收到应答
			{
				if(value & 0x00000002)
				{
					break;
				}
			}
			reSendCnt ++;
		}
		if(reSendCnt == 1)
		{
			DebugPrintf("\n旧协议配网超时");
			reSendCnt = 0;
			while(reSendCnt<1)
			{
				queueMsg.len = FrameRouterCompose_ext(&buff[Region_DataValNumber], //配网设备的MAC
				  buff,                                        //配网命令数据
				  len,                                          //配网命令长度
				  queueMsg.msg,                                 //缓存配网命令的邮箱
				  0,                                            //路由表
				  0);                                            //路由表长度
				queueMsg.toCh = Default_Channel;
				DebugPrintf("\n路由协议开始配网");
				xQueueSend(xQueueNetTask,&queueMsg, (TickType_t)10);			//发到无线配网任务
				xResult = xTaskNotifyWait(0x00000000,0x00000002,&value,400);		//等待配网回应
				if(xResult == pdPASS)                                         //接收到应答
				{
					if(value & 0x00000002)
					{   
						DebugPrintf("\n路由协议配网成功");
						break;
					}
				}
				reSendCnt ++;
			}
		}
	}
	else														//设备控制
	{   
		queueMsg.toCh = Wireless_Channel[0];
        vFrameDeviceCtrlReorganize(&queueMsg);
	}	
}

/**
*********************************************************************************************************
*  函 数 名: LowPowerDeviceInit
*  功能说明: 初始化低功耗设备保存的地址空间
*  形    参: @macth 已经配网的设备的逻辑地址
*  返 回 值: 无
*********************************************************************************************************
*/
void LowPowerDeviceInit(void)
{       
	uint8_t i,j;
	//Device_Match_t sleep_device;     //低功耗设备
	STMFLASH_Read(DEVICE_SEELP_ADDR,(uint16_t*)&sleep_device,sizeof(Device_Match_t)/2);
	
	for(i=0;i<DEVICE_NUM_MAX;i++)
	{
		for(j=0;j < deviceInfo.match.deviceNum;j++)
		{		
			if(sleep_device.deviceBuff[i] == deviceInfo.match.deviceBuff[j])
			{
				break;							
			}
		}
		if(j == deviceInfo.match.deviceNum)   
		{
			sleep_device.deviceBuff[i]=0;		//初始化为0
		}
	}
	
	STMFLASH_Write(DEVICE_SEELP_ADDR,(uint16_t*)&sleep_device,sizeof(Device_Match_t)/2);
}
/**
*********************************************************************************************************
*  函 数 名: LowPowerDeviceWakeUp
*  功能说明: 低功耗设备唤醒
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void LowPowerDeviceWakeUp(uint8_t ch)
{
	uint8_t i = 0;
	for(i=0;i<15;i++)
	{
		Si4438_Transmit_Start(&Wireless_Buf,ch,(uint8_t*)"1", 1);
		delay_ms(25);
	}
}

//检测低功耗设备是否已经存在了
uint8_t LowPowerDeviceIsExsit(uint8_t addr)
{
	uint8_t i=0;
	for(i=0;i<DEVICE_NUM_MAX;i++)
	{
		if(sleep_device.deviceBuff[i] == addr)
		{
			return 1;
		}			
	}
	return 0;
}
uint8_t  LowPowerDeviceInset(uint8_t mac_bit7,uint8_t addr)
{
	uint8_t i=0;
	uint8_t lowPowerflag = 0;
	
	lowPowerflag = mac_bit7&0x30;
	lowPowerflag >>= 4;
	
	if(lowPowerflag == 1)
	{
		if(LowPowerDeviceIsExsit(addr) == 1)return 1;
		for(i=0;i<DEVICE_NUM_MAX;i++)
		{
			if(sleep_device.deviceBuff[i] == 0)
			{
				sleep_device.deviceBuff[i] =  addr;
				STMFLASH_Write(DEVICE_SEELP_ADDR,(uint16_t*)&sleep_device,sizeof(Device_Match_t)/2);
				return 1;
			}
		}
	}
	return 0;
}

void LowPowerDeviceDelete(uint8_t addr)
{
	uint8_t i=0;
	for(i=0;i<DEVICE_NUM_MAX;i++)
	{
		if(sleep_device.deviceBuff[i] == addr)
		{
			sleep_device.deviceBuff[i] =  0;
			STMFLASH_Write(DEVICE_SEELP_ADDR,(uint16_t*)&sleep_device,sizeof(Device_Match_t)/2);
			break;
		}
	}
}

uint8_t  LowPowerDeviceMach(uint8_t addr)
{
  	uint8_t i=0;
	for(i=0;i<DEVICE_NUM_MAX;i++)
	{
		if(sleep_device.deviceBuff[i] == addr)
		{	
			return 1;
		}
	}
	return 0;
}





/**
*********************************************************************************************************
*  函 数 名: vDevice
*  功能说明: 设备配网
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/


