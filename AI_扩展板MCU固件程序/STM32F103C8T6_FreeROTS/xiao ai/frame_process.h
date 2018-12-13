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

//用串口烧公钥则定义，程序内定义则注释
//#define USE_USAT_RSAKey

//////////////////////// 宏定义 //////////////////////////////////
/*************** *************** 版本管理与记录 ****************************************/
/*
Rev_0100  增加版本和校时信息帧 2018/2/3_10点
Rev_0101  回复时有数据就需要带上数据标识 2018/2/3_16点
Rev_0102  不同的群组采用不同的通讯频率 2018/2/4_16点
Rev_0103  单独密文帧,组网帧超时加长8S 2018/2/8_19点
Rev_0104  临时采用默认的无线频点 2018/2/26_15点
Rev_0105  修复密文帧不起作用的bug 2018/2/27_11点
Rev_0106  检查密文帧的合法性 2018/3/1_13点
Rev_0107  修复异常回复帧需带异常标志的Bug，不能组网的bug,增加系统时间运行 2018/3/3_21点
Rev_0108  增加加解密测试帧，用于调试和核对 2018/3/5_21点
Rev_0109  1.密文帧改为只要下发就回确认，T1由2秒改为20秒 2.2018/3/8_22点
Rev_0110  新的扩展板中串口2改为用串口1通信
Rev_0111  公钥在程序中定义为常量，修复回复帧带后续标志的bug
Rev_0112  在异常帧中，上报时有下行的地址信息     2018/3/26
Rev_0113  增加扩展板存放MAC地址功能并改用跳频大端模式;路由时会处理其他命令帧       2018/4/26_11:30、2018/5/12_16:10
Rev_0214  跳频版本，更改路由表存储空间大小      2018/6/20_11:00、2018/6/29_17:40
Rev_0215  增加扩展板74编码能力       2018/07/09_15:00
Rev_0216  增加无线信道空闲检测，密文的存储条件修改       2018/07/13_18:35、2018/07/17_18:20
Rev_0217  无线速率提高                2018/08/13_17:00
Rev_0218  修复无线通信时MCU不响应中断，接收会终止的bug，更改TX的发送流程    2018/08/22_11:30
Rev_0301  使用STM32F103C8T6 功能延续前一版本
*/

#define Version_Number 0x0303

/***********************************************************************/
#define Devece_Attrs 1 //本体为1， 节点为0

#ifdef Use_74dcode //带上了74编码方式
#include "74.h"
#endif

#if (Devece_Attrs)
#define Frame_RetryTimes 2000 //单位为mS
#else
#define Frame_RetryTimes 500 //单位为mS
#endif

#define LED_MODE1 0x10
#define LED_MODE2 0x20
#define LED_MODE3 0x30
#define LED_MODE4 0x40

//错误字
#define Key_NotReady 0x53    //密钥未就绪
#define LANG_Addr_Error 0x54 //转发的数据，群组地址错误

#define Cipher_Error 0X04    //
#define Parameter_Error 0x05 //群组地址或密文不一样
#define Format_Error 0X06    //AES格式错误
#define Data_Error 0X07      //群组地址不一样

#define FrameFunction_Exist 0x57 //帧功能已存在

#define Secret_Key_Yes 0x66

///////////////////////////////////////////////////////////////////////
//主动上报帧
#define Local_UpReportCmd 0x88  //本地上报帧
#define Remote_UpReportCmd 0x98 //节点上报帧

//接收到的帧用0X98位与来判断
#define Local_CmdFrame 0x00        //本地命令帧
#define Local_EventFrame 0x08      //本地事件帧
#define RemoteUp_CmdFrame 0x10     //下行远程命令帧
#define RemoteUp_EventFrame 0x18   //下行远程事件帧
#define RemoteDown_CmdFrame 0x90   //上行远程命令帧
#define RemoteDown_EventFrame 0x98 //上行远程事件帧

//上报事件标志(不同的位表示)

#define Motor_Event 0u       //电机工作状态结束
#define PIR_Event 1u         //人体感应器被检出
#define Touch_Event 2u       //触摸感应器被检出
#define Floater_Event 3u     //浮子灯位置已改变
#define BatteryLow_Event 8u  //工作电池低电量
#define BatteryFull_Event 9u //工作电池满电量

