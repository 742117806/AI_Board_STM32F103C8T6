//freeRTOS����Ӧ��

#include "cmsis_os.h"
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
/* 
********************************************************************************************************** 
                       ����
********************************************************************************************************** 
*/

/* ������ */
osThreadId UartTaskHandle;
osThreadId LedTaskHandle;
osThreadId TouchTaskHandle;
osThreadId WirelessTaskHandle;
osThreadId SuperviseTaskHandle;

/* �����¼�����*/
/* 
********************************************************************************************************* 
*  �� �� ��:  AppSuperviseTask
*  ����˵��:  
*  ��    ��: �� 
*  �� �� ֵ: �� 
********************************************************************************************************* 
*/

void AppSuperviseTask(void const *argument)
{
    for (;;)
    {
        System_8msTick_Process();
        osDelay(8);
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
    vTaskSuspendAll(); //�������������
    UartSendBytes(USART1, (uint8_t *)&frameCmd, frameLen);
    xTaskResumeAll(); //�ر����������
}


void AppUartTask(void const *argument)
{
    BaseType_t xResult;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10); //10ms
    uint32_t ulValue;
    uint16_t keyValue;

    for (;;)
    {
        xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, xMaxBlockTime); //�ȴ������¼�
        if (xResult == pdPASS)
        {
            keyValue = ulValue & 0x000000ff;                         //��ȡ������ֵ
			if(keyValue > 0)
			{
				KeyUpReportCmd(keyValue & 0x0f, (keyValue & 0xf0) >> 4); //ͨ�����ڷ��Ͱ����¼�֡
			}
			keyValue = ulValue & 0x0000ff00;                         
			keyValue >>= 8;
			if(keyValue == 0x10)
			{
			
			    RemoteDown_CmdFrame_Process(Wireless_Buf.Wireless_RxData, Wireless_Buf.Wireless_PacketLength);
			}
			else if(keyValue == 0x20)
			{
			    RemoteDown_EventFrame_Process(Wireless_Buf.Wireless_RxData, Wireless_Buf.Wireless_PacketLength, &HKFrame_Buf);
			}

        }


        UartRx_Process(&UpCom_RxBuf, &Device_ParaBuf);


        osDelay(1);
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

    uint16_t LED_RunMode = 0;		//LED������ģʽ
    uint8_t vol;					//LED��ֵ

    uint8_t lastVaule;              //��һ��led��ֵ
    uint8_t vol_led_delay = 0;		//����LED������ʱ
	uint16_t lastMode = 0;			//��һ״̬

    LED_CentreOn();

    for (;;)
    {

        xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, xMaxBlockTime);
        if (xResult == pdPASS)
        {
            ledFunc = (LedFunc_t *)&ulValue;

            LED_RunMode = (uint16_t)ledFunc->cmdType | (ulValue & 0xff00);

            vol = ledFunc->ledColor;
            if ((LED_RunMode == 5) || (LED_RunMode == 0x0100) || (LED_RunMode == 0x0500)) //��������
            {
                vol_led_delay = 0;
            }
        }

        switch (LED_RunMode)
        {
        case 0:

            LED_AroundOff();
            lastMode = 0;       //˯��
            LED_RunMode = 0x8000; //Ϊ�˱��ⲻͣ��дLED����IC�ļĴ���
	

            break;
        case 1: //������
            LED_Breath(LED_BLUE_MAP_BITS, 20, 35);
            lastMode = 1; //������
            break;
        case 2:
            LED_AroundOn((LedColor_e)vol);
            lastMode = 2; //����
            lastVaule = vol;
            break;
        case 3: //��ˮ��ģʽ
            lastMode = 3;
            LED_Flow((LedColor_e)vol, 15, 10);

            break;
        case 4: //�м�Ƶ���
            //LED_CentreOn();
	
            LED_LightCtrl(LED_CENTRE_MAP_BITS, 255);
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
        case 0x0100:   //����1����
            LED_Key1Show(RED_OR_ORANGE);
            break;
		case 0x0500:   //����3����
            LED_Key3Show(RED_OR_ORANGE);
            break;
        case 0x0200:   //����1̧��
		case 0x0600:	//����3̧��
			LED_RunMode = lastMode;
			vol = lastVaule;
            break;

        default:

            break;
        }

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
    uint8_t keyValue;
    uint8_t keyLast; //�ϴΰ�����ֵ

    for (;;)
    {

        keyValue = TouchKeyScan(0);
        if (keyValue > 0)
        {

            switch (keyValue)
            {
            case KEY1_PRES:

                xTaskNotify(LedTaskHandle, LED_INDEX_MODE_AROUND_KEY1_ON, eSetValueWithOverwrite);
                break;
            case KEY2_PRES:
                xTaskNotify(UartTaskHandle, keyValue, eSetValueWithOverwrite);
                break;
            case KEY3_PRES:
                xTaskNotify(LedTaskHandle, LED_INDEX_MODE_AROUND_KEY3_ON, eSetValueWithOverwrite);
                break;
            case KEY4_PRES:
                xTaskNotify(UartTaskHandle, keyValue, eSetValueWithOverwrite);
                break;
            case KEY1_LONG_PRES:
                xTaskNotify(UartTaskHandle, keyValue, eSetValueWithOverwrite);
                break;
            case KEY_UP:
                if (keyLast == KEY1_PRES)
                {
                    xTaskNotify(UartTaskHandle, keyLast, eSetValueWithOverwrite);
                    xTaskNotify(LedTaskHandle, 0x0200, eSetValueWithOverwrite);
                }
                else if (keyLast == KEY3_PRES)
                {
                    xTaskNotify(UartTaskHandle, keyLast, eSetValueWithOverwrite);
                    xTaskNotify(LedTaskHandle, 0x0600, eSetValueWithOverwrite);
                }

                break;
            default:

                break;
            }
            keyLast = keyValue; //�����ϴ�ֵ
        }

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
    for (;;)
    {
        WireLess_Process(&Wireless_Buf, &Device_ParaBuf);
        osDelay(8);
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
    osThreadDef(UartTask, AppUartTask, osPriorityNormal, 2, 128);
    osThreadDef(LedTask, AppLedTask, osPriorityNormal, 0, 128);
    osThreadDef(TouchTask, AppTouchTask, osPriorityNormal, 1, 128);
    osThreadDef(WirelessTask1, AppWirelessTask, osPriorityNormal, 3, 512);
    osThreadDef(SuperviseTask, AppSuperviseTask, osPriorityNormal, 4, 64);

    UartTaskHandle = osThreadCreate(osThread(UartTask), NULL);
    LedTaskHandle = osThreadCreate(osThread(LedTask), NULL);
    TouchTaskHandle = osThreadCreate(osThread(TouchTask), NULL);
    WirelessTaskHandle = osThreadCreate(osThread(WirelessTask1), NULL);
    SuperviseTaskHandle = osThreadCreate(osThread(SuperviseTask), NULL);
}
