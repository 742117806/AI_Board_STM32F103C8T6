

#include "includes.h"


/****************************************************************
����˵�����õ����ڵ�Ӧ�ò�����֡����Կ���ͨ�ţ��γ��µ�һ֡����
����:
** @desAddr Ҫ�����豸���߼���ַ
** @srcData  Ӧ�ó�����
** @srcLen   Ӧ�����ݳ���
** @routerTab Ҫ�����·�ɱ�
** @routerLen ����·�ɱ�ĳ���
����ֵ����һ֡���ݵ��ܳ���
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
    memcpy(p->src_addr, &deviceInfo.mac_frame.mac[4], 3); //��չ��ĵ�ַ
    p->routerNum.index = routerLen;             //·�ɵ�ǰ���� ���ڳ���
    p->routerNum.type = 0xF;
    p->router_len = routerLen;                      //·�ɱ���
    memcpy(&outData[13], routerTab, p->router_len); //·�ɱ�����
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
����˵�����õ����ڵ�Ӧ�ò�����֡������������γ��µ�һ֡����
����:
** @desAddrMAC Ҫ�����豸��MAC��ַ
** @srcData  Ӧ�ó�����
** @srcLen   Ӧ�����ݳ���
** @routerTab Ҫ�����·�ɱ�
** @routerLen ����·�ɱ�ĳ���
**@outData �������
����ֵ����һ֡���ݵ��ܳ���
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
    memcpy(p->src_addr, &deviceInfo.mac_frame.mac[4], 3); //��չ��ĵ�ַ

    p->routerNum.index = routerLen; //·�ɵ�ǰ����
    p->routerNum.type = 0xF;
    p->router_len = routerLen;                   //·�ɱ���
    memcpy(&temp[20], routerTab, p->router_len); //·�ɱ�����
    memcpy(&temp[20 + p->router_len], srcData, srcLen);
    crc_16 = CRC16_2(temp, p->len);
    temp[p->len] = crc_16 >> 8;
    temp[p->len + 1] = crc_16 & 0x00ff;
    temp[p->len + 2] = 0x96;
    temp[p->len + 3] = 0x96;
    memcpy(outData, temp, p->len + 4);
    return p->len + 4;
}



