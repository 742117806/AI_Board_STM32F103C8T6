//freeRTOS����Ӧ��

#include "taskApp.h"
#include "user_call_back.h"
#include "spi.h"
#include "delay.h"
#include "frame_process.h"
#include "led.h"
/*
**********************************************************************************************************
                      ��������
**********************************************************************************************************
*/
extern void System_8msTick_Process(void);
extern void WireLess_Process(WLS *p_wl, DevicePara_TypDef *p_device);
extern void UartRx_Process(UpCom_Rx_TypDef *prx_ubuf, DevicePara_TypDef *p_device);
//extern void RxData_Process(HKFrame_TypDef *p_framebuf, DevicePara_TypDef *p_device);

extern void Led_Process(DevicePara_TypDef *p_device, Led_Color_TypeDef colour);
extern uint8_t FrameRouterCompose(
    uint8_t desAddr,
    uint8_t *srcData,
    uint8_t srcLen,
    uint8_t *outData,
    uint8_t *routerTab,
    uint8_t routerLen);
/*
**********************************************************************************************************
                       ����
**********************************************************************************************************
*/
//QUEUE_WIRELESS_SEND_t retryWirelessBuff;
RETRY_WAITE_FOR_t retryWaiteFor;

/* ������ */
osThreadId UartTaskHandle;
osThreadId LedTaskHandle;
osThreadId TouchTaskHandle;
osThreadId WirelessTaskHandle;
osThreadId SuperviseTaskHandle;
osThreadId RetryTaskHandle; //�ȴ�Ӧ�����·������������
osThreadId NetCreateTaskHandle;
osThreadId IWDG_TaskHandle; //�ȴ�Ӧ�����·������������

/* ������ */
QueueHandle_t xQueueWirelessTask = NULL;
//QueueHandle_t xQueue2 = NULL;
QueueHandle_t xQueueVol = NULL;
//QueueHandle_t xQueueKeyVal = NULL;
QueueHandle_t xQueueAckRouterTable = NULL;
QueueHandle_t xQueueNetCreateTask = NULL;
QueueHandle_t xQueueWirelessRetryTask = NULL;

/* �����ź������ */
//SemaphoreHandle_t xSemaphore_wireless = NULL;
SemaphoreHandle_t xSemaphore_uartTx = NULL;

/* ֱ�ӱ�־���� */
EventGroupHandle_t xIWDG_EventGroup;

uint8_t currentFrameNum = 0; //��ǰ�Ľ��յ��ϲ��֡���

extern uint8_t FrameRouterHeart(
    uint8_t desAddr,
    uint8_t *srcData,
    uint8_t srcLen,
    uint8_t *outData,
    uint8_t *routerTab,
    uint8_t routerLen);

/* �����¼�����*/

