
#include "includes.h"



/*********************������ض��������************************/

#define START_TASK_PRIO		6 			//�������ȼ�
#define START_STK_SIZE 		192 		//�����ջ��С	 
TaskHandle_t StartTask_Handler;    		//������
void vStartTask(void *pvParameters);   	//������

#define LED_TASK_PRIO		2 			//�������ȼ�
#define LED_STK_SIZE 		64 		//�����ջ��С	 
TaskHandle_t LedTask_Handler;    		//������
void vLedTask(void *pvParameters);   	//������

#define NET_TASK_PRIO		5 			//�������ȼ�
#define NET_STK_SIZE 		192 		//�����ջ��С	 
TaskHandle_t NetTask_Handler;    		//������
void vNetTask(void *pvParameters);   	//������

#define WIRELESS_RX_TASK_PRIO		4 			//�������ȼ�
#define WIRELESS_RX_STK_SIZE 		192 		//�����ջ��С	 
TaskHandle_t WirelessRxTask_Handler;    		//������
void vWirelessRxTask(void *pvParameters);   	//������

#define WIRELESS_TX_TASK_PRIO		3 			//�������ȼ�
#define WIRELESS_TX_STK_SIZE 		256 		//�����ջ��С	 
TaskHandle_t WirelessTxTask_Handler;    		//������
void vWirelessTxTask(void *pvParameters);   	//������


//#define WIRELESS_ROUTE_TASK_PRIO		1 			//�������ȼ�
//#define WIRELESS_ROUTE_STK_SIZE 		192 		//�����ջ��С	 
//TaskHandle_t WirelessRouteTask_Handler;    			//������
//void vWirelessRouteTask(void *pvParameters);   		//������

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
    BaseType_t xReturn;

    //����LED����
    xReturn = xTaskCreate((TaskFunction_t )vLedTask,            //������
                          (const char*    )"vLedTask",          //��������
                          (uint16_t       )LED_STK_SIZE,        //�����ջ��С
                          (void*          )NULL,                //���ݸ��������Ĳ���
                          (UBaseType_t    )LED_TASK_PRIO,       //�������ȼ�
                          (TaskHandle_t*  )&LedTask_Handler);   //������
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvLedTask����ʧ��");
    }

    //����KEY����
    xReturn =  xTaskCreate((TaskFunction_t )vNetTask,            //������
                           (const char*    )"vNetTask",          //��������
                           (uint16_t       )NET_STK_SIZE,        //�����ջ��С
                           (void*          )NULL,                //���ݸ��������Ĳ���
                           (UBaseType_t    )NET_TASK_PRIO,       //�������ȼ�
                           (TaskHandle_t*  )&NetTask_Handler);   //������
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvNetTask����ʧ��");
    }

    //�������߷�������
    xReturn =  xTaskCreate((TaskFunction_t )vWirelessRxTask,            //������
                           (const char*    )"vWirelessRxTask",          //��������
                           (uint16_t       )WIRELESS_RX_STK_SIZE,        //�����ջ��С
                           (void*          )NULL,                		//���ݸ��������Ĳ���
                           (UBaseType_t    )WIRELESS_RX_TASK_PRIO,       //�������ȼ�
                           (TaskHandle_t*  )&WirelessRxTask_Handler);   //������
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvWirelessRxTask����ʧ��");
    }

    //�������߷�������
    xReturn =  xTaskCreate((TaskFunction_t )vWirelessTxTask,            //������
                           (const char*    )"vWirelessTxTask",          //��������
                           (uint16_t       )WIRELESS_TX_STK_SIZE,        //�����ջ��С
                           (void*          )NULL,                		//���ݸ��������Ĳ���
                           (UBaseType_t    )WIRELESS_TX_TASK_PRIO,       //�������ȼ�
                           (TaskHandle_t*  )&WirelessTxTask_Handler);   //������
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvWirelessTxTask����ʧ��");
    }

    //��������·������
//    xReturn =  xTaskCreate((TaskFunction_t )vWirelessRouteTask,            //������
//                           (const char*    )"vWirelessRouteTask",          //��������
//                           (uint16_t       )WIRELESS_ROUTE_STK_SIZE,        //�����ջ��С
//                           (void*          )NULL,                		//���ݸ��������Ĳ���
//                           (UBaseType_t    )WIRELESS_ROUTE_TASK_PRIO,       //�������ȼ�
//                           (TaskHandle_t*  )&WirelessRouteTask_Handler);   //������
//    if(xReturn != pdPASS)
//    {
//        printf("\nvWirelessRouteTask����ʧ��");
//    }

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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���4:0λ��ռ���ȼ�

    StartTaskCreate();

    vTaskStartScheduler();          //�����������
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    DebugPrintf("\n����: %s ����ջ���",pcTaskName);
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
    vUart2Init(9600);
    vSystemLogoDisplay();			//��ӡLOGO��Ϣ
    vKeyInit();						//������ʼ��
    SN3218_Init();
    vDeviceInfoInit();
    vAppWirelessInit();
    AES_Init();                     //����ŵ�vDeviceInifoInit()����
    LowPowerDeviceInit();			//����ŵ�vDeviceInifoInit()����
    AppQueueCreate();               //��������
    AppSemaphoreCreate();
    AppTaskCreate();				//��������
    IWDG_Init(4,625);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s
    while(1)
    {
        vUartFrameProcess(&sUart1Rx);
		IWDG_Feed();
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


    //LED_AroundStaSet(LED_COLOR_ALL,OFF);
	LED_ALL_StaSet(OFF);
    while(1)
    {
        vLedProcess();

        key_now = eKeyScan(0);			//����ɨ��
        vTouchProcess(key_now);         //ִ�а�����Ӧ����
        if(key_now == KEY_ALL_REL)
        {
            vLedCtrlModeLoop(led_set_mode,led_set_color);
        }

        vTaskDelay(25);
    }
}

