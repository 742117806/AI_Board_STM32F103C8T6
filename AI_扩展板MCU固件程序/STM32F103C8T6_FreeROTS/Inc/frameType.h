#ifndef __FRAMETYPE_H
#define __FRAMETYPE_H
#include "stm32f1xx.h"
#include "deviceInfo.h"


//和通信帧相关的定义
#define HKData_LenMax                               228          //数据标识4+数据8MAX, 现在是逻辑地址帧（）
#define HKFrame_LenMax                              (11+HKData_LenMax+16)     //头1+地址4+配置序号1+控制码1+数据长度1 +校验2+尾1+加密预留空间

#define DEVICE_INDEX_OFFSET		31			//设备地址偏移量（设备取地址范围31-254）

//串口烧录MAC命令
typedef struct WRITE_MAC_CMD_
{
	uint8_t FameHead;		// 帧头
	uint8_t CmdFunc;		//功能命令
	uint8_t DataLen;			//数据长度
	uint8_t mac[8];			//设备MAC
	uint8_t crc[2];
}WRITE_MAC_CMD_t;


//帧配置码
typedef struct FSQ_
{
	uint8_t frameNum:4;			//帧序号
	uint8_t ctrlField:1;		//控制域：0一个字节，1两个字节
	uint8_t encryptType:2;		//加密类型：0加密，1第一种加密，2第二种加密，3第三种加密
	uint8_t routeFlag:1;		//路由标志
}FSQ_t;

//帧控制码
typedef struct CTRL_
{
	uint8_t c_AFN:3;		//控制功能码，0从站应答使用，1读后续帧数据
	uint8_t eventFlag:1;	//事件标志，0普通帧，1事件帧
	uint8_t	relayFlag:1;	//中继标志，0本地帧，1转发帧
	uint8_t followUpFlag:1;	//后续帧标志，0无后续帧，1有后续帧
	uint8_t recAckFlag:1;	//接收站接收应答标志，0正确应答，1异常应答
	uint8_t dir:1;			//传送方向，0主站发出，1从站发出
}CTRL_t;

//协议帧数据区
typedef struct FRAME_DATA_
{
	uint8_t AFN;		//数据功能码
	uint8_t Index[3];	//数据标识
	uint8_t content[235];	//链路数据		
}FRAME_DATA_t;
//帧命令
typedef struct FRAME_CMD_
{
	uint8_t FameHead;		//帧头
	uint8_t addr_DA;		//逻辑地址
	uint8_t addr_GA[3];		//群众地址
	FSQ_t 	FSQ;			//帧配置码
	CTRL_t Ctrl;			//帧控制码
	uint8_t DataLen;		//数据长度
	FRAME_DATA_t userData;	//用户数据区
//帧命令			
	
}FRAME_CMD_t;
//设备入网链路数据区
typedef struct JOINE_NET_CMD_
{
	uint8_t mac[8];
	uint8_t aes[16];		//用在rsa函数来得出一个aes加密或解密的密钥
	
}JOINE_NET_CMD_t;

//安卓板下发通信密钥和群组地址的数据结构
typedef struct WRITE_AES_CMD_
{
	uint8_t aes[16];		//用在rsa函数来得出一个aes加密或解密的密钥
	uint8_t addr_GA[3];		//群众地址

}WRITE_AES_CMD_t;



typedef enum LedCmdType_
{
	AROUND_OFF,         //外围灯熄灭
	AROUND_BREATH,      //外围灯呼吸模式
	AROUND_ON,	        //外围灯点亮
	AROUND_FLOW,        //外围灯流水模式
	CENTRE_MODE,		//中间灯模式
	AROUND_INDEX,		//外围灯按个点亮	
}LedCmdType_e;

typedef enum LedColor_
{	
	INVALID,
	BLUE,
	RED_OR_ORANGE,
	GREEN_OR_PURPLE,
	WHITE,
}LedColor_e;

typedef struct LedFunc_
{
   LedColor_e ledColor:4;
   LedCmdType_e cmdType:4;
}LedFunc_t;

//路由层协议帧控制域结构
typedef struct FRAME_ROUTER_CTRL_
{
	uint8_t type:1;			//帧类型，1通信，0组网
	uint8_t mode:1;			//帧模式，0正常，1LDC休眠省电
	uint8_t heat:1;			//心跳帧标志
	uint8_t nn:4;			//预留
	uint8_t dir:1;			//传输方向，0主，1从	
}FRAME_ROUTER_CTRL_t;

//路由级数和每一级对应的通信方式
typedef struct FRAME_ROUTER_NUMBER_
{
    uint8_t type:4;  //通信方式，按每一位标识各级路由，0表示电力线载波，1表示无线
	uint8_t index:4;		//当前路由到达级数指示
}FRAME_ROUTER_NUMBER_t; 
//路由层协议帧结构
typedef struct FRAME_ROUTER_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//长度（包括长度字节到结束符前面字节数）
	uint8_t len_c;     //长度按位取反
	FRAME_ROUTER_CTRL_t ctrl;		//帧控制
    uint8_t netNum[2];			//网络号，取逻辑地址的低位2个字节
	uint8_t des_addr;			//目标地址
	uint8_t src_addr;			//源地址
	FRAME_ROUTER_NUMBER_t routerNum;	//路由级数
	uint8_t router_len;			//路由表长度
		
}FRAME_ROUTER_CMD_t;