void DeviceReNet(uint8_t des_addr) //�豸��������
{
    extern uint8_t const Set_LogicAddr_Id[3];
    UpCom_Rx_TypDef send_buff;
    FRAME_CMD_t *frame_cmd = (FRAME_CMD_t *)send_buff.Frame_Data;
    JOINE_NET_CMD_t *p_joine_net = (JOINE_NET_CMD_t *)frame_cmd->userData.content;

    //AC BA 00 2A 5B 0A 10 1C 80 FF FF FF 04 02 00 01 00 00 22 01 68 AE 6D 7E 96 49 FD 2D 72 80 9A 5C F9 7E 44 9B 88 EA 53
    frame_cmd->DataLen = 0x1C;
    frame_cmd->addr_DA = des_addr;
    memcpy(frame_cmd->addr_GA, deviceInfo.addr_GA, 3);
    frame_cmd->Ctrl.relayFlag = 1; //ת��֡
    frame_cmd->Ctrl.dir = 0;       //������
    memcpy(frame_cmd->userData.Index, Set_LogicAddr_Id, 3);
    frame_cmd->userData.AFN = 0x80;
    memcpy(p_joine_net->mac, deviceInfo.mac, 8);
    memcpy(p_joine_net->aes, deviceInfo.aes, 16);

    send_buff.FrameTotalLen = Frame_Compose((uint8_t *)frame_cmd);

    xQueueSend(xQueueNetCreateTask, &send_buff, (TickType_t)100);
}
/*
*********************************************************************************************************
*  �� �� ��:  AppSuperviseTask
*  ����˵��:
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
//extern uint8_t deviceBuff[224];
extern DES_DEVICE_t desDevice[224];
//extern uint8_t deviceNum;
void AppSuperviseTask(void const *argument)
{
    //	unsigned portBASE_TYPE uxHighWaterMark;
    uint8_t userTemp[1] = {0}; //Ӧ�ò�����
    uint8_t routerTab[3] = {0};
    uint8_t routerLen = 0;
    QUEUE_WIRELESS_SEND_t queue_wireless;
    BaseType_t xResult;
    uint32_t ulValue;
    uint8_t retry_cnt = 0; //���Դ���
    uint8_t i, j;

    for (;;)
    {
        osDelay(30000);
        for (j = 0; j < deviceInfo.deviceNum; j++)
        {
            userTemp[0] = 0;
            routerLen = desDevice[deviceInfo.deviceBuff[j] - DEVICE_INDEX_OFFSET].path1.len;
            if (routerLen > 0)
            {

                for (i = 0; i < routerLen; i++)
                {
                    routerTab[i] = desDevice[deviceInfo.deviceBuff[j] - DEVICE_INDEX_OFFSET].path1.addr[i];
                }
            }
            queue_wireless.len = FrameRouterHeart(deviceInfo.deviceBuff[j], userTemp, 1, queue_wireless.msg, routerTab, routerLen);
            queue_wireless.toCh = Default_Channel;
            xQueueSend(xQueueWirelessTask, &queue_wireless, (TickType_t)100);

            xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, (TickType_t)1000); //�ȴ���Ӧ
            if (xResult == pdPASS)                                                         /* �ɹ����� */
            {
                if (ulValue == deviceInfo.deviceBuff[j])
                {
                    //printf("111");
                    retry_cnt = 0; //�������
                    osDelay(500);
                }
            }
            else
            {
                if (j > 0)
                {
                    j--; //�´λ��ǲ�ѯ���ʧ�ܵ��豸
                }
                retry_cnt++;
                if (retry_cnt > 2) //3�����Զ�ʧ��
                {
                    retry_cnt = 0;
                    //printf("333");

                    userTemp[0] = 0;
                    routerLen = 1;
                    for (i = 0; i < deviceInfo.deviceNum; i++)
                    {
                        if (deviceInfo.deviceBuff[j + 1] == deviceInfo.deviceBuff[i])
                            continue;
                        routerTab[0] = deviceInfo.deviceBuff[i];
                        queue_wireless.len = FrameRouterHeart(deviceInfo.deviceBuff[j + 1], userTemp, 1, queue_wireless.msg, routerTab, routerLen);
                        //queue_wireless.toCh = Wireless_Channel[0];
						queue_wireless.toCh = Default_Channel;
                        xQueueSend(xQueueWirelessTask, &queue_wireless, (TickType_t)100);
                        xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, (TickType_t)1000); //�ȴ���Ӧ
                        if (xResult == pdPASS)                                                         /* �ɹ����� */
                        {
                            if (ulValue == deviceInfo.deviceBuff[j + 1]) //�յ�Ӧ��
                            {
                                //printf("4444");
                                break;
                            }
                        }
                    }
                    j++; //��ѯ��һ���豸
                }
                //printf("222");
            }
        }
    }
}
uint8_t frameSeq = 0; //֡���

void KeyUpReportCmd(uint8_t keyVal, uint8_t longFlag)
{
    extern uint8_t const Key_Event_ID[3];
    extern uint8_t const Self_LogicAddr[4];
    uint8_t frameLen = 0;
    FRAME_CMD_t frameCmd;

    memmove(&frameCmd.addr_DA, Self_LogicAddr, LogicAddr_Len);
    frameCmd.FSQ.frameNum = (frameSeq & 0x0f);
    frameSeq++;
    frameCmd.Ctrl.dir = 1;
    frameCmd.Ctrl.eventFlag = 1;

    frameCmd.DataLen = 5;
    frameCmd.userData.AFN = keyVal;
    memmove(frameCmd.userData.Index, Key_Event_ID, 3);
    frameCmd.userData.content[0] = longFlag;
    frameLen = Frame_Compose((uint8_t *)&frameCmd);

    UartSendBytes(USART1, (uint8_t *)&frameCmd, frameLen);
}
uint8_t deviceAckBuff[256] = {0};
uint8_t deviceAckLen = 0;

