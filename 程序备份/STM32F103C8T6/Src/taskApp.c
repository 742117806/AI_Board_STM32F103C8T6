//RTX����Ӧ��

#include "taskApp.h"
#include "RTL.h"
#include "user_call_back.h"
#include "spi.h"
#include "delay.h"
/* 
********************************************************************************************************** 
                      �������� 
********************************************************************************************************** 
*/

static void AppTaskCreate(void);
__task void AppTaskLED(void);
__task void AppTaskStart(void);

/* 
********************************************************************************************************** 
                       ����
********************************************************************************************************** 
*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
static uint64_t AppTaskLEDStk[256 / 8];   /* ����ջ */
static uint64_t AppTaskStartStk[512 / 8]; /* ����ջ */

/* ������ */
OS_TID HandleTaskLED = NULL;

/* 
********************************************************************************************************* 
*  �� �� ��: RTX_OS_Init 
*  ����˵��: ����RTXϵͳ
*  ��    ��: �� 
*  �� �� ֵ: �� 
********************************************************************************************************* 
*/

void RTX_OS_Init(void)
{
    /* ������������ */
    os_sys_init_user(AppTaskStart,             /* ������ */
                     2,                        /* �������ȼ� */
                     &AppTaskStartStk,         /* ����ջ */
                     sizeof(AppTaskStartStk)); /* ����ջ��С����λ�ֽ��� */
}
/* 
********************************************************************************************************* 
*  �� �� ��: AppTaskLED 
*  ����˵��: LED��˸  
*  ��    ��: �� 
*  �� �� ֵ: �� 
*    �� �� ��: 1  (��ֵԽС���ȼ�Խ�ͣ������ uCOS�෴) 
********************************************************************************************************* 
*/
extern TIM_HandleTypeDef htim1;
extern uint32_t time1_delay;
extern uint32_t delay_flag;
extern void Time1Delay_us(uint32_t n_us);
__task void AppTaskLED(void)
{
	uint8_t flag = 0;
	
//	SN3218_Init();
//	PowerOn_Led();
    //extern SPI_HandleTypeDef hspi1;
   // uint8_t wireless_init_result;

    //wireless_init_result = Wireless_Init();
    //DEBUG_Printf("Wireless_Init() = %d\r\n", wireless_init_result);
    //Si4438_Receive_Start(Wireless_Channel[0]);	//��ʼ������������
    while (1)
    {
		
       // LEDR_TOGGLE();
	    //TouchLED_ALL(flag^1);
		delay_ms(30);
        os_dly_wait(110);
    }
}

/* 
********************************************************************************************************* 
*  �� �� ��: AppTaskStart 
*  ����˵��: ��������Ҳ����������ȼ����� 
*  ��    ��: �� 
*  �� �� ֵ: �� 
*    �� �� ��: 2   
********************************************************************************************************* 
*/
__task void AppTaskStart(void)
{

    //Wireless_Init();
    uint16_t len, t;
    AppTaskCreate();

    while (1)
    {

        //LEDR_TOGGLE();

        if (USART2_RX_STA & 0x8000)
        {
            len = USART2_RX_STA & 0x3fff; //�õ��˴ν��յ������ݳ���
            //printf("\r\n�����͵���ϢΪ:\r\n\r\n");
            for (t = 0; t < len; t++)
            {
                UartSendData(USART2, USART2_RX_BUF[t]); //�򴮿�1��������
            }
            //printf("\r\n\r\n");//���뻻��
            USART2_RX_STA = 0;
        }
        if (USART_RX_STA & 0x8000)
        {
            len = USART_RX_STA & 0x3fff; //�õ��˴ν��յ������ݳ���
            //printf("\r\n�����͵���ϢΪ:\r\n\r\n");
            for (t = 0; t < len; t++)
            {
                UartSendData(USART1, USART_RX_BUF[t]); //�򴮿�1��������
				
            }
			//Si4438_Transmit_Start(&Wireless_Buf,22,USART_RX_BUF,len);
            //printf("\r\n\r\n");//���뻻��
            USART_RX_STA = 0;
        }
        os_dly_wait(1);
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
static void AppTaskCreate(void)
{
    HandleTaskLED = os_tsk_create_user(AppTaskLED,             /* ������ */
                                       1,                      /* �������ȼ� */
                                       &AppTaskLEDStk,         /* ����ջ */
                                       sizeof(AppTaskLEDStk)); /* ����ջ��С����λ�ֽ��� */
}
