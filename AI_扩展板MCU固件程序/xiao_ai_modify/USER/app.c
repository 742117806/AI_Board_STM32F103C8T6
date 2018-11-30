
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
    if(Wireless_Init() == Wireless_InitError)
    {

    }
    Get_WireLessChannel(Wireless_Channel);
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
    printf("\ndelay_cnt = %d",delay_cnt);   //一般耗时30ms
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
        xSemaphoreTake(xSemWireless,1000);			//获取无线资源占用互斥信号量
        Si4438_Receive_Start(Wireless_Channel[0]); //重新开始接收无线数据
        xSemaphoreGive(xSemWireless);			//释放无线资源占用互斥信号量
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
void vFrameDeviceNetReorganize(QUEUE_WIRELESS_SEND_t *queueMsg)
{
	DebugPrintf("\n设备配网");
	xQueueSend(xQueueWirelessTx, queueMsg, (TickType_t)10);			//直接发到无线发射任务	
	vTaskDelay(300);
	xQueueSend(xQueueNetTask,queueMsg, (TickType_t)10);			//发到配网任务	
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
	while(reSendCnt<3)
	{
		DebugPrintf("\n设备控制");
		xQueueSend(xQueueWirelessTx, queueMsg, (TickType_t)10);			//直接发到无线发射任务	
		reSendCnt ++;
		vTaskDelay(300);
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


	FrameCmdLocalAck(buff,len,0,0);         							//应答
	
	queueMsg.len = len;
	queueMsg.toCh = Wireless_Channel[0];
	memcpy(queueMsg.msg,buff,len);
	
	//加密
	if((buff[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
	{
	     DebugPrintf("\n需要加密");
	     Encrypt_Convert(queueMsg.msg,len , &queueMsg.len, 1);   //加密
	}
	else
	{
	    DebugPrintf("\n不用加密");
	}
	
	
	if(memcmp(&buff[Region_DataIDNumber],CMD_INIT[0],3)==0)  	//设备配网
	{
        vFrameDeviceNetReorganize(&queueMsg);
	}
	else														//设备控制
	{
        vFrameDeviceCtrlReorganize(&queueMsg);
	}	
}





/**
*********************************************************************************************************
*  函 数 名: vDevice
*  功能说明: 设备配网
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/