void AppUartTask(void const *argument)
{

    for (;;)
    {
        UartRx_Process(&UpCom_RxBuf, &Device_ParaBuf);
        xEventGroupSetBits(xIWDG_EventGroup, IWDG_EventGroup_BIT0);
        osDelay(8);
    }
}
/*
*********************************************************************************************************
*  �� �� ��: AppLedTask
*  ����˵��:
*  ��    ��: ��
*  �� �� ֵ: ��
*    �� �� ��: 1  (��ֵԽС���ȼ�Խ�ͣ������ uCOS�෴)
*********************************************************************************************************
*/
void AppLedTask(void const *argument)
{
    BaseType_t xResult;

    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5); //5ms
    uint32_t ulValue;
    LedFunc_t *ledFunc;

    uint16_t LED_RunMode = 0; //LED������ģʽ
    uint8_t vol;              //LED��ֵ

    uint8_t lastVaule;         //��һ��led��ֵ
    uint8_t vol_led_delay = 0; //����LED������ʱ
    uint16_t lastMode = 0;     //��һ״̬
    uint8_t xQueueVal;

    LED_CentreOn();

    for (;;)
    {

        xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, xMaxBlockTime);
        if (xResult == pdPASS)
        {

            switch (ulValue)
            {
            case LED_TASK_EVNT_KEY1_DN:
                LED_RunMode = 6;
                break;
            case LED_TASK_EVNT_KEY3_DN:
                LED_RunMode = 7;
                break;
            case LED_TASK_EVNT_KEY2_DN:
                LED_RunMode = 5;
                break;
            case LED_TASK_EVNT_KEY4_DN:
                LED_RunMode = 5;
                break;
            case LED_TASK_EVNT_KEY1_UP:
                LED_RunMode = 8;
            case LED_TASK_EVNT_KEY3_UP:
                LED_RunMode = 9;
                break;

            default:
                break;
            }
        }
        xResult = xQueueReceive(xQueueVol, &xQueueVal, (TickType_t)5);
        if (xResult == pdPASS) /* �ɹ����� */
        {
            ledFunc = (LedFunc_t *)&xQueueVal;
            vol = ledFunc->ledColor;
            LED_RunMode = ledFunc->cmdType;
            vol_led_delay = 0;
        }

        switch (LED_RunMode)
        {
        case 0:

            LED_AroundOff();
            lastMode = 0;         //˯��
            LED_RunMode = 0x8000; //Ϊ�˱��ⲻͣ��дLED����IC�ļĴ���

            break;
        case 1: //������
            LED_Breath(LED_BLUE_MAP_BITS, 20, 10);
            lastMode = 1; //������
            break;
        case 2:
            LED_AroundOn((LedColor_e)vol);
            lastMode = 2; //����
            lastVaule = vol;
            break;
        case 3: //��ˮ��ģʽ
            lastMode = 3;
            LED_Flow((LedColor_e)vol, 2, 10);

            break;
        case 4: //�м�Ƶ���
            LED_LightCtrl(LED_CENTRE_MAP_BITS, LED_CENTRE_LIGHT_VALUE);
            break;
        case 5:
            LED_AroundVolIndex(vol);
            vol_led_delay++;
            if (vol_led_delay > 100)
            {
                LED_RunMode = lastMode;
                vol = lastVaule;
            }
            break;
        case 6: //����1����
            LED_Key1Show(RED_OR_ORANGE);
            break;
        case 7: //����3����
            LED_Key3Show(RED_OR_ORANGE);
            break;
        case 8: //����1̧��
        case 9: //����3̧��
            LED_RunMode = lastMode;
            vol = lastVaule;
            break;

        default:

            break;
        }
        xEventGroupSetBits(xIWDG_EventGroup, IWDG_EventGroup_BIT1);
        osDelay(1);
    }
}