#define Key_Event 1u    //按键事件
#define Switch_Event 2u //开关事件

#define Retry_Buf_Size 8 //上报BUF数量

//////////////////////////////////////////////////////////////////////
typedef struct
{
  volatile uint8_t RetryCnt; //不为0时，表示需要重试，并计算重试时间
  uint8_t Retry_SendLen;
  uint8_t RetryBuf_Number; //表示用到第几个空间
  uint8_t Retry_DataBuf[HKFrame_LenMax];
  uint32_t Retry_StartTime;
} Retry_TypDef;

typedef struct
{
  Retry_TypDef Retry_Buf[Retry_Buf_Size];
  uint8_t RetryBuf_Space; //第几bit代表第几个空间，0为可用，1为已占用
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

/************************* 外部变量 ***************************/
extern DevicePara_TypDef Device_ParaBuf;
extern HKFrame_TypDef HKFrame_Buf;
extern uint8_t LANGroup_Addr[3];
extern uint8_t Local_MAC_Addr[MAC_Data_Len + 5];
extern BeiJingTime_TypDef RunningTime_Buf;
extern uint8_t const Self_LogicAddr[4];
/************************* 外部函数 ***************************/

uint8_t FrameData_Detect(uint8_t *p, uint8_t len); //指定数据的起始地址对数据检查
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
帧结构：
头：                  1
地址：                4
配置序号：            1
控制码：              1
数据长度：            1
数据单元：            数据长度
校验：                2
尾：                  1
***********************/

/************************
控制命令数据单元结构：（从机才用）
数据命令：            4
命令参数：            0或1

*************************/

/************************
上报数据单元结构：
上报标识：            4
信息字：              3
信息长度：            1
信息数据：            信息长度
*************************/

/*
//帧各个域的访问定位
#define Region_HeaderNumber                 0          //帧头
#define Region_AddrNumber                   1          //帧地址
#define Region_SeqNumber                    5          //帧配置序号
#define Region_CmdNumber                    6          //帧命令
#define Region_DataLenNumber                7          //帧数据长度
#define Region_DataNumber                   8          //帧数据标识
#define Region_URIDNumber                   8          //上报数据标识
#define Region_EventWordNumber              12          //信息字标识
#define Region_EventDataLenNumber           15          //信息字数据长度
#define Region_EventDataNumber              16          //信息字数据

#define HKData_LenMax                           12          //数据标识4+数据8MAX
#define HKFrame_LenMax                           (11+HKData_LenMax)     //头1+地址4+配置序号1+控制码1+数据长度1 +校验2+尾1
*/

////////////////////////////////协议帧的宏定义/////////////////////////////////////////////////
#define HKFreamHeader 0xAC
#define HKFreamEnd 0x53
//#define Transpond_Data_Main                         0x10
//#define Transpond_Data_Slave                        0x90

//#define Up_TimeOut_Val                              9     //250Byte约62.5ms

//HK各命令帧长度
#define LogicAddr_Len 4
#define FrameCs_Len 2

#define HKData_LenMax 228                        //数据标识4+数据8MAX, 现在是逻辑地址帧（）
#define HKFrame_LenMax (11 + HKData_LenMax + 16) //头1+地址4+配置序号1+控制码1+数据长度1 +校验2+尾1+加密预留空间
#define UpDate_Len 100

//帧各个域的访问定位
#define Region_HeaderNumber 0   //帧头
#define Region_AddrNumber 1     //帧地址
#define Region_SeqNumber 5      //帧配置序号
#define Region_CmdNumber 6      //帧命令
#define Region_DataLenNumber 7  //帧数据长度
#define Region_DataAFNNumber 8  //帧数据AFN
#define Region_DataIDNumber 9   //帧数据标识
#define Region_DataValNumber 12 //信息字标识

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

//#define GK_Data_Len                               13
#define GK_Data_Len (16 * 2)
#define GA_Data_Len 3

#define AesData_LenMax 16
#define AesFrame_LenMax (5 + AesData_LenMax) //头1+控制码1+数据长度1 +校验2
