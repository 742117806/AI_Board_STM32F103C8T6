//freeRTOS任务应用

#include "cmsis_os.h"
#include "taskApp.h"
#include "user_call_back.h"
#include "spi.h"
#include "delay.h"
#include "frame_process.h"
#include "led.h"
/* 
********************************************************************************************************** 
                      函数声明 
********************************************************************************************************** 
*/
extern void System_8msTick_Process(void);
extern void WireLess_Process(WLS *p_wl, DevicePara_TypDef *p_device);
extern void UartRx_Process(UpCom_Rx_TypDef *prx_ubuf, DevicePara_TypDef *p_device);
//extern void RxData_Process(HKFrame_TypDef *p_framebuf, DevicePara_TypDef *p_device);

extern void Led_Process(DevicePara_TypDef *p_device, Led_Color_TypeDef colour);
/* 
********************************************************************************************************** 
                       变量
********************************************************************************************************** 
*/

/* 任务句柄 */
osThreadId UartTaskHandle;
osThreadId LedTaskHandle;
osThreadId TouchTaskHandle;
osThreadId WirelessTaskHandle;
osThreadId SuperviseTaskHandle;

/* 任务事件变量*/
/* 
********************************************************************************************************* 
*  函 数 名:  AppSuperviseTask
*  功能说明:  
*  形    参: 无 
*  返 回 值: 无 
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
uint8_t frameSeq = 0; //帧序号

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
    vTaskSuspendAll(); //开启任务调度锁
    UartSendBytes(USART1, (uint8_t *)&frameCmd, frameLen);
    xTaskResumeAll(); //关闭任务调度锁
}


void AppUartTask(void const *argument)
{
    BaseType_t xResult;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10); //10ms
    uint32_t ulValue;
    uint16_t keyValue;

    for (;;)
    {
        xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, xMaxBlockTime); //等待按键事件
        if (xResult == pdPASS)
        {
            keyValue = ulValue & 0x000000ff;                         //获取按键键值
			if(keyValue > 0)
			{
				KeyUpReportCmd(keyValue & 0x0f, (keyValue & 0xf0) >> 4); //通过串口发送按键事件帧
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
*  函 数 名: AppLedTask 
*  功能说明: 
*  形    参: 无 
*  返 回 值: 无 
*    优 先 级: 1  (数值越小优先级越低，这个跟 uCOS相反) 
********************************************************************************************************* 
*/
void AppLedTask(void const *argument)
{
    BaseType_t xResult;

    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5); //5ms
    uint32_t ulValue;
    LedFunc_t *ledFunc;

    uint16_t LED_RunMode = 0;		//LED的运行模式
    uint8_t vol;					//LED的值

    uint8_t lastVaule;              //上一次led的值
    uint8_t vol_led_delay = 0;		//音量LED点亮延时
	uint16_t lastMode = 0;			//上一状态

    LED_CentreOn();

    for (;;)
    {

        xResult = xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulValue, xMaxBlockTime);
        if (xResult == pdPASS)
        {
            ledFunc = (LedFunc_t *)&ulValue;

            LED_RunMode = (uint16_t)ledFunc->cmdType | (ulValue & 0xff00);

            vol = ledFunc->ledColor;
            if ((LED_RunMode == 5) || (LED_RunMode == 0x0100) || (LED_RunMode == 0x0500)) //音量控制
            {
                vol_led_delay = 0;
            }
        }

        switch (LED_RunMode)
        {
        case 0:

            LED_AroundOff();
            lastMode = 0;       //睡眠
            LED_RunMode = 0x8000; //为了避免不停的写LED控制IC的寄存器
	

            break;
        case 1: //呼吸灯
            LED_Breath(LED_BLUE_MAP_BITS, 20, 35);
            lastMode = 1; //呼吸灯
            break;
        case 2:
            LED_AroundOn((LedColor_e)vol);
            lastMode = 2; //醒来
            lastVaule = vol;
            break;
        case 3: //流水灯模式
            lastMode = 3;
            LED_Flow((LedColor_e)vol, 15, 10);

            break;
        case 4: //中间灯点亮
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
        case 0x0100:   //按键1按下
            LED_Key1Show(RED_OR_ORANGE);
            break;
		case 0x0500:   //按键3按下
            LED_Key3Show(RED_OR_ORANGE);
            break;
        case 0x0200:   //按键1抬起
		case 0x0600:	//按键3抬起
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
*  函 数 名: AppTouchTask 
*  功能说明: 
*  形    参: 无 
*  返 回 值: 无 
*    优 先 级: 2   
********************************************************************************************************* 
*/
void AppTouchTask(void const *argument)
{
    uint8_t keyValue;
    uint8_t keyLast; //上次按键的值

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
            keyLast = keyValue; //更新上次值
        }

        osDelay(10);
    }
}

/* 
********************************************************************************************************* 
*  函 数 名: AppWirelessTask 
*  功能说明: 
*  形    参: 无 
*  返 回 值: 无 
*    优 先 级: 2   
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
*  函 数 名: AppTaskCreate 
*  功能说明: 创建应用任务 
*  形    参: 无 
*  返 回 值: 无 
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
