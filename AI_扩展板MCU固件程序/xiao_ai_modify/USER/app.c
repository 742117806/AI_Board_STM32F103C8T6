
#include "includes.h"

/********************������ض����˵��***************************/
QueueHandle_t xQueueWirelessTx = NULL;		//��������������
QueueHandle_t xQueueLedTask = NULL;		//LED����������
QueueHandle_t xQueueNetTask = NULL;		//NET����������


/********************�����ź�������*******************************/
SemaphoreHandle_t xSemWireless = NULL;


/*
*********************************************************************************************************
*  �� �� ��: vTouchProcess
*  ����˵��: �������·����¼�����
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void KeyUpReportCmd(uint8_t keyVal, uint8_t longFlag)
{
    uint8_t temp[50]= {0x00};
    uint8_t frameLen = 0;
    temp[Region_CmdNumber] |= FCMD_DIR_BIT|FCMD_EVNT_BIT;	//֡���䷽��1��վ ,�¼�֡
    temp[Region_DataLenNumber] = 0x05;					//֡���ݳ���
    memset(&temp[Region_AddrNumber],0xFF,4);
    temp[Region_SeqNumber] |=  (frameNume&0x0f);   		//֡���
    frameNume ++;
    temp[Region_DataAFNNumber] = keyVal;    			//������ֵ
    memcpy(&temp[Region_DataIDNumber],CMD_STATUE[3],3); //���ݱ�ʶ
    temp[Region_DataValNumber] =  longFlag;   	 		//������־
    frameLen = Frame_Compose(temp);
    UseComSendBytes(temp,frameLen);
}

/*
*********************************************************************************************************
*  �� �� ��: vTouchRelease
*  ����˵��: �������ֺ�ִ������
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vTouchRelease(eKEY_VALUE key)
{

    switch(key)
    {
    case KEY_NONE:		//û�а���
        //DebugPrintf("\nKEY_NONE ");
        break;
    case KEY1_PRES_S:		//�̰�
        //DebugPrintf("\nKEY1_PRES_S �ͷ�");
        LedDispKey1(LED_COLOR_ALL,OFF);
        break;
    case KEY1_PRES_L:		//����
        //DebugPrintf("\nKEY1_PRES_L �ͷ�");
        LedDispKey1(LED_COLOR_ALL,OFF);
        break;
    case KEY2_PRES_S:		//�̰�
        //DebugPrintf("\nKEY2_PRES_S �ͷ�");
        break;
    case KEY2_PRES_L:		//����
        break;
    case KEY3_PRES_S:		//�̰�
        //DebugPrintf("\nKEY3_PRES_S �ͷ�");
        LedDispKey3(LED_COLOR_ORANG,OFF);
        break;
    case KEY3_PRES_L:		//����
        break;
    case KEY4_PRES_S:		//�̰�
        //DebugPrintf("\nKEY4_PRES_S �ͷ�");
        break;
    case KEY4_PRES_L:		//����

        break;
    default:
        break;
    }
}


/*
*********************************************************************************************************
*  �� �� ��: vTouchProcess
*  ����˵��: ����ִ������
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vTouchProcess(eKEY_VALUE key_now)
{

    static   eKEY_VALUE key_last  = KEY_NONE;   //�ϴΰ���



    switch(key_now)
    {
    case KEY_NONE:		//�����Ѿ�����
        //DebugPrintf("\n�����Ѿ�����");
        break;
    case KEY1_PRES_S:		//�̰�
        //DebugPrintf("\nKEY1_PRES_S");
        KeyUpReportCmd(0x01,0);
		SN3218_LedStaSet(0,OFF);		//����LED��
        LedDispKey1(LED_COLOR_ORANG,ON);
        key_last = key_now;
       
        break;
    case KEY1_PRES_L:		//����
        //DebugPrintf("\nKEY1_PRES_L");
        KeyUpReportCmd(0x01,0x01);
        key_last = key_now;
        break;
    case KEY2_PRES_S:		//�̰�
        KeyUpReportCmd(0x02,0);
        key_last = key_now;
		led_delay  = 0;
        break;
    case KEY2_PRES_L:		//����
        key_last = key_now;
        break;
    case KEY3_PRES_S:		//�̰�
        KeyUpReportCmd(0x04,0);
		SN3218_LedStaSet(0,OFF);		//����LED��
        LedDispKey3(LED_COLOR_ORANG,ON);
        key_last = key_now;
        
        break;
    case KEY3_PRES_L:		//����
        key_last = key_now;
        break;
    case KEY4_PRES_S:		//�̰�
        KeyUpReportCmd(0x08,0);
        key_last = key_now;
		led_delay  = 0;
        break;
    case KEY4_PRES_L:		//����
        key_last = key_now;
        break;
    case KEY_ALL_REL:		//���а������ͷ���
        //DebugPrintf("\n���а������ͷ���");
        vTouchRelease(key_last);     //ִ�а����ͷź�
        key_last = KEY_ALL_REL;
        break;
    }

}

/**
*********************************************************************************************************
*  �� �� ��: vLedProcess
*  ����˵��: LEDִ������
*  ��    ��: @dat ����LEDģʽ������
*  �� �� ֵ: ��
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
*  �� �� ��: uxLecColorConvert
*  ����˵��: ��ӦЭ�鶨�����ɫ��LED�����������ɫ
*  ��    ��: @color Э�鶨�崫������ɫ
*  �� �� ֵ: ��
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
*  �� �� ��: vLedProcess
*  ����˵��: LEDִ������
*  ��    ��: ��
*  �� �� ֵ: ��
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

	
	
	xResult = xQueueReceive(xQueueLedTask,    		/* ��Ϣ���о�� */
								&Msg, 				/* �洢���յ������ݵ�����queueMsg */
								(TickType_t)5);     /* ��������ʱ��5��tick */
	if (xResult == pdPASS) 							/* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
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
		case AROUND_OFF:    //��Χ��Ϩ��
			LED_AroundStaSet(LED_COLOR_ALL,OFF);
			break;
		case AROUND_BREATH: //��Χ�ƺ���ģʽ
			LED_AroundStaSet(LED_COLOR_ALL,OFF);
			break;
		case AROUND_ON:     //��Χ�Ƶ���
			vLedAroundDeal((eLedColor)color);
			break;
		case AROUND_FLOW:   //��Χ����ˮģʽ
			LED_AroundStaSet(LED_COLOR_ALL,OFF);
			break;
		case CENTRE_MODE:   //�м��ģʽ
			LED_CenterStaSet(ON);
			break;
		case AROUND_INDEX:  //��Χ�ư�������
			LedDispVol(vol);
			break;
		default:
			break;
		}
	}

}


/**
*********************************************************************************************************
*  �� �� ��: vLedCtrlModeLoop
*  ����˵��: LEDѭ��ִ�� ����Ҫ������ˮ�ƺͺ����ƣ�
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vLedCtrlModeLoop(uint8_t mode)
{
	if(mode == AROUND_BREATH)		//��Χ�ƺ���ģʽ
	{
		LED_AroundBreath(led_set_color,2);
		 
	}
	else if(mode == AROUND_FLOW) //��Χ����ˮģʽ
	{
	   LED_AroundFlow(led_set_color,2);
	}
	else
	{
		led_delay ++;
		if(led_delay > 50)		//3��
		{
			led_delay = 0;
			LED_AroundStaSet(LED_COLOR_ALL,OFF); 
		}
	}
}


/*
*********************************************************************************************************
*  �� �� ��: vAppWirelessInit
*  ����˵��: ����ģ���ʼ��
*  ��    ��: ��
*  �� �� ֵ: ��
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
*  �� �� ��: vWirelessSendBytes
*  ����˵��: �������ݷ���
*  ��    ��: @ch ����ͨ��
	         @buff ��������
			 @len ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vWirelessSendBytes(uint8_t ch, uint8_t *buff, uint8_t len)
{
    uint8_t delay_cnt = 0;

    xSemaphoreTake(xSemWireless,1000);			//��ȡ������Դռ�û����ź���

    Si4438_Transmit_Start(&Wireless_Buf, ch, buff, len);
    while(WIRELESS_STATUS != Wireless_TX_Finish)
    {
        delay_cnt ++;
        if(delay_cnt > 50)break;
        vTaskDelay(2);
    };

    xSemaphoreGive(xSemWireless);			//�ͷ�������Դռ�û����ź���
    printf("\ndelay_cnt = %d",delay_cnt);   //һ���ʱ30ms
    Si4438_Receive_Start(Wireless_Channel[0]);
}




/**
*********************************************************************************************************
*  �� �� ��: vWirelessRecvProcess
*  ����˵��: ���߽��պ���
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vWirelessRecvProcess(void)
{
    if(WIRELESS_STATUS == Wireless_RX_Finish)
    {

        //DebugSendBytes(Wireless_Buf.Wireless_RxData,Wireless_Buf.Wireless_PacketLength);
        xSemaphoreTake(xSemWireless,1000);			//��ȡ������Դռ�û����ź���
        Si4438_Receive_Start(Wireless_Channel[0]); //���¿�ʼ������������
        xSemaphoreGive(xSemWireless);			//�ͷ�������Դռ�û����ź���
    }

}
/**
*********************************************************************************************************
*  �� �� ��: v
*  ����˵��: �豸����֡����
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vFrameDeviceNetReorganize(QUEUE_WIRELESS_SEND_t *queueMsg)
{
	DebugPrintf("\n�豸����");
	xQueueSend(xQueueWirelessTx, queueMsg, (TickType_t)10);			//ֱ�ӷ������߷�������	
	vTaskDelay(300);
	xQueueSend(xQueueNetTask,queueMsg, (TickType_t)10);			//������������	
}

/**
*********************************************************************************************************
*  �� �� ��: v
*  ����˵��: �豸����֡����
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vFrameDeviceCtrlReorganize(QUEUE_WIRELESS_SEND_t *queueMsg)
{
	uint8_t reSendCnt = 0;
	while(reSendCnt<3)
	{
		DebugPrintf("\n�豸����");
		xQueueSend(xQueueWirelessTx, queueMsg, (TickType_t)10);			//ֱ�ӷ������߷�������	
		reSendCnt ++;
		vTaskDelay(300);
	}
}



/**
*********************************************************************************************************
*  �� �� ��: vWirelessRecvProcess
*  ����˵��: ����Զ�̿�������֡����
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vFrameUartRemoteCmdDeal(uint8_t *buff,uint8_t len)
{
	QUEUE_WIRELESS_SEND_t queueMsg;


	FrameCmdLocalAck(buff,len,0,0);         							//Ӧ��
	
	queueMsg.len = len;
	queueMsg.toCh = Wireless_Channel[0];
	memcpy(queueMsg.msg,buff,len);
	
	//����
	if((buff[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
	{
	     DebugPrintf("\n��Ҫ����");
	     Encrypt_Convert(queueMsg.msg,len , &queueMsg.len, 1);   //����
	}
	else
	{
	    DebugPrintf("\n���ü���");
	}
	
	
	if(memcmp(&buff[Region_DataIDNumber],CMD_INIT[0],3)==0)  	//�豸����
	{
        vFrameDeviceNetReorganize(&queueMsg);
	}
	else														//�豸����
	{
        vFrameDeviceCtrlReorganize(&queueMsg);
	}	
}





/**
*********************************************************************************************************
*  �� �� ��: vDevice
*  ����˵��: �豸����
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/


