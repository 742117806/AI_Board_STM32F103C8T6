#ifndef __ROUTE_H
#define __ROUTE_H

#include "includes.h"
#include "app.h"

//·�ɲ�Э��֡������ṹ
typedef struct FRAME_ROUTER_CTRL_
{
  uint8_t type : 1; //֡���ͣ�1ͨ�ţ�0����
  uint8_t mode : 1; //֡ģʽ��0������1LDC����ʡ��
  uint8_t heat : 1; //����֡��־
  uint8_t nn : 4;   //Ԥ��
  uint8_t dir : 1;  //���䷽��0����1��
} FRAME_ROUTER_CTRL_t;

//·�ɼ�����ÿһ����Ӧ��ͨ�ŷ�ʽ
typedef struct FRAME_ROUTER_NUMBER_
{
  uint8_t type : 4;  //ͨ�ŷ�ʽ����ÿһλ��ʶ����·�ɣ�0��ʾ�������ز���1��ʾ����
  uint8_t index : 4; //��ǰ·�ɵ��Ｖ��ָʾ
} FRAME_ROUTER_NUMBER_t;
//·�ɲ�Э��֡�ṹ
typedef struct FRAME_ROUTER_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //���ȣ����������ֽڵ�������ǰ���ֽ�����
  uint8_t len_c;                   //���Ȱ�λȡ��
  FRAME_ROUTER_CTRL_t ctrl;        //֡����
  uint8_t netNum[2];               //����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
  uint8_t des_addr;                //Ŀ���ַ
  uint8_t src_addr;                //Դ��ַ
  FRAME_ROUTER_NUMBER_t routerNum; //·�ɼ���
  uint8_t router_len;              //·�ɱ���

} FRAME_ROUTER_CMD_t;

//·�ɲ�Э����վ����֡�ṹ
typedef struct FRAME_ROUTER_MASTER_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //���ȣ����������ֽڵ�������ǰ���ֽ�����
  uint8_t len_c;                   //���Ȱ�λȡ��
  FRAME_ROUTER_CTRL_t ctrl;        //֡����
  uint8_t netNum[2];               //����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
  uint8_t des_addr;                //Ŀ���ַ
  uint8_t src_addr[3];             //Դ��ַ
  FRAME_ROUTER_NUMBER_t routerNum; //·�ɼ���
  uint8_t router_len;              //·�ɱ���

} FRAME_ROUTER_MASTER_CMD_t;

//·�ɲ�Э����վ����֡�ṹ (�������ʱ��Ŀ���ַ��8���ֽڵ�MAC��ַ)
typedef struct FRAME_ROUTER_MASTER_EXT_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //���ȣ����������ֽڵ�������ǰ���ֽ�����
  uint8_t len_c;                   //���Ȱ�λȡ��
  FRAME_ROUTER_CTRL_t ctrl;        //֡����
  uint8_t netNum[2];               //����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
  uint8_t des_addr[8];             //Ŀ���ַ
  uint8_t src_addr[3];             //Դ��ַ
  FRAME_ROUTER_NUMBER_t routerNum; //·�ɼ���
  uint8_t router_len;              //·�ɱ���

} FRAME_ROUTER_MASTER_EXT_CMD_t;

//·�ɲ�Э���վ����֡�ṹ
typedef struct FRAME_ROUTER_SLAVE_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //���ȣ����������ֽڵ�������ǰ���ֽ�����
  uint8_t len_c;                   //���Ȱ�λȡ��
  FRAME_ROUTER_CTRL_t ctrl;        //֡����
  uint8_t netNum[2];               //����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
  uint8_t des_addr[3];             //Ŀ���ַ
  uint8_t src_addr;                //Դ��ַ
  FRAME_ROUTER_NUMBER_t routerNum; //·�ɼ���
  uint8_t router_len;              //·�ɱ���

} FRAME_ROUTER_SLAVE_CMD_t;

//·�ɲ�Э��֡�ṹ (�������ʱ��Ŀ���ַ��8���ֽڵ�MAC��ַ)
typedef struct FRAME_ROUTER_EXT_CMD_
{
  uint8_t head_h;                  //0x69
  uint8_t head_l;                  //0x69
  uint8_t len;                     //���ȣ����������ֽڵ�������ǰ���ֽ�����
  uint8_t len_c;                   //���Ȱ�λȡ��
  FRAME_ROUTER_CTRL_t ctrl;        //֡����
  uint8_t netNum[2];               //����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
  uint8_t des_addr[8];             //Ŀ���ַ
  uint8_t src_addr;                //Դ��ַ
  FRAME_ROUTER_NUMBER_t routerNum; //·�ɼ���
  uint8_t router_len;              //·�ɱ���

} FRAME_ROUTER_EXT_CMD_t;


typedef struct MASTER_DEVICE_RSSI_
{
   uint8_t addr;
   uint8_t rssi;
}MASTER_DEVICE_RSSI_t;
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
	

void  vRouteFrameMatchProcess(Device_Match_t *match,QUEUE_WIRELESS_SEND_t *pMsg);						  


#endif


