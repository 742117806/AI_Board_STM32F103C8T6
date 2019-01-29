#ifndef __PROTOCOL_H
#define __PROTOCOL_H	 
#include "includes.h"
#include "wireless_drv.h"
#include "wireless_app.h"
//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 
#define UseComSendBytes(d,l)		UartSendBytes(USART1,d,l)
#define CarrierSendBytes(d,l)		UartSendBytes(USART2,d,l)

#define UART_BUFF_LEN	254

////////////////////////////////Э��֡�ĺ궨��/////////////////////////////////////////////////
#define HKFreamHeader 0xAC
#define HKFreamEnd 0x53

//AES֡��ʽ
#define AESFreamHeader 0x68

//HK������֡����
#define LogicAddr_Len 4
#define FrameCs_Len 2

//֡������ķ��ʶ�λ
#define Region_HeaderNumber 0   //֡ͷ
#define Region_AddrNumber 1     //֡��ַ
#define Region_SeqNumber 5      //֡�������
#define Region_CmdNumber 6      //֡����
#define Region_DataLenNumber 7  //֡���ݳ���
#define Region_DataAFNNumber 8  //֡����AFN
#define Region_DataIDNumber 9   //֡���ݱ�ʶ
#define Region_DataValNumber 12 //��Ϣ�ֱ�ʶ

//֡���������Ӧ��λ����
#define FCMD_DIR_BIT		(1<<7)    //���䷽�� 1��վ
#define FCMD_EVNT_BIT		(1<<3)    //�¼�֡
#define FCMD_REMOTE_BIT		(1<<4) 		//ת��֡

#define FSEQ_ENC_BIT		(3<<5)		//����


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
#define MAC_FRAME_LEN 13
#define GK_Data_Len (16 * 2)
#define GA_Data_Len 3

//·��Э��֡ͷ֡β����
#define ROUTER_FRAME_HDADER 0x69
#define ROUTER_FRAME_END	0x96

////////////////////////////////Э��֡�ĺ궨��/////////////////////////////////////////////////





//���ڽ�������״̬ö��
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

    UartRx_Finished, //�������
} eUartRxSta_t;

//���ڽ�������״̬ö��
typedef enum
{
    Carrier_FrameHead1 = 0,
	Carrier_FrameHead2,
    Carrier_Datalen,
    Carrier_Datalen_c,
	Carrier_Data,
    Carrier_FrameCRCH,
	Carrier_FrameCRCL,
	 Carrier_FrameEnd1,
	Carrier_FrameEnd2,
    Carrier_Finished, //�������
} eCarrierRxSta_t;


//���ڽ������ݽṹ
typedef struct
{
    volatile eUartRxSta_t status;
    uint8_t total_len;
    uint8_t frame_buff[UART_BUFF_LEN];
    volatile uint8_t time_out_cnt;
} sUartRx_t;



//�ز����ڽ������ݽṹ
typedef struct
{
    volatile eCarrierRxSta_t status;
    uint8_t rec_len;
	uint8_t total_len;
    uint8_t frame_buff[UART_BUFF_LEN];
    volatile uint8_t time_out_cnt;
} sCarrierRx_t;

//���һ֡���ݷ��ؽ��
typedef enum
{
	FRAME_OK,
	FRAME_ERR,
}eFrameCheckType;

//*****************LED***********************************//

//����Э�鶨���ö�ٽṹ
typedef enum 
{
    AROUND_OFF,    //��Χ��Ϩ��
    AROUND_BREATH, //��Χ�ƺ���ģʽ
    AROUND_ON,     //��Χ�Ƶ���
    AROUND_FLOW,   //��Χ����ˮģʽ
    CENTRE_MODE,   //�м��ģʽ
    AROUND_INDEX,  //��Χ�ư�������
} eLedCmdType;

typedef enum 
{
    INVALID,
    BLUE,
    RED_OR_ORANGE,
    GREEN_OR_PURPLE,
    WHITE,
}eLedColor;

typedef struct
{
    eLedColor ledColor : 4;
    eLedCmdType cmdType : 4;
} LedFunc_t;




extern sUartRx_t sUart1Rx;
extern uint8_t frameNume;
extern uint8_t wait_frameNum;		//�ȴ��Ļ�Ӧ��֡��
extern sUartRx_t sUart2Rx ;		//����һ������Э����սṹ�����

//������������ݱ�ʶ
extern const uint8_t CMD_INIT[][3];
extern const uint8_t CMD_STATUE[][3];
extern const uint8_t CMD_CTRL[][3];
extern const uint8_t CMD_DEVICE_CTRL[][3];

uint8_t Frame_Compose(uint8_t *p);
void vUartRxFrame(uint8_t rx_data, sUartRx_t *pu_buf,uint8_t mode);
void vUartFrameProcess(sUartRx_t *pbuff);
void vUartAesProcess(sUartRx_t *pbuff);
void FrameCmdLocalAck(uint8_t *cmdBuff,uint8_t cmdLen,uint8_t *userDat,uint8_t userLen);
void FrameCmdLocalAck1(uint8_t *cmdBuff,uint8_t cmdLen,uint8_t *userDat,uint8_t userLen);
void vWirelessFrameDeal(uint8_t *packBuff,uint8_t packLen,uint8_t mode);
void Carrier2UartFrameRec(uint8_t rec, sCarrierRx_t *pu_buf);


		 				    
#endif