//路由层协议主站发出帧结构
typedef struct FRAME_ROUTER_MASTER_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//长度（包括长度字节到结束符前面字节数）
	uint8_t len_c;     //长度按位取反
	FRAME_ROUTER_CTRL_t ctrl;		//帧控制
    uint8_t netNum[2];			//网络号，取逻辑地址的低位2个字节
	uint8_t des_addr;			//目标地址
	uint8_t src_addr[3];			//源地址
	FRAME_ROUTER_NUMBER_t routerNum;	//路由级数
	uint8_t router_len;			//路由表长度
		
}FRAME_ROUTER_MASTER_CMD_t;

//路由层协议主站发出帧结构 (针对配网时候，目标地址是8个字节的MAC地址)
typedef struct FRAME_ROUTER_MASTER_EXT_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//长度（包括长度字节到结束符前面字节数）
	uint8_t len_c;     //长度按位取反
	FRAME_ROUTER_CTRL_t ctrl;		//帧控制
    uint8_t netNum[2];			//网络号，取逻辑地址的低位2个字节
	uint8_t des_addr[8];			//目标地址
	uint8_t src_addr[3];			//源地址
	FRAME_ROUTER_NUMBER_t routerNum;	//路由级数
	uint8_t router_len;			//路由表长度
		
}FRAME_ROUTER_MASTER_EXT_CMD_t;

//路由层协议从站发出帧结构
typedef struct FRAME_ROUTER_SLAVE_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//长度（包括长度字节到结束符前面字节数）
	uint8_t len_c;     //长度按位取反
	FRAME_ROUTER_CTRL_t ctrl;		//帧控制
    uint8_t netNum[2];			//网络号，取逻辑地址的低位2个字节
	uint8_t des_addr[3];			//目标地址
	uint8_t src_addr;			//源地址
	FRAME_ROUTER_NUMBER_t routerNum;	//路由级数
	uint8_t router_len;			//路由表长度
		
}FRAME_ROUTER_SLAVE_CMD_t;



//路由层协议帧结构 (针对配网时候，目标地址是8个字节的MAC地址)
typedef struct FRAME_ROUTER_EXT_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//长度（包括长度字节到结束符前面字节数）
	uint8_t len_c;     //长度按位取反
	FRAME_ROUTER_CTRL_t ctrl;		//帧控制
    uint8_t netNum[2];			//网络号，取逻辑地址的低位2个字节
	uint8_t des_addr[8];			//目标地址
	uint8_t src_addr;			//源地址
	FRAME_ROUTER_NUMBER_t routerNum;	//路由级数
	uint8_t router_len;			//路由表长度
	

}FRAME_ROUTER_EXT_CMD_t;

//串口转无线的队列结构体
typedef struct QUEUE_UART_TO_WIRELESS_
{
	uint8_t msg[256];		//队列数据
	uint8_t len;			//数据长度
	uint8_t toCh;			//要发送的无线数据通道号
	
}QUEUE_WIRELESS_SEND_t;

//无线数据重发后要等到的数据标识3个字节和帧序号
typedef struct RETRY_WAITE_FOR_
{
	uint8_t index[3];    //数据标识
	uint8_t frameNum;	//帧序号
	uint8_t retryCnt;		//重发次数
	uint8_t flag;		//等待回应标志，1要等待回复，已经等到回复
} RETRY_WAITE_FOR_t;


/* 路由表结构相关定义 */
//每个节点的地址和对应的信号强度值
typedef struct NODE_
{
	uint8_t addr;
	uint8_t rssi;
}NODE_t;


//节点链接起来的路径
typedef struct PATH_
{
	uint8_t len;		//路由表长度
	uint8_t addr[3];   	//1级中继 , 2级中继, 3级中继
}PATH_t;


//目的设备
typedef struct DES_DEVICE_
{
	//uint8_t des_addr;		//目的地设备地址
	uint8_t des_rssi;		//目的地设备信号强度
	PATH_t path1;
	PATH_t path2;
	PATH_t path3;
}DES_DEVICE_t;

//当前应答的路由表和表长度结构
typedef struct ROUTER_TAB_ACK_
{
	uint8_t len;
	uint8_t table[3];
	
}ROUTER_TAB_ACK_t;

//缓存设备邻近节点的信号强度结构
typedef struct ROUTER_BROTHER_RSSI_
{
	uint8_t addr1;
	uint8_t rssi1;		//3个邻近节点 
	uint8_t addr2;
	uint8_t rssi2;		//3个邻近节点 
	uint8_t addr3;
	uint8_t rssi3;		//3个邻近节点 
}ROUTER_BROTHER_RSSI_t;


#endif


