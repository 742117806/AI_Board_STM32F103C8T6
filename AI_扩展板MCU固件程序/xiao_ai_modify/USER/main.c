
#include "includes.h"

 
 
/*********************������ض��������************************/

#define START_TASK_PRIO		1 			//�������ȼ�
#define START_STK_SIZE 		320 		//�����ջ��С	 
TaskHandle_t StartTask_Handler;    		//������
void vStartTask(void *pvParameters);   	//������

#define LED_TASK_PRIO		2 			//�������ȼ�
#define LED_STK_SIZE 		128 		//�����ջ��С	 
TaskHandle_t LedTask_Handler;    		//������
void vLedTask(void *pvParameters);   	//������

#define NET_TASK_PRIO		3 			//�������ȼ�
#define NET_STK_SIZE 		256 		//�����ջ��С	 
TaskHandle_t NetTask_Handler;    		//������
void vNetTask(void *pvParameters);   	//������

#define WIRELESS_RX_TASK_PRIO		4 			//�������ȼ�
#define WIRELESS_RX_STK_SIZE 		256 		//�����ջ��С	 
TaskHandle_t WirelessRxTask_Handler;    		//������
void vWirelessRxTask(void *pvParameters);   	//������

#define WIRELESS_TX_TASK_PRIO		5 			//�������ȼ�
#define WIRELESS_TX_STK_SIZE 		256 		//�����ջ��С	 
TaskHandle_t WirelessTxTask_Handler;    		//������
void vWirelessTxTask(void *pvParameters);   	//������



 
/************************************************

************************************************/
/* ������ʼ���� */
void StartTaskCreate(void)
{
	//������ʼ����
    xTaskCreate((TaskFunction_t )vStartTask,            //������
                (const char*    )"vStartTask",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������   
}
/*��������*/

void AppTaskCreate(void)
{
 

	//����LED����
    xTaskCreate((TaskFunction_t )vLedTask,            //������
                (const char*    )"vLedTask",          //��������
                (uint16_t       )LED_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                //���ݸ��������Ĳ���
                (UBaseType_t    )LED_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&LedTask_Handler);   //������ 

	//����KEY����
    xTaskCreate((TaskFunction_t )vNetTask,            //������
                (const char*    )"vNetTask",          //��������
                (uint16_t       )NET_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                //���ݸ��������Ĳ���
                (UBaseType_t    )NET_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&NetTask_Handler);   //������  

	//�������߷�������
    xTaskCreate((TaskFunction_t )vWirelessRxTask,            //������
                (const char*    )"vWirelessRxTask",          //��������
                (uint16_t       )WIRELESS_RX_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                		//���ݸ��������Ĳ���
                (UBaseType_t    )WIRELESS_RX_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&WirelessRxTask_Handler);   //������  

	//�������߷�������
    xTaskCreate((TaskFunction_t )vWirelessTxTask,            //������
                (const char*    )"vWirelessTxTask",          //��������
                (uint16_t       )WIRELESS_TX_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                		//���ݸ��������Ĳ���
                (UBaseType_t    )WIRELESS_TX_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&WirelessTxTask_Handler);   //������  	
				
}
/* �������� */
void AppQueueCreate(void)
{
	xQueueWirelessTx = xQueueCreate(5, sizeof(QUEUE_WIRELESS_SEND_t)); //���г���3��
	
	xQueueLedTask = xQueueCreate(10, sizeof(uint8_t)); 			//10�����У�ÿ�����г���Ϊuint8_t
	
	xQueueNetTask = xQueueCreate(3, sizeof(QUEUE_WIRELESS_SEND_t)); 
	
}

/* ���������ź���*/
void AppSemaphoreCreate(void)
{

	xSemWireless = xSemaphoreCreateMutex();
	xSemaphoreGive(xSemWireless);
}


/* 
********************************************************************************************************* 
*  �� �� ��: vSystemLogoDisplay 
*  ����˵��: ��ӡϵͳ�����Ϣ
*  ��    ��: ��
*  �� �� ֵ: ��  
********************************************************************************************************* 
*/ 
void vSystemLogoDisplay(void)
{
   DebugPrintf("\n*******************FeerRTOS ����*********************\n");
   DebugPrintf("���ڣ�%s %s \n",__DATE__,__TIME__);
}
/* 
********************************************************************************************************* 
*  �� �� ��: main 
*  ����˵��: 
*  ��    ��: ��
*  �� �� ֵ: ��  
********************************************************************************************************* 
*/ 
int main(void)
{		
	delay_init(72);	    //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����NVIC�жϷ���4:0λ��ռ���ȼ�

	StartTaskCreate();
		
	vTaskStartScheduler();          //�����������
}



/* 
********************************************************************************************************* 
*  �� �� ��: vTaskStart 
*  ����˵��: 
*  ��    ��: pvParameters ���ڴ���������ʱ���ݵ��β� 
*  �� �� ֵ: �� 
*  �� �� ��: 4   
********************************************************************************************************* 
*/ 
static void vStartTask(void *pvParameters) 
{ 

	vUart1Init(38400);
	vUart2Init(38400);
	vSystemLogoDisplay();			//��ӡLOGO��Ϣ
	vKeyInit();						//������ʼ��
	SN3218_Init();
	vAppWirelessInit();
	vDeviceInfoInit();
	AES_Init();                     //����ŵ�vDeviceInifoInit()����
	//LowPowerDeviceInit();			//����ŵ�vDeviceInifoInit()����  ����������
	AppQueueCreate();               //��������
	AppSemaphoreCreate();
	AppTaskCreate();				//��������	
    
    while(1) 
    { 
		vUartFrameProcess(&sUart1Rx);
		
		vTaskDelay(10);
    } 
	
} 


/* 
********************************************************************************************************* 
*  �� �� ��: vLedTask 
*  ����˵��: ������尴����LED������
*  ��    ��: pvParameters ���ڴ���������ʱ���ݵ��β� 
*  �� �� ֵ: �� 
*  �� �� ��: 4   
********************************************************************************************************* 
*/ 
static void vLedTask(void *pvParameters) 
{ 
    eKEY_VALUE key_now = KEY_NONE;  //��ǰ����
   
	 	 
    LED_AroundStaSet(LED_COLOR_ALL,ON);
    while(1) 
    { 
		vLedProcess();
		
        key_now = eKeyScan(0);			//����ɨ��
		vTouchProcess(key_now);         //ִ�а�����Ӧ����
		if(key_now == KEY_ALL_REL)
		{
			vLedCtrlModeLoop(led_set_mode);
		}
		
        vTaskDelay(25); 		
    } 
} 

/* 
********************************************************************************************************* 
*  �� �� ��: vKeyTask 
*  ����˵��: 
*  ��    ��: pvParameters ���ڴ���������ʱ���ݵ��β� 
*  �� �� ֵ: �� 
*  �� �� ��: 4   
********************************************************************************************************* 
*/ 
static void vNetTask(void *pvParameters) 
{ 
	BaseType_t xResult;
    QUEUE_WIRELESS_SEND_t queueMsg;
	uint8_t route_flag = 0;//��Ҫ����·������

	
    while(1) 
    { 
		xResult = xQueueReceive(xQueueNetTask,    	/* ��Ϣ���о�� */
								(void *)&queueMsg, 		/* �洢���յ������ݵ�����queueMsg */
								(TickType_t)30);        	/* ��������ʱ��5��tick */
		if (xResult == pdPASS) 							/* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
        {
			DebugPrintf("\n���յ�������������");
		    //vRouteFrameMatchProcess(&deviceInfo.match,&queueMsg);
			
			xQueueSend(xQueueWirelessTx,&queueMsg, (TickType_t)10);			//�������߷�������		
		}
		else
		{
		    if(route_flag == 1)
			{
				DebugPrintf("\n���ճ�ʱ��ִ��·������");
			}
		}
        vTaskDelay(1); 
    } 
} 
/* 
********************************************************************************************************* 
*  �� �� ��: vWirelessTxTask 
*  ����˵��: 
*  ��    ��: pvParameters ���ڴ���������ʱ���ݵ��β� 
*  �� �� ֵ: �� 
*  �� �� ��: 4   
********************************************************************************************************* 
*/
static void vWirelessTxTask(void *pvParameters) 
{ 
	BaseType_t xResult;
    QUEUE_WIRELESS_SEND_t queueMsg;
    uint8_t frame_len;
    while(1) 
    { 
		xResult = xQueueReceive(xQueueWirelessTx,    	/* ��Ϣ���о�� */
								(void *)&queueMsg, 		/* �洢���յ������ݵ�����queueMsg */
								(TickType_t)5);        	/* ��������ʱ��5��tick */
		if (xResult == pdPASS) 							/* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
        {
			DebugPrintf("\n��������������ճɹ�");
				//74����
			if((queueMsg.msg[0]==0x69)&&(queueMsg.msg[1]==0x69))
			{
				frame_len = queueMsg.msg[2];
				if(frame_len<140)
				{
					FrameRouteData_74Convert(queueMsg.msg,queueMsg.len,&queueMsg.len,1);
				}
			}
			else if(queueMsg.msg[0]==0xAC)
			{
				frame_len = queueMsg.msg[Region_DataLenNumber];
				if(frame_len<70)
				{
					FrameData_74Convert(queueMsg.msg,queueMsg.len,&queueMsg.len,1);
				}
			}
			vWirelessSendBytes(queueMsg.toCh,queueMsg.msg,queueMsg.len);
		}		
        vTaskDelay(30); 
    } 
} 

/* 
********************************************************************************************************* 
*  �� �� ��: vWirelessTxTask 
*  ����˵��: 
*  ��    ��: pvParameters ���ڴ���������ʱ���ݵ��β� 
*  �� �� ֵ: �� 
*  �� �� ��: 4   
********************************************************************************************************* 
*/
static void vWirelessRxTask(void *pvParameters) 
{ 
	while(1) 
    { 
	   vWirelessRecvProcess();
	   vTaskDelay(30); 
	}
}
 /**
 *****************����ע�ӵĴ�����ͨ�����ÿ⺯����ʵ��IO���Ƶķ���*****************************************
int main(void)
{ 
 
	delay_init();		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
	while(1)
	{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);  //LED0��Ӧ����GPIOB.5���ͣ���  ��ͬLED0=0;
			GPIO_SetBits(GPIOE,GPIO_Pin_5);   //LED1��Ӧ����GPIOE.5���ߣ��� ��ͬLED1=1;
			delay_ms(300);  		   //��ʱ300ms
			GPIO_SetBits(GPIOB,GPIO_Pin_5);	   //LED0��Ӧ����GPIOB.5���ߣ���  ��ͬLED0=1;
			GPIO_ResetBits(GPIOE,GPIO_Pin_5); //LED1��Ӧ����GPIOE.5���ͣ��� ��ͬLED1=0;
			delay_ms(300);                     //��ʱ300ms
	}
} 
 
 ****************************************************************************************************
 ***/
 

	
/**
*******************����ע�͵��Ĵ�����ͨ�� ֱ�Ӳ����Ĵ��� ��ʽʵ��IO�ڿ���**************************************
int main(void)
{ 
 
	delay_init();		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
	while(1)
	{
     GPIOB->BRR=GPIO_Pin_5;//LED0��
	   GPIOE->BSRR=GPIO_Pin_5;//LED1��
		 delay_ms(300);
     GPIOB->BSRR=GPIO_Pin_5;//LED0��
	   GPIOE->BRR=GPIO_Pin_5;//LED1��
		 delay_ms(300);

	 }
 }
**************************************************************************************************
**/