/*
*********************************************************************************************************
*  �� �� ��: AppTouchTask
*  ����˵��:
*  ��    ��: ��
*  �� �� ֵ: ��
*    �� �� ��: 2
*********************************************************************************************************
*/
void AppTouchTask(void const *argument)
{
    uint8_t keyValue = 0;
    uint8_t keyLast = 0; //�ϴΰ�����ֵ

    for (;;)
    {

        keyValue = TouchKeyScan(0);

        if (keyValue > 0)
        {

            switch (keyValue)
            {
            case KEY1_PRES:
                xTaskNotify(LedTaskHandle, LED_TASK_EVNT_KEY1_DN, eSetValueWithOverwrite);
                break;
            case KEY2_PRES:
                KeyUpReportCmd(keyValue & 0x0f, (keyValue & 0xf0) >> 4); //ͨ�����ڷ��Ͱ����¼�֡
                break;
            case KEY3_PRES:
                xTaskNotify(LedTaskHandle, LED_TASK_EVNT_KEY3_DN, eSetValueWithOverwrite);

                break;
            case KEY4_PRES:
                KeyUpReportCmd(keyValue & 0x0f, (keyValue & 0xf0) >> 4); //ͨ�����ڷ��Ͱ����¼�֡
                break;
            case KEY1_LONG_PRES:
                xTaskNotify(LedTaskHandle, LED_TASK_EVNT_KEY1_DN_L, eSetValueWithOverwrite);
                KeyUpReportCmd(keyLast & 0x0f, (keyValue & 0xf0) >> 4); //ͨ�����ڷ��Ͱ����¼�֡
                break;
            case KEY_UP:
                if (keyLast == KEY1_PRES)
                {
                    xTaskNotify(LedTaskHandle, LED_TASK_EVNT_KEY1_UP, eSetValueWithOverwrite);
                    KeyUpReportCmd(keyLast & 0x0f, (keyLast & 0xf0) >> 4); //ͨ�����ڷ��Ͱ����¼�֡
                }
                else if (keyLast == KEY3_PRES)
                {
                    xTaskNotify(LedTaskHandle, LED_TASK_EVNT_KEY3_UP, eSetValueWithOverwrite);
                    KeyUpReportCmd(keyLast & 0x0f, (keyLast & 0xf0) >> 4); //ͨ�����ڷ��Ͱ����¼�֡
                }
                else if (keyLast == KEY1_LONG_PRES)
                {
                    xTaskNotify(LedTaskHandle, LED_TASK_EVNT_KEY1_UP, eSetValueWithOverwrite);
                }

                break;
            default:

                break;
            }
            keyLast = keyValue; //�����ϴ�ֵ
        }
        xEventGroupSetBits(xIWDG_EventGroup, IWDG_EventGroup_BIT2);
        osDelay(10);
    }
}

/*
*********************************************************************************************************
*  �� �� ��: AppWirelessTask
*  ����˵��:
*  ��    ��: ��
*  �� �� ֵ: ��
*    �� �� ��: 2
*********************************************************************************************************
*/

