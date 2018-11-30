
#include "includes.h"



DeviceInfo_t deviceInfo;
/*
*********************************************************************************************************
*  �� �� ��: vDeviceInfoInit
*  ����˵��: �豸��Ϣ��ʼ��+�豸MAC��ַ��¼
*  ��    ��: ��
*  �� �� ֵ: ��
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
			DebugPrintf("\r\n����д�豸��MAC��ַ����ʽΪHEXЭ��֡ \"68 20 08 xx xx xx xx xx xx xx xx CRC_H CRC_L\"");
		}
		vTaskDelay(10);
	}
}

/*
*********************************************************************************************************
*  �� �� ��: uxIsLowPowerDevice
*  ����˵��: �ж��Ƿ�Ϊ�͹����豸
*  ��    ��: ��
*  �� �� ֵ: 1�͹����豸��0�ǵ͹����豸
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
*  �� �� ��: vLowPowerDeviceWakeUp
*  ����˵��: �͹����豸�����������ݻ���
*  ��    ��: ��
*  �� �� ֵ: ��
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
*  �� �� ��: vDeviceMatchNet
*  ����˵��: �����豸������������
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void  vDeviceMatchNet(uint8_t *buff,uint8_t len)
{
	QUEUE_WIRELESS_SEND_t queueMsg;
	uint8_t *mac = &buff[Region_DataValNumber];
	
	queueMsg.len = FrameRouterCompose_ext(mac, //�����豸��MAC
						  buff,                                        //������������
						  len,                                          //���������
						  queueMsg.msg,                                 //�����������������
						  0,                                            //·�ɱ�
						  0);                                            //·�ɱ���
	queueMsg.toCh = Default_Channel;
	
	
	if(uxIsLowPowerDevice(*(mac+7)) == 1)				//�͹����豸
	{
		DebugPrintf("\n�͹����豸����");	
		
	}
	else   //��ͨ�豸
	{
		DebugPrintf("\n��ͨ�豸����");
	    xQueueSend(xQueueWirelessTx, &queueMsg, (TickType_t)10);			//ֱ�ӷ������߷�������
	}
	
	DebugSendBytes(queueMsg.msg,queueMsg.len);
}

/**
*********************************************************************************************************
*  �� �� ��: vVersionReport
*  ����˵��: ��ȡ����汾��
*  ��    ��: @cmd ���յ�����������
			 @len���յ��������
			 @version Ҫ�ϱ��İ汾��
*  �� �� ֵ: ��
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
*  �� �� ��: vDevicMacReport
*  ����˵��: ��ȡ�豸MAC��ַ
*  ��    ��: @cmd ���յ�����������
			 @len���յ��������
			 @mac Ҫ�ϱ��İ汾��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vDeviceMacReport(uint8_t *cmd,uint8_t len ,uint8_t *mac)
{	
	FrameCmdLocalAck(cmd,len,mac,8);
}


/**
*********************************************************************************************************
*  �� �� ��: vDeviceListSave
*  ����˵��: �����豸�б�
*  ��    ��: @cmd ���յ�����������
			 @len���յ��������
			 @version Ҫ�ϱ��İ汾��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vDeviceListSave(uint8_t *list,uint8_t len)
{	 
	if(len>225)return ;		//����Ĳ����б��ȹ���
	deviceInfo.match.deviceNum = len;
	
	memcpy(deviceInfo.match.deviceBuff,list,len);
	STMFLASH_Write(DEVICE_MATCH_SAVE_ADDR,(uint16_t*)&deviceInfo.match,sizeof(deviceInfo.match));
}





