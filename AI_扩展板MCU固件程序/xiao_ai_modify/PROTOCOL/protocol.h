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

////////////////////////////////协议帧的宏定义/////////////////////////////////////////////////
#define HKFreamHeader 0xAC
#define HKFreamEnd 0x53

//AES帧格式
#define AESFreamHeader 0x68

//HK各命令帧长度
#define LogicAddr_Len 4
#define FrameCs_Len 2

//帧各个域的访问定位
#define Region_HeaderNumber 0   //帧头
#define Region_AddrNumber 1     //帧地址
#define Region_SeqNumber 5      //帧配置序号
#define Region_CmdNumber 6      //帧命令
#define Region_DataLenNumber 7  //帧数据长度
#define Region_DataAFNNumber 8  //帧数据AFN
#define Region_DataIDNumber 9   //帧数据标识
#define Region_DataValNumber 12 //信息字标识

//帧控制命令对应的位定义
#define FCMD_DIR_BIT		(1<<7)    //传输方向 1从站
#define FCMD_EVNT_BIT		(1<<3)    //事件帧
#define FCMD_REMOTE_BIT		(1<<4) 		//转发帧

#define FSEQ_ENC_BIT		(3<<5)		//加密


//AES帧格式
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


//AES各命令帧长度
#define MAC_Data_Len 8
#define MAC_FRAME_LEN 13
#define GK_Data_Len (16 * 2)
#define GA_Data_Len 3

//路由协议帧头帧尾定义
#define ROUTER_FRAME_HDADER 0x69
#define ROUTER_FRAME_END	0x96

////////////////////////////////协议帧的宏定义/////////////////////////////////////////////////





//串口接收数据状态枚举
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

    UartRx_Finished, //接收完成
} eUartRxSta_t;

//串口接收数据状态枚举
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
    Carrier_Finished, //接收完成
} eCarrierRxSta_t;


//串口接收数据结构
typedef struct
{
    volatile eUartRxSta_t status;
    uint8_t total_len;
    uint8_t frame_buff[UART_BUFF_LEN];
    volatile uint8_t time_out_cnt;
} sUartRx_t;



//载波串口接收数据结构
typedef struct
{
    volatile eCarrierRxSta_t status;
    uint8_t rec_len;
	uint8_t total_len;
    uint8_t frame_buff[UART_BUFF_LEN];
    volatile uint8_t time_out_cnt;
} sCarrierRx_t;

//检测一帧数据返回结果
typedef enum
{
	FRAME_OK,
	FRAME_ERR,
}eFrameCheckType;

//*****************LED***********************************//

//根据协议定义的枚举结构
typedef enum 
{
    AROUND_OFF,    //外围灯熄灭
    AROUND_BREATH, //外围灯呼吸模式
    AROUND_ON,     //外围灯点亮
    AROUND_FLOW,   //外围灯流水模式
    CENTRE_MODE,   //中间灯模式
    AROUND_INDEX,  //外围灯按个点亮
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
extern uint8_t wait_frameNum;		//等待的回应的帧号
extern sUartRx_t sUart2Rx ;		//定义一个串口协议接收结构体变量

//各个命令的数据标识
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