void AppWirelessTask(void const *argument)
{
    BaseType_t xResult;
    QUEUE_WIRELESS_SEND_t queueUartMsg;


    for (;;)
    {
//    memset(&queueUartMsg,0x00,sizeof(QUEUE_WIRELESS_SEND_t)) ;
        xResult = xQueueReceive(xQueueWirelessTask,    /* ��Ϣ���о�� */
                                (void *)&queueUartMsg, /* �洢���յ������ݵ�����queueUartMsg */
                                (TickType_t)5);        /* ��������ʱ��5��tick */

        if (xResult == pdPASS) /* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
        {
            osDelay(50);
            if((queueUartMsg.msg[0]==0x69)&&(queueUartMsg.msg[1]==0x69))
            {
                if(queueUartMsg.msg[2]<70)
                {
                    FrameRouteData_74Convert((FRAME_ROUTER_CMD_t*)queueUartMsg.msg,queueUartMsg.len,&queueUartMsg.len,1);
                }
            }
            Si4438_Transmit_Start(&Wireless_Buf, queueUartMsg.toCh, queueUartMsg.msg, queueUartMsg.len);
        }

        //		Si4438_Transmit_Start(&Wireless_Buf, Default_Channel, "Test Wireless Data", strlen("Test Wireless Data"));
        //      osDelay(1000);
        if (WIRELESS_STATUS == Wireless_RX_Finish)
        {
            WireLess_Process(&Wireless_Buf, &Device_ParaBuf);
            if(WIRELESS_STATUS == Wireless_RX_Finish)
            {
                Si4438_Receive_Start(Wireless_Channel[0]); //��ʼ������������
            }
        }
        if (WIRELESS_STATUS == Wireless_TX_Finish)
        {
            DEBUG_Printf("Wireless_TX_Finish\r\n");
            WIRELESS_STATUS = Wireless_Free;
            Si4438_Receive_Start(Wireless_Channel[0]); //��ʼ������������
        }
        else if (WIRELESS_STATUS == Wireless_RX_Failure)
        {
            //WirelessRx_Timeout_Cnt = 0;
            delay_ms(200);
            Si4438_Receive_Start(Wireless_Channel[0]); //��ʼ������������
        }



//    if (WIRELESS_STATUS == Wireless_RX_Finish) //Receive Finish
//    {

//      //UartSendBytes(USART1,Wireless_Buf.Wireless_RxData,Wireless_Buf.Wireless_PacketLength);
//      //printf("Wireless_RX_Finish");
//      WIRELESS_STATUS = Wireless_Free;
//      Wireless_ErrCnt = 0;
//      WireLess_Process(&Wireless_Buf, &Device_ParaBuf);
//    }
//    else if (WIRELESS_ERROR_CODE == Wireless_CmdResponseError)
//    {
//      WIRELESS_ERROR_CODE = Wireless_NoError;
//      /*Si4438_Receive_Start(Wireless_Channel[0]);   //Start Receive*/
//      if (++Wireless_ErrCnt > 10)
//        WIRELESS_ERROR_CODE = Wireless_InitError;
//    }

//    if (WIRELESS_ERROR_CODE == Wireless_ExecuteError) //Channel Busy
//    {
//      WIRELESS_STATUS = Wireless_Free;

//      Si4438_Receive_Start(Wireless_Channel[0]); //Start Receive
//    }

//    if (WIRELESS_STATUS == Wireless_TX_Finish)
//    {
//      WIRELESS_STATUS = Wireless_Free;
//      Wireless_ErrCnt = 0;
//      //LEDR_INVERSE();
//      Si4438_Receive_Start(Wireless_Channel[0]); //Start Receive
//    }

//    else if (WIRELESS_STATUS == Wireless_RX_Failure)
//    {
//      WIRELESS_STATUS = Wireless_Free;
//      Wireless_ErrCnt = 0;
//      Res_FifoInfo(RESET_RECEIVE);
//      //Si4438_Receive_Start(Wireless_Channel[0]);         //Start Receive
//    }

        xEventGroupSetBits(xIWDG_EventGroup, IWDG_EventGroup_BIT3);
        osDelay(1);
    }
}

//���ߵ�����������
void AppRetryTask(void const *argument)
{

    uint32_t ulValue;
    uint8_t retry_cnt = 0;
    BaseType_t xResult;
    QUEUE_WIRELESS_SEND_t queue_wireless_send;

    for (;;)
    {

        xResult = xQueueReceive(xQueueWirelessRetryTask, &queue_wireless_send, portMAX_DELAY);

        if (xResult == pdPASS) /* �ɹ����� */
        {
            retry_cnt = 3;
            while (retry_cnt--)
            {

                xQueueSend(xQueueWirelessTask, &queue_wireless_send, (TickType_t)10);

                xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, (TickType_t)500);
                if (xResult == pdPASS)
                {
                    if (ulValue == (1ul << 1))
                        break;
                }
            }
        }
        osDelay(1);
    }
}

//����·�ɲ�������
void GetRouterTable(uint8_t device,uint8_t *routerTab)
{
	uint16_t i,j;
	
	
	
}

