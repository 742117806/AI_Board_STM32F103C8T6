#ifndef FRAME_PROCESS_H
#define FRAME_PROCESS_H

//#include "peri_includes.h"
//#include <stdlib.h>

#include "wireless_drv.h"
#include "led.h"
#include "crc16.h"
#include "wireless_app.h"
#include "touch.h"
#include "rsa.h"
#include "deviceInfo.h"
#ifdef Use_Rout
#include "rout.h"
#endif

//�ô����չ�Կ���壬�����ڶ�����ע��
//#define USE_USAT_RSAKey

//////////////////////// �궨�� //////////////////////////////////
/*************** *************** �汾�������¼ ****************************************/
/*
Rev_0100  ���Ӱ汾��Уʱ��Ϣ֡ 2018/2/3_10��
Rev_0101  �ظ�ʱ�����ݾ���Ҫ�������ݱ�ʶ 2018/2/3_16��
Rev_0102  ��ͬ��Ⱥ����ò�ͬ��ͨѶƵ�� 2018/2/4_16��
Rev_0103  ��������֡,����֡��ʱ�ӳ�8S 2018/2/8_19��
Rev_0104  ��ʱ����Ĭ�ϵ�����Ƶ�� 2018/2/26_15��
Rev_0105  �޸�����֡�������õ�bug 2018/2/27_11��
Rev_0106  �������֡�ĺϷ��� 2018/3/1_13��
Rev_0107  �޸��쳣�ظ�֡����쳣��־��Bug������������bug,����ϵͳʱ������ 2018/3/3_21��
Rev_0108  ���Ӽӽ��ܲ���֡�����ڵ��Ժͺ˶� 2018/3/5_21��
Rev_0109  1.����֡��ΪֻҪ�·��ͻ�ȷ�ϣ�T1��2���Ϊ20�� 2.2018/3/8_22��
Rev_0110  �µ���չ���д���2��Ϊ�ô���1ͨ��
Rev_0111  ��Կ�ڳ����ж���Ϊ�������޸��ظ�֡��������־��bug
Rev_0112  ���쳣֡�У��ϱ�ʱ�����еĵ�ַ��Ϣ     2018/3/26
Rev_0113  ������չ����MAC��ַ���ܲ�������Ƶ���ģʽ;·��ʱ�ᴦ����������֡       2018/4/26_11:30��2018/5/12_16:10
Rev_0214  ��Ƶ�汾������·�ɱ�洢�ռ��С      2018/6/20_11:00��2018/6/29_17:40
Rev_0215  ������չ��74��������       2018/07/09_15:00
Rev_0216  ���������ŵ����м�⣬���ĵĴ洢�����޸�       2018/07/13_18:35��2018/07/17_18:20
Rev_0217  �����������                2018/08/13_17:00
Rev_0218  �޸�����ͨ��ʱMCU����Ӧ�жϣ����ջ���ֹ��bug������TX�ķ�������    2018/08/22_11:30
Rev_0301  ʹ��STM32F103C8T6 ��������ǰһ�汾
*/

#define Version_Number 0x0303

/***********************************************************************/
#define Devece_Attrs 1 //����Ϊ1�� �ڵ�Ϊ0

#ifdef Use_74dcode //������74���뷽ʽ
#include "74.h"
#endif

#if (Devece_Attrs)
#define Frame_RetryTimes 2000 //��λΪmS
#else
#define Frame_RetryTimes 500 //��λΪmS
#endif

#define LED_MODE1 0x10
#define LED_MODE2 0x20
#define LED_MODE3 0x30
#define LED_MODE4 0x40

//������
#define Key_NotReady 0x53    //��Կδ����
#define LANG_Addr_Error 0x54 //ת�������ݣ�Ⱥ���ַ����

#define Cipher_Error 0X04    //
#define Parameter_Error 0x05 //Ⱥ���ַ�����Ĳ�һ��
#define Format_Error 0X06    //AES��ʽ����
#define Data_Error 0X07      //Ⱥ���ַ��һ��

#define FrameFunction_Exist 0x57 //֡�����Ѵ���

#define Secret_Key_Yes 0x66

