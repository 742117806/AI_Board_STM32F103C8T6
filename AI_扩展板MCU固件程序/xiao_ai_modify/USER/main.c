
#include "includes.h"



/*********************任务相关定义和声明************************/

#define START_TASK_PRIO		6 			//任务优先级
#define START_STK_SIZE 		192 		//任务堆栈大小	 
TaskHandle_t StartTask_Handler;    		//任务句柄
void vStartTask(void *pvParameters);   	//任务函数

#define LED_TASK_PRIO		2 			//任务优先级
#define LED_STK_SIZE 		64 		//任务堆栈大小	 
TaskHandle_t LedTask_Handler;    		//任务句柄
void vLedTask(void *pvParameters);   	//任务函数

#define NET_TASK_PRIO		5 			//任务优先级
#define NET_STK_SIZE 		192 		//任务堆栈大小	 
TaskHandle_t NetTask_Handler;    		//任务句柄
void vNetTask(void *pvParameters);   	//任务函数

#define WIRELESS_RX_TASK_PRIO		4 			//任务优先级
#define WIRELESS_RX_STK_SIZE 		192 		//任务堆栈大小	 
TaskHandle_t WirelessRxTask_Handler;    		//任务句柄
void vWirelessRxTask(void *pvParameters);   	//任务函数

#define WIRELESS_TX_TASK_PRIO		3 			//任务优先级
#define WIRELESS_TX_STK_SIZE 		256 		//任务堆栈大小	 
TaskHandle_t WirelessTxTask_Handler;    		//任务句柄
void vWirelessTxTask(void *pvParameters);   	//任务函数


//#define WIRELESS_ROUTE_TASK_PRIO		1 			//任务优先级
//#define WIRELESS_ROUTE_STK_SIZE 		192 		//任务堆栈大小	 
//TaskHandle_t WirelessRouteTask_Handler;    			//任务句柄
//void vWirelessRouteTask(void *pvParameters);   		//任务函数

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
    BaseType_t xReturn;

    //创建LED任务
    xReturn = xTaskCreate((TaskFunction_t )vLedTask,            //任务函数
                          (const char*    )"vLedTask",          //任务名称
                          (uint16_t       )LED_STK_SIZE,        //任务堆栈大小
                          (void*          )NULL,                //传递给任务函数的参数
                          (UBaseType_t    )LED_TASK_PRIO,       //任务优先级
                          (TaskHandle_t*  )&LedTask_Handler);   //任务句柄
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvLedTask创建失败");
    }

    //创建KEY任务
    xReturn =  xTaskCreate((TaskFunction_t )vNetTask,            //任务函数
                           (const char*    )"vNetTask",          //任务名称
                           (uint16_t       )NET_STK_SIZE,        //任务堆栈大小
                           (void*          )NULL,                //传递给任务函数的参数
                           (UBaseType_t    )NET_TASK_PRIO,       //任务优先级
                           (TaskHandle_t*  )&NetTask_Handler);   //任务句柄
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvNetTask创建失败");
    }

    //创建无线发射任务
    xReturn =  xTaskCreate((TaskFunction_t )vWirelessRxTask,            //任务函数
                           (const char*    )"vWirelessRxTask",          //任务名称
                           (uint16_t       )WIRELESS_RX_STK_SIZE,        //任务堆栈大小
                           (void*          )NULL,                		//传递给任务函数的参数
                           (UBaseType_t    )WIRELESS_RX_TASK_PRIO,       //任务优先级
                           (TaskHandle_t*  )&WirelessRxTask_Handler);   //任务句柄
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvWirelessRxTask创建失败");
    }

    //创建无线发射任务
    xReturn =  xTaskCreate((TaskFunction_t )vWirelessTxTask,            //任务函数
                           (const char*    )"vWirelessTxTask",          //任务名称
                           (uint16_t       )WIRELESS_TX_STK_SIZE,        //任务堆栈大小
                           (void*          )NULL,                		//传递给任务函数的参数
                           (UBaseType_t    )WIRELESS_TX_TASK_PRIO,       //任务优先级
                           (TaskHandle_t*  )&WirelessTxTask_Handler);   //任务句柄
    if(xReturn != pdPASS)
    {
        DebugPrintf("\nvWirelessTxTask创建失败");
    }

    //创建无线路由任务