//const  uint8_t matchNetFrame[39]={
//0xAC,0x43,0x00,0x2A,0x5B,0x0A,0x10,0x1C,0x80,0xFF,0xFF,0xFF,0x04,0x02,0x00,0x01,
//0x00,0x00,0x21,0x01,0xA3,0xA6,0x89,0x26,0xAF,0xA7,0x13,0x29,0x33,0x0A,0xB1,0xA2,
//0x15,0xF8,0xFB,0xDB,0x1C,0xC6,0x53
//};
//void FrameRouter(uint8_t *buff,uint8_t addr,uint8_t *mac,uint8_t *aes,uint8_t *routTab,uint8_t uint8_routLen)
//{
////AC 43 00 2A 5B 0A 10 1C 80 FF FF FF 04 02 00 01 00 00 21 01 A3 A6 89 26 AF A7 13 29 33 0A B1 A2 15 F8 FB DB 1C C6 53
//   buff[0] =
//}

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
    uint8_t device_index = 0;
    uint8_t routerTable[3]= {0xBB,0xCC,0xDD};  //·�ɱ�
    uint8_t frameTemp[39] = {0};               //�豸����Ӧ��Э��֡����
    uint8_t heart_index = 0;
    uint8_t heart_delay1 =  0;
    uint16_t heart_delay2 = 0;

    while(1)
    {
        xResult = xQueueReceive(xQueueNetTask,    	/* ��Ϣ���о�� */
                                (void *)&queueMsg, 		/* �洢���յ������ݵ�����queueMsg */
                                (TickType_t)30);        	/* ��������ʱ��5��tick */
        if (xResult == pdPASS) 							/* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
        {
            DebugPrintf("\n���յ�������������");
            memcpy(frameTemp,&queueMsg.msg[20],39);
            route_flag = 1;
            device_index = 0;
        }
        else
        {
            if(route_flag == 1)
            {

                if(device_index < deviceInfo.match.deviceNum)
                {
                    DebugPrintf("\n����·������");

                    if(deviceInfo.match.deviceBuff[device_index]!= frameTemp[Region_AddrNumber])
                    {
                        routerTable[0] = deviceInfo.match.deviceBuff[device_index];


                        queueMsg.len = FrameRouterCompose_ext( &frameTemp[Region_DataValNumber], //�����豸��MAC
                                                               frameTemp,                                        //������������
                                                               frameTemp[Region_DataLenNumber]+11,              //���������
                                                               queueMsg.msg,                                 //�����������������
                                                               routerTable,                                            //·�ɱ�
                                                               1);                                            //·�ɱ���
                        xQueueSend(xQueueWirelessTx,&queueMsg, (TickType_t)10);			//�������߷�������
                        vTaskDelay(500);
                    }
                }
                else
                {
                    DebugPrintf("\n·���豸�Ѿ��������");
                    route_flag = 0;
                    device_index = 0;
                }
                device_index++;
            }
            else
            {
                heart_delay2 ++;
                if(heart_delay2 > 1200)	//120��
                {
                    heart_delay1 ++;
                    if(heart_delay1 > 30)
                    {
                        heart_delay1 = 0;

                        if(heart_index < deviceInfo.match.deviceNum)
                        {
                            DebugPrintf("\n��ѯ�豸����");
                            FrameHeartCompose(deviceInfo.match.deviceBuff[heart_index++]);

                        }
                        else
                        {

                            heart_index = 0;
                            heart_delay2 = 0;
                        }
                    }
                }
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
                if(frame_len<=70)
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


//static void vWirelessRouteTask(void *pvParameters)
//{
//    uint8_t index = 0;
//    while(1)
//    {
//        //printf("\nvWirelessRouteTask");
//        for(index = 0; index<deviceInfo.match.deviceNum; index++)
//        {
//            FrameHeartCompose(deviceInfo.match.deviceBuff[index]);
//            vTaskDelay(1000);
//        }
//        vTaskDelay(30000);
//    }
//}
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