///////////////////////////////////////////////////////////////////////
//�����ϱ�֡
#define Local_UpReportCmd 0x88  //�����ϱ�֡
#define Remote_UpReportCmd 0x98 //�ڵ��ϱ�֡

//���յ���֡��0X98λ�����ж�
#define Local_CmdFrame 0x00        //��������֡
#define Local_EventFrame 0x08      //�����¼�֡
#define RemoteUp_CmdFrame 0x10     //����Զ������֡
#define RemoteUp_EventFrame 0x18   //����Զ���¼�֡
#define RemoteDown_CmdFrame 0x90   //����Զ������֡
#define RemoteDown_EventFrame 0x98 //����Զ���¼�֡

//�ϱ��¼���־(��ͬ��λ��ʾ)

#define Motor_Event 0u       //�������״̬����
#define PIR_Event 1u         //�����Ӧ�������
#define Touch_Event 2u       //������Ӧ�������
#define Floater_Event 3u     //���ӵ�λ���Ѹı�
#define BatteryLow_Event 8u  //������ص͵���
#define BatteryFull_Event 9u //�������������

#define Key_Event 1u    //�����¼�
#define Switch_Event 2u //�����¼�

#define Retry_Buf_Size 8 //�ϱ�BUF����

//////////////////////////////////////////////////////////////////////
typedef struct
{
  volatile uint8_t RetryCnt; //��Ϊ0ʱ����ʾ��Ҫ���ԣ�����������ʱ��
  uint8_t Retry_SendLen;
  uint8_t RetryBuf_Number; //��ʾ�õ��ڼ����ռ�
  uint8_t Retry_DataBuf[HKFrame_LenMax];
  uint32_t Retry_StartTime;
} Retry_TypDef;

typedef struct
{
  Retry_TypDef Retry_Buf[Retry_Buf_Size];
  uint8_t RetryBuf_Space; //�ڼ�bit����ڼ����ռ䣬0Ϊ���ã�1Ϊ��ռ��
  //uint8_t FrameConfig;    //
  uint8_t Upreport_FrameSeq;
  //uint8_t FrameCmd;
  uint8_t FrameProcess_Buf[HKFrame_LenMax];

} HKFrame_TypDef;

typedef struct
{
  uint8_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hour;
  uint8_t week;
  uint8_t minute;
  uint8_t second;
} BeiJingTime_TypDef;

/************************* �ⲿ���� ***************************/
extern DevicePara_TypDef Device_ParaBuf;
extern HKFrame_TypDef HKFrame_Buf;
extern uint8_t LANGroup_Addr[3];
extern uint8_t Local_MAC_Addr[MAC_Data_Len + 5];
extern BeiJingTime_TypDef RunningTime_Buf;
extern uint8_t const Self_LogicAddr[4];
/************************* �ⲿ���� ***************************/

uint8_t FrameData_Detect(uint8_t *p, uint8_t len); //ָ�����ݵ���ʼ��ַ�����ݼ��
uint8_t RealData_Detect(uint8_t *p, uint8_t len, uint8_t Frame_len);

uint8_t Frame_Check(uint8_t *p, uint8_t Len);
uint8_t Frame_Compose(uint8_t *p);
uint8_t MACRead_Process(uint8_t *p_buf);
void Frame_Process(uint8_t *p_source, uint8_t len, HKFrame_TypDef *p_framebuf, DevicePara_TypDef *p_fashionpara);

uint8_t Encrypt_Convert(uint8_t *p_source, uint8_t len, uint8_t Convert_Mode);
void Retransmission_Process(HKFrame_TypDef *p_framebuf);

void Rsa_Decode(uint8_t *p_minw);
void RemoteDown_EventFrame_Process(uint8_t *p_source, uint8_t len, HKFrame_TypDef *p_framebuf);
void RemoteDown_CmdFrame_Process(uint8_t *p_source, uint8_t len);
void FrameData_74Convert(FRAME_CMD_t *srcData, uint8_t srcLen, uint8_t *outLen, uint8_t mode);
void FrameRouteData_74Convert(FRAME_ROUTER_CMD_t *srcData, uint8_t srcLen, uint8_t *outLen, uint8_t mode);

#endif