/*
*********************************************************************************************************
*  �� �� ��: AppNetCreateTask
*  ����˵��: ��������
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
extern uint8_t FrameRouterCompose_ext(uint8_t *desAddrMAC,
                                      uint8_t *srcData,
                                      uint8_t srcLen,
                                      uint8_t *outData,
                                      uint8_t *routerTab,
                                      uint8_t routerLen);
void AppNetCreateTask(void const *argument)
{

    uint32_t ulValue;
    uint8_t i, j;

    uint8_t routerTab[3];
    uint8_t routerLen;

    FRAME_CMD_t *userFrame;
    BaseType_t xResult;
    UpCom_Rx_TypDef pxQueueTemp;
    QUEUE_WIRELESS_SEND_t queue_wireless_send;
    FRAME_CMD_t *p_ack = (FRAME_CMD_t *)deviceAckBuff;

    for (;;)
    {
        xResult = xQueueReceive(xQueueNetCreateTask, &pxQueueTemp, portMAX_DELAY);
        if (xResult == pdPASS) /* �ɹ����� */
        {
            userFrame = (FRAME_CMD_t *)&pxQueueTemp.Frame_Data;
            JOINE_NET_CMD_t *joine_cmd = (JOINE_NET_CMD_t *)userFrame->userData.content;
            if (p_ack->addr_DA == userFrame->addr_DA)
            {
                p_ack->FSQ.frameNum = currentFrameNum;
                deviceAckLen = Frame_Compose((uint8_t *)p_ack);
                UpUart_DataTx(deviceAckBuff, deviceAckLen, 0);
            }
            else
            {
                //ֱ��������û�о����м��豸
                for (i = 0; i < 3; i++)
                {
                    userFrame->FSQ.frameNum = currentFrameNum;
                    pxQueueTemp.FrameTotalLen = Frame_Compose((uint8_t *)pxQueueTemp.Frame_Data);

                    queue_wireless_send.len = FrameRouterCompose_ext(joine_cmd->mac,
                                              pxQueueTemp.Frame_Data,
                                              pxQueueTemp.FrameTotalLen,
                                              queue_wireless_send.msg,
                                              0,
                                              0);


                    xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, 2); //�������Ӧ

                    queue_wireless_send.toCh = Default_Channel;
                    xQueueSend(xQueueWirelessTask, &queue_wireless_send, (TickType_t)10);	//����·�ɵ�����

                    xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, 800); //�ȴ���Ӧ
                    if ((xResult == pdPASS) && (ulValue == (1ul << 1)))
                    {
                        break;
                    }

                    //����0xACЭ��������û��·��Э��
                    userFrame->FSQ.frameNum = currentFrameNum;
                    queue_wireless_send.len = pxQueueTemp.FrameTotalLen;
                    queue_wireless_send.toCh = Default_Channel;

                    memcpy(queue_wireless_send.msg,pxQueueTemp.Frame_Data,pxQueueTemp.FrameTotalLen);
                    FrameData_74Convert((FRAME_CMD_t*)queue_wireless_send.msg,pxQueueTemp.FrameTotalLen,&queue_wireless_send.len,1); //����

                    xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, 2); //�������Ӧ
                    xQueueSend(xQueueWirelessTask, &queue_wireless_send.msg, (TickType_t)10);   //����·�ɵ�������0xAC��ͷ��
                    xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, 800); //�ȴ���Ӧ
                    if ((xResult == pdPASS) && (ulValue == (1ul << 1)))
                    {
                        break;
                    }
                }
                if (ulValue != (1ul << 1))
                {

                    for (i = 0; i < deviceInfo.deviceNum; i++)      //·�ɲ�ѯ
                    {
                        if (deviceInfo.deviceBuff[i] == userFrame->addr_DA)
                            continue;
                        routerTab[0] = deviceInfo.deviceBuff[i];

                        routerLen = 1;
                        queue_wireless_send.len = FrameRouterCompose_ext(joine_cmd->mac,
                                                  pxQueueTemp.Frame_Data,
                                                  pxQueueTemp.FrameTotalLen,
                                                  queue_wireless_send.msg,
                                                  routerTab,
                                                  routerLen);
                        for (j = 0; j < 2; j++)
                        {
                            queue_wireless_send.toCh = Default_Channel;
                            xQueueSend(xQueueWirelessTask, &queue_wireless_send, (TickType_t)10);

                            xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, 800);
                            if (xResult == pdPASS)
                            {
                                if (ulValue == (1ul << 1))
                                {
                                    break;
                                }
                            }
                        }
                        if (xResult == pdPASS)
                        {
                            if (ulValue == (1ul << 1))
                            {
                                break;
                            }
                        }
                    }
                }
                xQueueReceive(xQueueNetCreateTask, &pxQueueTemp, 1);
                xQueueReceive(xQueueNetCreateTask, &pxQueueTemp, 1);
            }

        }
        osDelay(1);
    }
}

