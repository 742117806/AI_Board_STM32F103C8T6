

#include "includes.h"


/****************************************************************
功能说明：得到串口的应用层数据帧后，针对控制通信，形成新的一帧数据
参数:
** @desAddr 要控制设备的逻辑地址
** @srcData  应用成数据
** @srcLen   应用数据长度
** @routerTab 要插入的路由表
** @routerLen 插入路由表的长度
返回值：新一帧数据的总长度
****************************************************************/
uint8_t FrameRouterCompose(
    uint8_t desAddr,
    uint8_t *srcData,
    uint8_t srcLen,
    uint8_t *outData,
    uint8_t *routerTab,
    uint8_t routerLen)
{

    FRAME_ROUTER_MASTER_CMD_t *p = (FRAME_ROUTER_MASTER_CMD_t *)outData;

    uint16_t crc_16;
    uint8_t out_frameLen;

    out_frameLen = srcLen + routerLen + 13;
    memset(outData, 0x00, out_frameLen + 4);

    p->head_h = 0x69;
    p->head_l = 0x69;

    p->len = srcLen + routerLen + 13;
    p->len_c = ~(p->len);
    p->ctrl.mode = 0;
    p->ctrl.type = 1;
    p->netNum[0] = deviceInfo.aes_field.addr_GA[1];
    p->netNum[1] = deviceInfo.aes_field.addr_GA[2];
    p->des_addr = desAddr;
    memcpy(p->src_addr, &deviceInfo.mac_frame.mac[4], 3); //扩展板的地址
    p->routerNum.index = routerLen;             //路由当前级数 等于长度
    p->routerNum.type = 0xF;
    p->router_len = routerLen;                      //路由表长度
    memcpy(&outData[13], routerTab, p->router_len); //路由表数据
    memcpy(&outData[13 + p->router_len], srcData, srcLen);
    crc_16 = CRC16_2(outData, p->len);
    outData[p->len] = crc_16 >> 8;
    outData[p->len + 1] = crc_16 & 0x00ff;
    outData[p->len + 2] = 0x96;
    outData[p->len + 3] = 0x96;
    //memcpy(srcData, outData, p->len + 4);
    return p->len + 4;
}

/****************************************************************
功能说明：得到串口的应用层数据帧后，针对配网，形成新的一帧数据
参数:
** @desAddrMAC 要配网设备的MAC地址
** @srcData  应用成数据
** @srcLen   应用数据长度
** @routerTab 要插入的路由表
** @routerLen 插入路由表的长度
**@outData 输出数据
返回值：新一帧数据的总长度
*********************************
*******************************/
uint8_t FrameRouterCompose_ext(uint8_t *desAddrMAC,
                               uint8_t *srcData,
                               uint8_t srcLen,
                               uint8_t *outData,
                               uint8_t *routerTab,
                               uint8_t routerLen)
{
    uint8_t temp[256] = {0};
    FRAME_ROUTER_MASTER_EXT_CMD_t *p = (FRAME_ROUTER_MASTER_EXT_CMD_t *)temp;
    uint16_t crc_16;

    p->head_h = 0x69;
    p->head_l = 0x69;

    p->len = srcLen + routerLen + 20;
    p->len_c = ~(p->len);
    p->ctrl.mode = 0;
    p->ctrl.type = 0;
	
    p->netNum[0] = deviceInfo.aes_field.addr_GA[1];
    p->netNum[1] = deviceInfo.aes_field.addr_GA[2];
    memcpy(p->des_addr, desAddrMAC, 8);
    memcpy(p->src_addr, &deviceInfo.mac_frame.mac[4], 3); //扩展板的地址

    p->routerNum.index = routerLen; //路由当前级数
    p->routerNum.type = 0xF;
    p->router_len = routerLen;                   //路由表长度
    memcpy(&temp[20], routerTab, p->router_len); //路由表数据
    memcpy(&temp[20 + p->router_len], srcData, srcLen);
    crc_16 = CRC16_2(temp, p->len);
    temp[p->len] = crc_16 >> 8;
    temp[p->len + 1] = crc_16 & 0x00ff;
    temp[p->len + 2] = 0x96;
    temp[p->len + 3] = 0x96;
    memcpy(outData, temp, p->len + 4);
    return p->len + 4;
}