/***********************
֡�ṹ��
ͷ��                  1
��ַ��                4
������ţ�            1
�����룺              1
���ݳ��ȣ�            1
���ݵ�Ԫ��            ���ݳ���
У�飺                2
β��                  1
***********************/

/************************
�����������ݵ�Ԫ�ṹ�����ӻ����ã�
�������            4
���������            0��1

*************************/

/************************
�ϱ����ݵ�Ԫ�ṹ��
�ϱ���ʶ��            4
��Ϣ�֣�              3
��Ϣ���ȣ�            1
��Ϣ���ݣ�            ��Ϣ����
*************************/

/*
//֡������ķ��ʶ�λ
#define Region_HeaderNumber                 0          //֡ͷ
#define Region_AddrNumber                   1          //֡��ַ
#define Region_SeqNumber                    5          //֡�������
#define Region_CmdNumber                    6          //֡����
#define Region_DataLenNumber                7          //֡���ݳ���
#define Region_DataNumber                   8          //֡���ݱ�ʶ
#define Region_URIDNumber                   8          //�ϱ����ݱ�ʶ
#define Region_EventWordNumber              12          //��Ϣ�ֱ�ʶ
#define Region_EventDataLenNumber           15          //��Ϣ�����ݳ���
#define Region_EventDataNumber              16          //��Ϣ������

#define HKData_LenMax                           12          //���ݱ�ʶ4+����8MAX
#define HKFrame_LenMax                           (11+HKData_LenMax)     //ͷ1+��ַ4+�������1+������1+���ݳ���1 +У��2+β1
*/

////////////////////////////////Э��֡�ĺ궨��/////////////////////////////////////////////////
#define HKFreamHeader 0xAC
#define HKFreamEnd 0x53
//#define Transpond_Data_Main                         0x10
//#define Transpond_Data_Slave                        0x90

//#define Up_TimeOut_Val                              9     //250ByteԼ62.5ms

//HK������֡����
#define LogicAddr_Len 4
#define FrameCs_Len 2

#define HKData_LenMax 228                        //���ݱ�ʶ4+����8MAX, �������߼���ַ֡����
#define HKFrame_LenMax (11 + HKData_LenMax + 16) //ͷ1+��ַ4+�������1+������1+���ݳ���1 +У��2+β1+����Ԥ���ռ�
#define UpDate_Len 100

//֡������ķ��ʶ�λ
#define Region_HeaderNumber 0   //֡ͷ
#define Region_AddrNumber 1     //֡��ַ
#define Region_SeqNumber 5      //֡�������
#define Region_CmdNumber 6      //֡����
#define Region_DataLenNumber 7  //֡���ݳ���
#define Region_DataAFNNumber 8  //֡����AFN
#define Region_DataIDNumber 9   //֡���ݱ�ʶ
#define Region_DataValNumber 12 //��Ϣ�ֱ�ʶ

//AES֡��ʽ
#define AESFreamHeader 0x68

#define MACWrite_Cmd_Request 0x20
#define MACWrite_Yes_Response 0x21
#define MACWrite_NO_Response 0x22

#define MACRead_Cmd_Request 0x30
#define MACRead_Yes_Response 0x31
#define MACRead_NO_Response 0x32

#define Encode_Cmd_Request 0xC0
#define Encode_Yes_Response 0x41
#define Encode_No_Response 0x42

#define Decode_Cmd_Request 0xD0
#define Decode_Yes_Response 0x51
#define Decode_No_Response 0x52

#define GAWrite_Cmd_Request 0xA0
#define GAWrite_Yes_Response 0xA1
#define GAWrite_NO_Response 0xA2

#define GKWrite_Cmd_Request 0xB0
#define GKWrite_Yes_Response 0xB1
#define GKWrite_NO_Response 0xB2

#define Secret_Key_Yes 0x66

#define None_GA_Key 0xAE
#define None_GK_Key 0xBE
#define None_GAK_Key 0xCE

//AES������֡����
#define MAC_Data_Len 8

//#define GK_Data_Len                               13
#define GK_Data_Len (16 * 2)
#define GA_Data_Len 3

#define AesData_LenMax 16
#define AesFrame_LenMax (5 + AesData_LenMax) //ͷ1+������1+���ݳ���1 +У��2
