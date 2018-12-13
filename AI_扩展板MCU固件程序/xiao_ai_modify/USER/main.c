
#include "includes.h"

 
 
/*********************任务相关定义和声明************************/

#define START_TASK_PRIO		1 			//任务优先级
#define START_STK_SIZE 		320 		//任务堆栈大小	 
TaskHandle_t StartTask_Handler;    		//任务句柄
void vStartTask(void *pvParameters);   	//任务函数

#define LED_TASK_PRIO		2 			//任务优先级
#define LED_STK_SIZE 		128 		//任务堆栈大小	 
TaskHandle_t LedTask_Handler;    		//任务句柄
void vLedTask(void *pvParameters);   	//任务函数

#define NET_TASK_PRIO		3 			//任务优先级
#define NET_STK_SIZE 		256 		//任务堆栈大小	 
TaskHandle_t NetTask_Handler;    		//任务句柄
void vNetTask(void *pvParameters);   	//任务函数

#define WIRELESS_RX_TASK_PRIO		4 			//任务优先级
#define WIRELESS_RX_STK_SIZE 		256 		//任务堆栈大小	 
TaskHandle_t WirelessRxTask_Handler;    		//任务句柄
void vWirelessRxTask(void *pvParameters);   	//任务函数

#define WIRELESS_TX_TASK_PRIO		5 			//任务优先级
#define WIRELESS_TX_STK_SIZE 		256 		//任务堆栈大小	 
TaskHandle_t WirelessTxTask_Handler;    		//任务句柄
void vWirelessTxTask(void *pvParameters);   	//任务函数



 
/************************************************

************************************************/
/* 创建开始任务 */
void StartTaskCreate(void)
{
	//创建开始任务
    xTaskCreate((TaskFunction_t )vStartTask,            //任务函数
                (const char*    )"vStartTask",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄   
}
/*创建任务*/

void AppTaskCreate(void)
{
 

	//创建LED任务
    xTaskCreate((TaskFunction_t )vLedTask,            //任务函数
                (const char*    )"vLedTask",          //任务名称
                (uint16_t       )LED_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                //传递给任务函数的参数
                (UBaseType_t    )LED_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&LedTask_Handler);   //任务句柄 

	//创建KEY任务
    xTaskCreate((TaskFunction_t )vNetTask,            //任务函数
                (const char*    )"vNetTask",          //任务名称
                (uint16_t       )NET_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                //传递给任务函数的参数
                (UBaseType_t    )NET_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&NetTask_Handler);   //任务句柄  

	//创建无线发射任务
    xTaskCreate((TaskFunction_t )vWirelessRxTask,            //任务函数
                (const char*    )"vWirelessRxTask",          //任务名称
                (uint16_t       )WIRELESS_RX_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                		//传递给任务函数的参数
                (UBaseType_t    )WIRELESS_RX_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&WirelessRxTask_Handler);   //任务句柄  

	//创建无线发射任务
    xTaskCreate((TaskFunction_t )vWirelessTxTask,            //任务函数
                (const char*    )"vWirelessTxTask",          //任务名称
                (uint16_t       )WIRELESS_TX_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                		//传递给任务函数的参数
                (UBaseType_t    )WIRELESS_TX_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&WirelessTxTask_Handler);   //任务句柄  	
				
}
/* 创建邮箱 */
void AppQueueCreate(void)
{
	xQueueWirelessTx = xQueueCreate(5, sizeof(QUEUE_WIRELESS_SEND_t)); //队列长度3个
	
	xQueueLedTask = xQueueCreate(10, sizeof(uint8_t)); 			//10个队列，每个队列长度为uint8_t
	
	xQueueNetTask = xQueueCreate(3, sizeof(QUEUE_WIRELESS_SEND_t)); 
	
}

/* 创建互斥信号量*/
void AppSemaphoreCreate(void)
{

	xSemWireless = xSemaphoreCreateMutex();
	xSemaphoreGive(xSemWireless);
}


/* 
********************************************************************************************************* 
*  函 数 名: vSystemLogoDisplay 
*  功能说明: 打印系统相关信息
*  形    参: 无
*  返 回 值: 无  
********************************************************************************************************* 
*/ 
void vSystemLogoDisplay(void)
{
   DebugPrintf("\n*******************FeerRTOS 工程*********************\n");
   DebugPrintf("日期：%s %s \n",__DATE__,__TIME__);
}
/* 
********************************************************************************************************* 
*  函 数 名: main 
*  功能说明: 
*  形    参: 无
*  返 回 值: 无  
********************************************************************************************************* 
*/ 
int main(void)
{		
	delay_init(72);	    //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置NVIC中断分组4:0位抢占优先级

	StartTaskCreate();
		
	vTaskStartScheduler();          //开启任务调度
}



/* 
********************************************************************************************************* 
*  函 数 名: vTaskStart 
*  功能说明: 
*  形    参: pvParameters 是在创建该任务时传递的形参 
*  返 回 值: 无 
*  优 先 级: 4   
********************************************************************************************************* 
*/ 
static void vStartTask(void *pvParameters) 
{ 

	vUart1Init(38400);
	vUart2Init(38400);
	vSystemLogoDisplay();			//打印LOGO信息
	vKeyInit();						//按键初始化
	SN3218_Init();
	vAppWirelessInit();
	vDeviceInfoInit();
	AES_Init();                     //必须放到vDeviceInifoInit()后面
	//LowPowerDeviceInit();			//必须放到vDeviceInifoInit()后面  ，调用死机
	AppQueueCreate();               //创建邮箱
	AppSemaphoreCreate();
	AppTaskCreate();				//创建任务	
    
    while(1) 
    { 
		vUartFrameProcess(&sUart1Rx);
		
		vTaskDelay(10);
    } 
	
} 


/* 
********************************************************************************************************* 
*  函 数 名: vLedTask 
*  功能说明: 控制面板按键和LED灯任务
*  形    参: pvParameters 是在创建该任务时传递的形参 
*  返 回 值: 无 
*  优 先 级: 4   
********************************************************************************************************* 
*/ 
static void vLedTask(void *pvParameters) 
{ 
    eKEY_VALUE key_now = KEY_NONE;  //当前按键
   
	 	 
    LED_AroundStaSet(LED_COLOR_ALL,ON);
    while(1) 
    { 
		vLedProcess();
		
        key_now = eKeyScan(0);			//按键扫描
		vTouchProcess(key_now);         //执行按键相应功能
		if(key_now == KEY_ALL_REL)
		{
			vLedCtrlModeLoop(led_set_mode);
		}
		
        vTaskDelay(25); 		
    } 
} 

/* 
********************************************************************************************************* 
*  函 数 名: vKeyTask 
*  功能说明: 
*  形    参: pvParameters 是在创建该任务时传递的形参 
*  返 回 值: 无 
*  优 先 级: 4   
********************************************************************************************************* 
*/ 
static void vNetTask(void *pvParameters) 
{ 
	BaseType_t xResult;
    QUEUE_WIRELESS_SEND_t queueMsg;
	uint8_t route_flag = 0;//需要进行路由配网

	
    while(1) 
    { 
		xResult = xQueueReceive(xQueueNetTask,    	/* 消息队列句柄 */
								(void *)&queueMsg, 		/* 存储接收到的数据到变量queueMsg */
								(TickType_t)30);        	/* 设置阻塞时间5个tick */
		if (xResult == pdPASS) 							/* 成功接收，并通过串口将数据打印出来 */
        {
			DebugPrintf("\n接收到无线配网数据");
		    //vRouteFrameMatchProcess(&deviceInfo.match,&queueMsg);
			
			xQueueSend(xQueueWirelessTx,&queueMsg, (TickType_t)10);			//发到无线发送任务		
		}
		else
		{
		    if(route_flag == 1)
			{
				DebugPrintf("\n接收超时，执行路由配网");
			}
		}
        vTaskDelay(1); 
    } 
} 
/* 
********************************************************************************************************* 
*  函 数 名: vWirelessTxTask 
*  功能说明: 
*  形    参: pvParameters 是在创建该任务时传递的形参 
*  返 回 值: 无 
*  优 先 级: 4   
********************************************************************************************************* 
*/
static void vWirelessTxTask(void *pvParameters) 
{ 
	BaseType_t xResult;
    QUEUE_WIRELESS_SEND_t queueMsg;
    uint8_t frame_len;
    while(1) 
    { 
		xResult = xQueueReceive(xQueueWirelessTx,    	/* 消息队列句柄 */
								(void *)&queueMsg, 		/* 存储接收到的数据到变量queueMsg */
								(TickType_t)5);        	/* 设置阻塞时间5个tick */
		if (xResult == pdPASS) 							/* 成功接收，并通过串口将数据打印出来 */
        {
			DebugPrintf("\n无线数据邮箱接收成功");
				//74编码
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
*  函 数 名: vWirelessTxTask 
*  功能说明: 
*  形    参: pvParameters 是在创建该任务时传递的形参 
*  返 回 值: 无 
*  优 先 级: 4   
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
 *****************下面注视的代码是通过调用库函数来实现IO控制的方法*****************************************
int main(void)
{ 
 
	delay_init();		  //初始化延时函数
	LED_Init();		        //初始化LED端口
	while(1)
	{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);  //LED0对应引脚GPIOB.5拉低，亮  等同LED0=0;
			GPIO_SetBits(GPIOE,GPIO_Pin_5);   //LED1对应引脚GPIOE.5拉高，灭 等同LED1=1;
			delay_ms(300);  		   //延时300ms
			GPIO_SetBits(GPIOB,GPIO_Pin_5);	   //LED0对应引脚GPIOB.5拉高，灭  等同LED0=1;
			GPIO_ResetBits(GPIOE,GPIO_Pin_5); //LED1对应引脚GPIOE.5拉低，亮 等同LED1=0;
			delay_ms(300);                     //延时300ms
	}
} 
 
 ****************************************************************************************************
 ***/
 

	
/**
*******************下面注释掉的代码是通过 直接操作寄存器 方式实现IO口控制**************************************
int main(void)
{ 
 
	delay_init();		  //初始化延时函数
	LED_Init();		        //初始化LED端口
	while(1)
	{
     GPIOB->BRR=GPIO_Pin_5;//LED0亮
	   GPIOE->BSRR=GPIO_Pin_5;//LED1灭
		 delay_ms(300);
     GPIOB->BSRR=GPIO_Pin_5;//LED0灭
	   GPIOE->BRR=GPIO_Pin_5;//LED1亮
		 delay_ms(300);

	 }
 }
**************************************************************************************************
**/

