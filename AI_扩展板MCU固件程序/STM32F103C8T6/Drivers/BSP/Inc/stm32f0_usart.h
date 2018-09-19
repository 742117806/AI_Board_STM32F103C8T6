#ifndef __STM32F0_USART_H__
#define __STM32F0_USART_H__

//#include "stm32f0_peripheral.h"
//#include "m0_systick.h"
#include <string.h>
#include "stm32f1xx.h"

/************************* ���ڹ������Ժ궨�� ***************************/



////////////////////////////////Э��֡�ĺ궨��/////////////////////////////////////////////////
#define HKFreamHeader                               0xAC
#define HKFreamEnd                                  0x53
//#define Transpond_Data_Main                         0x10
//#define Transpond_Data_Slave                        0x90



#define Up_TimeOut_Val                              9     //250ByteԼ62.5ms


//HK������֡����
#define LogicAddr_Len                               4
#define FrameCs_Len                                 2

#define HKData_LenMax                               228          //���ݱ�ʶ4+����8MAX, �������߼���ַ֡����
#define HKFrame_LenMax                              (11+HKData_LenMax+16)     //ͷ1+��ַ4+�������1+������1+���ݳ���1 +У��2+β1+����Ԥ���ռ�
#define UpDate_Len                                  100

//֡������ķ��ʶ�λ
#define Region_HeaderNumber                 0          //֡ͷ
#define Region_AddrNumber                   1          //֡��ַ
#define Region_SeqNumber                    5          //֡�������
#define Region_CmdNumber                    6          //֡����
#define Region_DataLenNumber                7          //֡���ݳ���
#define Region_DataAFNNumber                8          //֡����AFN
#define Region_DataIDNumber                 9          //֡���ݱ�ʶ
#define Region_DataValNumber                12          //��Ϣ�ֱ�ʶ




//AES֡��ʽ
#define AESFreamHeader                            0x68

#define MACWrite_Cmd_Request                      0x20 
#define MACWrite_Yes_Response                     0x21
#define MACWrite_NO_Response                      0x22

#define MACRead_Cmd_Request                      0x30 
#define MACRead_Yes_Response                     0x31
#define MACRead_NO_Response                      0x32

#define Encode_Cmd_Request                        0xC0
#define Encode_Yes_Response                       0x41
#define Encode_No_Response                        0x42

#define Decode_Cmd_Request                        0xD0
#define Decode_Yes_Response                       0x51
#define Decode_No_Response                        0x52

#define GAWrite_Cmd_Request                       0xA0
#define GAWrite_Yes_Response                      0xA1
#define GAWrite_NO_Response                       0xA2

#define GKWrite_Cmd_Request                       0xB0
#define GKWrite_Yes_Response                      0xB1
#define GKWrite_NO_Response                       0xB2

#define Secret_Key_Yes           									0x66

#define None_GA_Key                               0xAE
#define None_GK_Key                               0xBE
#define None_GAK_Key                              0xCE


//AES������֡����
#define MAC_Data_Len                               8

//#define GK_Data_Len                               13
#define GK_Data_Len                              (16*2)
#define GA_Data_Len                               3

#define AesData_LenMax                            16
#define AesFrame_LenMax                           (5+AesData_LenMax)  //ͷ1+������1+���ݳ���1 +У��2


typedef enum
{
  Uart_NoError = 0,
  Uart_TxBusy,
  Uart_ParaError,
}Uart_ErrorType;



typedef enum
{
  UartRx_FrameHead = 0,      
  UartRx_FrameAddr,         
  UartRx_FrameSeq,          
  UartRx_DataCmd,           
  UartRx_Datalen,           
  UartRx_Data,              
  UartRx_FrameCs,           
  UartRx_FrameEnd,          
  
  UartRx_AesCmd,           
  UartRx_Aeslen,           
  UartRx_AesData,             
  UartRx_AesFrameCs1,          
  UartRx_AesFrameCs2,          

  UartRx_Finished,          //�������
}UART_RxSta_TypDef;


#define Tx_GapTime_Size             7


typedef enum
{
  UartTx_Finished = 0,  //�������
  UartTx_Start,         //��������
  UartTx_End,         //���ͽ���
}Uart_TxSta_TypDef;

#define UARTFrame_TxLen_MAX   MAXDOWNBUF - 32
typedef struct
{
  Uart_TxSta_TypDef Tx_Status;
  uint8_t Frame_Len;
  uint8_t Byte_Cnt;
  uint8_t Frame_Data[HKFrame_LenMax]; 
  uint8_t TxEnd_Cnt;
  //uint32_t TxGap_StartTime;
}UartFrame_TX_TypDef;


/*************************************************************/



typedef struct
{
  volatile UART_RxSta_TypDef Rx_Status;
  
  uint8_t FrameTotalLen;
  uint8_t Frame_Data[HKFrame_LenMax];
  volatile uint8_t Over_time_count;
}UpCom_Rx_TypDef;





/************************* �ⲿ���� ***************************/



extern UpCom_Rx_TypDef         UpCom_RxBuf;
extern UartFrame_TX_TypDef     UpCom_TxBuf;
extern uint8_t  Secret_GKey_Flag;     //��Կ��Կ���ڱ�־
extern uint8_t  Secret_KeyOk_Flag;     //��ԿOK��־

/************************* �ⲿ���� ***************************/



void UpUart_Init(void);
void UpUart_RX_INT_Process(uint8_t rx_data, UpCom_Rx_TypDef *pu_buf);
Uart_ErrorType UpUart_DataTx(uint8_t *p_buf, uint8_t len, UartFrame_TX_TypDef *pu_buf);
uint8_t UpUart_TX_INT_Process(UartFrame_TX_TypDef *pu_buf);


#endif