//���Ź�����
void App_IWDG_Task(void const *argument)
{
    extern IWDG_HandleTypeDef hiwdg;
    EventBits_t uxBits;

    for(;;)
    {
        uxBits = xEventGroupWaitBits(xIWDG_EventGroup,
                                     IWDG_EventGroup_BITALL,
                                     pdTRUE,
                                     pdTRUE,
                                     (TickType_t)1000);
        if((uxBits & IWDG_EventGroup_BITALL) == IWDG_EventGroup_BITALL)
        {

            HAL_IWDG_Refresh(&hiwdg);
        }

        osDelay(1);
    }
}

/*
*********************************************************************************************************
*  �� �� ��: AppTaskCreate
*  ����˵��: ����Ӧ������
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void AppTaskCreate(void)
{
    xQueueWirelessTask = xQueueCreate(5, sizeof(QUEUE_WIRELESS_SEND_t)); //���г���3��
    xQueueVol = xQueueCreate(2, sizeof(uint8_t));
    xQueueAckRouterTable = xQueueCreate(2, sizeof(ROUTER_TAB_ACK_t));
    xQueueNetCreateTask = xQueueCreate(1, sizeof(UpCom_Rx_TypDef));
    xQueueWirelessRetryTask = xQueueCreate(1, sizeof(QUEUE_WIRELESS_SEND_t));


    /* ���������ź��� */
    xSemaphore_uartTx = xSemaphoreCreateMutex();
    xSemaphoreGive(xSemaphore_uartTx);

    osThreadDef(NetCreateTask, AppNetCreateTask, osPriorityNormal, 3, 256);
    osThreadDef(UartTask, AppUartTask, osPriorityNormal, 1, 256);
    osThreadDef(LedTask, AppLedTask, osPriorityNormal, 0, 128);
    osThreadDef(TouchTask, AppTouchTask, osPriorityNormal, 5, 128);
    osThreadDef(WirelessTask, AppWirelessTask, osPriorityNormal, 6, 512);
    osThreadDef(SuperviseTask, AppSuperviseTask, osPriorityNormal, 4, 256);
    osThreadDef(RetryTask, AppRetryTask, osPriorityNormal, 2, 128);
    osThreadDef(IWDG_Task, App_IWDG_Task, osPriorityNormal, 7, 64);


    /* �����¼���־�� */
    xIWDG_EventGroup = xEventGroupCreate();
    if(xIWDG_EventGroup == NULL)
    {
        printf("xIWDG_EventGroup Create Error");
    }

    NetCreateTaskHandle = osThreadCreate(osThread(NetCreateTask), NULL);
    if (NetCreateTaskHandle == NULL)
        printf("NetCreateTask Create Error");

    UartTaskHandle = osThreadCreate(osThread(UartTask), NULL);
    if (UartTaskHandle == NULL)
        printf("UartTask Create Error");

    LedTaskHandle = osThreadCreate(osThread(LedTask), NULL);
    if (LedTaskHandle == NULL)
        printf("LedTask Create Error");

    TouchTaskHandle = osThreadCreate(osThread(TouchTask), NULL);
    if (TouchTaskHandle == NULL)
        printf("TouchTask Create Error");

    WirelessTaskHandle = osThreadCreate(osThread(WirelessTask), NULL);
    if (WirelessTaskHandle == NULL)
        printf("WirelessTask Create Error");

    SuperviseTaskHandle = osThreadCreate(osThread(SuperviseTask), NULL);
    if (SuperviseTaskHandle == NULL)
        printf("SuperviseTask Create Error");

    RetryTaskHandle = osThreadCreate(osThread(RetryTask), NULL);
    if (RetryTaskHandle == NULL)
        printf("RetryTask Create Error");

    IWDG_TaskHandle = osThreadCreate(osThread(IWDG_Task), NULL);
    if (IWDG_TaskHandle == NULL)
        printf("IWDG_Task Create Error");
}
