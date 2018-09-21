//freeRTOS任务应用

#include "cmsis_os.h"
#include "taskApp.h"
#include "user_call_back.h"
#include "spi.h"
#include "delay.h"
#include "frame_process.h"
/* 
********************************************************************************************************** 
                      函数声明 
********************************************************************************************************** 
*/
extern void System_8msTick_Process(void);
extern void WireLess_Process(WLS *p_wl, DevicePara_TypDef *p_device);
extern void UpCom_Process(UpCom_Rx_TypDef *prx_ubuf, DevicePara_TypDef *p_device);
extern void RxData_Process(HKFrame_TypDef *p_framebuf, DevicePara_TypDef *p_device);
extern void Touch_Process(DevicePara_TypDef *p_device);
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
/* 
********************************************************************************************************* 
*  函 数 名:  AppSuperviseTask
*  功能说明:  
*  形    参: 无 
*  返 回 值: 无 
********************************************************************************************************* 
*/

void AppSuperviseTask(void const * argument)
{
   	for(;;)
	{
		System_8msTick_Process();
		osDelay(8);
	}
}
/* StartDefaultTask function */
void AppUartTask(void const * argument)
{

	/* USER CODE BEGIN 5 */

	/* Infinite loop */
	for(;;)
	{
		//DEBUG_Printf("StartDefaultTask"); 



        
        UpCom_Process(&UpCom_RxBuf, &Device_ParaBuf);

        RxData_Process(&HKFrame_Buf, &Device_ParaBuf);
        
        if (UpCom_TxBuf.Tx_Status == UartTx_Finished)
        {
            UpReport_Process(&HKFrame_Buf, &Device_ParaBuf);
        }
		osDelay(2);
	}
	/* USER CODE END 5 */ 
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
void AppLedTask(void const * argument)
{
	for(;;)
	{
	   //DEBUG_Printf("\r\nAppLedTask");
	   Led_Process(&Device_ParaBuf, Orange);
	   osDelay(8);
	   
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
void AppTouchTask(void const * argument)
{
	for(;;)
	{
	   //DEBUG_Printf("\r\nAppTouchTask");
	   Touch_Process(&Device_ParaBuf);
	   osDelay(8);
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
void AppWirelessTask(void const * argument)
{	
	for(;;)
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
void  AppTaskCreate(void)
{                    
	osThreadDef(UartTask, AppUartTask, osPriorityNormal, 2, 128);
	osThreadDef(LedTask,AppLedTask, osPriorityNormal, 0, 128);
	osThreadDef(TouchTask,AppTouchTask, osPriorityNormal, 1, 128);
	osThreadDef(WirelessTask1,AppWirelessTask, osPriorityNormal, 3, 512);
	osThreadDef(SuperviseTask,AppSuperviseTask,osPriorityNormal,4,128);
	
	UartTaskHandle = osThreadCreate(osThread(UartTask), NULL);
	LedTaskHandle = osThreadCreate(osThread(LedTask), NULL); 
	TouchTaskHandle =  osThreadCreate(osThread(TouchTask),NULL);
	WirelessTaskHandle = osThreadCreate(osThread(WirelessTask1),NULL);
	SuperviseTaskHandle =  osThreadCreate(osThread(SuperviseTask),NULL);
}