//    xReturn =  xTaskCreate((TaskFunction_t )vWirelessRouteTask,            //任务函数
//                           (const char*    )"vWirelessRouteTask",          //任务名称
//                           (uint16_t       )WIRELESS_ROUTE_STK_SIZE,        //任务堆栈大小
//                           (void*          )NULL,                		//传递给任务函数的参数
//                           (UBaseType_t    )WIRELESS_ROUTE_TASK_PRIO,       //任务优先级
//                           (TaskHandle_t*  )&WirelessRouteTask_Handler);   //任务句柄
//    if(xReturn != pdPASS)
//    {
//        printf("\nvWirelessRouteTask创建失败");
//    }

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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组4:0位抢占优先级

    StartTaskCreate();

    vTaskStartScheduler();          //开启任务调度
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    DebugPrintf("\n任务: %s 发现栈溢出",pcTaskName);
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
    vUart2Init(9600);
    vSystemLogoDisplay();			//打印LOGO信息
    vKeyInit();						//按键初始化
    SN3218_Init();
    vDeviceInfoInit();
    vAppWirelessInit();
    AES_Init();                     //必须放到vDeviceInifoInit()后面
    LowPowerDeviceInit();			//必须放到vDeviceInifoInit()后面
    AppQueueCreate();               //创建邮箱
    AppSemaphoreCreate();
    AppTaskCreate();				//创建任务
    IWDG_Init(4,625);    //与分频数为64,重载值为625,溢出时间为1s
    while(1)
    {
        vUartFrameProcess(&sUart1Rx);
		IWDG_Feed();
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


    //LED_AroundStaSet(LED_COLOR_ALL,OFF);
	LED_ALL_StaSet(OFF);
    while(1)
    {
        vLedProcess();

        key_now = eKeyScan(0);			//按键扫描
        vTouchProcess(key_now);         //执行按键相应功能
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
    uint8_t device_index = 0;
    uint8_t routerTable[3]= {0xBB,0xCC,0xDD};  //路由表
    uint8_t frameTemp[39] = {0};               //设备配网应用协议帧缓存
    uint8_t heart_index = 0;
    uint8_t heart_delay1 =  0;
    uint16_t heart_delay2 = 0;

    while(1)
    {
        xResult = xQueueReceive(xQueueNetTask,    	/* 消息队列句柄 */
                                (void *)&queueMsg, 		/* 存储接收到的数据到变量queueMsg */
                                (TickType_t)30);        	/* 设置阻塞时间5个tick */
        if (xResult == pdPASS) 							/* 成功接收，并通过串口将数据打印出来 */
        {
            DebugPrintf("\n接收到无线配网数据");
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
                    DebugPrintf("\n进行路由配网");

                    if(deviceInfo.match.deviceBuff[device_index]!= frameTemp[Region_AddrNumber])
                    {
                        routerTable[0] = deviceInfo.match.deviceBuff[device_index];


                        queueMsg.len = FrameRouterCompose_ext( &frameTemp[Region_DataValNumber], //配网设备的MAC
                                                               frameTemp,                                        //配网命令数据
                                                               frameTemp[Region_DataLenNumber]+11,              //配网命令长度
                                                               queueMsg.msg,                                 //缓存配网命令的邮箱
                                                               routerTable,                                            //路由表
                                                               1);                                            //路由表长度
                        xQueueSend(xQueueWirelessTx,&queueMsg, (TickType_t)10);			//发到无线发送任务
                        vTaskDelay(500);
                    }
                }
                else
                {
                    DebugPrintf("\n路由设备已经查找完毕");
                    route_flag = 0;
                    device_index = 0;
                }
                device_index++;
            }
            else
            {
                heart_delay2 ++;
                if(heart_delay2 > 1200)	//120秒
                {
                    heart_delay1 ++;
                    if(heart_delay1 > 30)
                    {
                        heart_delay1 = 0;

                        if(heart_index < deviceInfo.match.deviceNum)
                        {
                            DebugPrintf("\n查询设备心跳");
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

