#ifndef __ROUTE_H
#define __ROUTE_H

#include "includes.h"
#include "app.h"

//路由层协议帧控制域结构
typedef struct FRAME_ROUTER_CTRL_
{
  uint8_t type : 1; //帧类型，1通信，0组网
  uint8_t mode : 1; //帧模式，0正常，1LDC休眠省电
  uint8_t heat : 1; //心跳帧标志
  uint8_t nn : 4;   //预留
  uint8_t dir : 1;  //传输方向，0主，1从
} FRAME_ROUTER_CTRL_t;

//路由级数和每一级对应的通信方式
typedef struct FRAME_ROUTER_NUMBER_
{
  uint8_t type : 4;  //通信方式，按每一位标识各级路由，0表示电力线载波，1表示无线
  uint8_t index : 4; //当前路由到达级数指示
} FRAME_ROUTER_NUMBER_t;
//路由层协议帧结构
typedef struct FRAME_ROUTER_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //长度（包括长度字节到结束符前面字节数）
  uint8_t len_c;                   //长度按位取反
  FRAME_ROUTER_CTRL_t ctrl;        //帧控制
  uint8_t netNum[2];               //网络号，取逻辑地址的低位2个字节
  uint8_t des_addr;                //目标地址
  uint8_t src_addr;                //源地址
  FRAME_ROUTER_NUMBER_t routerNum; //路由级数
  uint8_t router_len;              //路由表长度

} FRAME_ROUTER_CMD_t;

//路由层协议主站发出帧结构
typedef struct FRAME_ROUTER_MASTER_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //长度（包括长度字节到结束符前面字节数）
  uint8_t len_c;                   //长度按位取反
  FRAME_ROUTER_CTRL_t ctrl;        //帧控制
  uint8_t netNum[2];               //网络号，取逻辑地址的低位2个字节
  uint8_t des_addr;                //目标地址
  uint8_t src_addr[3];             //源地址
  FRAME_ROUTER_NUMBER_t routerNum; //路由级数
  uint8_t router_len;              //路由表长度

} FRAME_ROUTER_MASTER_CMD_t;

//路由层协议主站发出帧结构 (针对配网时候，目标地址是8个字节的MAC地址)
typedef struct FRAME_ROUTER_MASTER_EXT_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //长度（包括长度字节到结束符前面字节数）
  uint8_t len_c;                   //长度按位取反
  FRAME_ROUTER_CTRL_t ctrl;        //帧控制
  uint8_t netNum[2];               //网络号，取逻辑地址的低位2个字节
  uint8_t des_addr[8];             //目标地址
  uint8_t src_addr[3];             //源地址
  FRAME_ROUTER_NUMBER_t routerNum; //路由级数
  uint8_t router_len;              //路由表长度

} FRAME_ROUTER_MASTER_EXT_CMD_t;

//路由层协议从站发出帧结构
typedef struct FRAME_ROUTER_SLAVE_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //长度（包括长度字节到结束符前面字节数）
  uint8_t len_c;                   //长度按位取反
  FRAME_ROUTER_CTRL_t ctrl;        //帧控制
  uint8_t netNum[2];               //网络号，取逻辑地址的低位2个字节
  uint8_t des_addr[3];             //目标地址
  uint8_t src_addr;                //源地址
  FRAME_ROUTER_NUMBER_t routerNum; //路由级数
  uint8_t router_len;              //路由表长度

} FRAME_ROUTER_SLAVE_CMD_t;

//路由层协议帧结构 (针对配网时候，目标地址是8个字节的MAC地址)
typedef struct FRAME_ROUTER_EXT_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //长度（包括长度字节到结束符前面字节数）
  uint8_t len_c;                   //长度按位取反
  FRAME_ROUTER_CTRL_t ctrl;        //帧控制
  uint8_t netNum[2];               //网络号，取逻辑地址的低位2个字节
  uint8_t des_addr[8];             //目标地址
  uint8_t src_addr;                //源地址
  FRAME_ROUTER_NUMBER_t routerNum; //路由级数
  uint8_t router_len;              //路由表长度

} FRAME_ROUTER_EXT_CMD_t;


typedef struct MASTER_DEVICE_RSSI_
{
   uint8_t addr;
   uint8_t rssi;
}MASTER_DEVICE_RSSI_t;


typedef struct routPath_
{
    uint8_t addr;		//目标地址
    uint8_t len;			//路由表长度
    uint8_t node[3];		//中继节点
} routPath_t;


uint8_t FrameRouterCompose_ext(uint8_t *desAddrMAC,
                               uint8_t *srcData,
                               uint8_t srcLen,
                               uint8_t *outData,
                               uint8_t *routerTab,
                               uint8_t routerLen);
uint8_t FrameRouterCompose(
    uint8_t desAddr,
    uint8_t *srcData,
    uint8_t srcLen,
    uint8_t *outData,
    uint8_t *routerTab,
    uint8_t routerLen);
	

extern routPath_t  routPath[DEVICE_NUM_MAX];
	

void  vRouteFrameMatchProcess(Device_Match_t *match,QUEUE_WIRELESS_SEND_t *pMsg);		
void vQueryDeviceRssi(Device_Match_t *list);	
void  FrameHeartCompose(uint8_t desAddr);	
void vMasterToDeviceRSSISave(uint8_t addr,uint8_t rssi);	
void RouteTableInit(void);	 
void  SaveDeviceRouteTable(uint8_t desAddr, uint8_t *table,uint8_t routeLen); 
void RouteTableRef(void);
void  DeviceCtrlFromeRouteTable(uint8_t desAddr,routPath_t *routPath,QUEUE_WIRELESS_SEND_t *queueMsg);

#endif


