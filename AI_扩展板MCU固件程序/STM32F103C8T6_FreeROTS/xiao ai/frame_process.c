#include "frame_process.h"
#include "aes.h"
#include "stmflash.h"
#include "string.h"
#include "stm32f0_eeprom.h"
#include "stm32f0_usart.h"
#include "user_call_back.h"
#include "delay.h"
#include "wireless_app.h"

DevicePara_TypDef Device_ParaBuf;
HKFrame_TypDef HKFrame_Buf;
uint8_t LANGroup_Addr[3];
uint8_t Local_MAC_Addr[MAC_Data_Len + 5];

//uint8_t EventFrame_Buf[EventFrame_LenMax];
//uint8_t CommandFrame_Buf[CommandFrame_LenMax];
//uint8_t Retry_DataBuf[5];

BeiJingTime_TypDef RunningTime_Buf; //运行中的时间，在单片机运行中更新
//BeiJingTime_TypDef WantCalibTime_Buf,        //需要校准的时间（指接收到时间校准帧的即时时刻）
//BeiJingTime_TypDef RealTime_Buf,            //实际时间（指的是下发的时间校准帧上的数据时间）

/************************** 人工写入的公钥 ***************************/
#ifndef USE_USAT_RSAKey

uint8_t const Mi_Yue_Tab[RsaByte_Size] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X08, 0X33, 0X99}; //02 7A D2 B3 82 97 C6 A6 86 14 A6 67 CB 74 CD 21 //RSA密钥直接存于程序的常量中

uint8_t const Mo_Zhi_Tab[RsaByte_Size] = {
    0XA4, 0XD9, 0XB2, 0XE8, 0X34, 0X7F, 0XBE, 0XF7, 0X14, 0XFA, 0X74, 0X32, 0XA3, 0X57, 0XC6, 0XAD}; //62 DA CF C7 85 2E E8 B1 A3 D4 0E 22 D1 A1 8D 17 //RSA模值直接存于程序的常量中

#endif

/*******************************************************************/

uint8_t const Self_LogicAddr[4] = {0xff, 0xff, 0xff, 0xff}; //{0x10, 0x01, 0x00, 0x00};

//uint8_t const UpReport_Id[4] = {0x01,0x15,0x00,0x04};
//uint8_t const Key_EventWord[3] = {0x00,0x00,0x01};
//uint8_t const Key_EventState_Id[4] = {0x02, 0x01, 0x00, 0x80};     //{0x80,0x00,0x01,0x00};

uint8_t const MACRead_ID[3] = {0x00, 0x00, 0x00};
uint8_t const Set_LogicAddr_Id[3] = {0xff, 0xff, 0xff};
uint8_t const Upgrade_ID[3] = {0xff, 0xff, 0xfb};

uint8_t const Led_Control_ID[3] = {0x01, 0x00, 0x21};
uint8_t const Key_Event_ID[3] = {0x00, 0x00, 0x20};
uint8_t const Provide_LogicAddrTab_Id[3] = {0xff, 0xff, 0xfe};

uint8_t const GetVersion_ID[3] = {0xff, 0x00, 0x01}; //版本查询
uint8_t const TimeCalib_ID[3] = {0xff, 0x00, 0x00};  //时间校准
uint8_t const SecretKey_ID[3] = {0xff, 0xff, 0x00};  //密文下发

#ifdef Use_RSA_Debug
uint8_t const SecretTest_ID[3] = {0x55, 0x55, 0x55}; //密文下发
#endif

void Rsa_Decode(uint8_t *p_minw)
{
    uint8_t *Mi_Yue; //02 7A D2 B3 82 97 C6 A6 86 14 A6 67 CB 74 CD 21
    uint8_t *Mo_Zhi; //62 DA CF C7 85 2E E8 B1 A3 D4 0E 22 D1 A1 8D 17
    uint8_t *Mi_Wen; //30 34 47 F9 6D 76 75 A7 8F 47 26 8E 70 4C BF 21
    uint8_t *RSA_TempBuf;

    //sim(); //关掉所有中断
    //__disable_interrupt();
#if (0)
    Mi_Wen = (uint8_t *)malloc(RsaByte_Size + 3);
    Mi_Yue = (uint8_t *)malloc(RsaByte_Size);
    Mo_Zhi = (uint8_t *)malloc(RsaByte_SizeMax);
    RSA_TempBuf = (uint8_t *)malloc(RsaByte_SizeMax);
#else
    Mi_Yue = aes_w;
    Mo_Zhi = &aes_w[RsaByte_Size];
    Mi_Wen = &aes_w[RsaByte_Size + RsaByte_SizeMax];
    RSA_TempBuf = &aes_w[RsaByte_Size * 2 + RsaByte_SizeMax + 3];
#endif
    memset(Mi_Wen, 0, RsaByte_Size);
    memset(Mi_Yue, 0, RsaByte_Size);
    memset(Mo_Zhi, 0, RsaByte_SizeMax);
    memset(RSA_TempBuf, 0, RsaByte_SizeMax);

#ifdef USE_USAT_RSAKey

#if (0) //已经准备好了，不需要再读
    Eeprom_Read(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2);
#endif
    memmove(Mi_Yue, aes_out, RsaByte_Size);
    memmove(&Mo_Zhi[1], &aes_out[RsaByte_Size], RsaByte_Size);

#else //从程序常量中获取公钥和模值
    memmove(Mi_Yue, Mi_Yue_Tab, RsaByte_Size);
    memmove(&Mo_Zhi[1], Mo_Zhi_Tab, RsaByte_Size);

#endif

    //Eeprom_ByteRead(Mi_Wen_EAddr, Mi_Wen, RsaByte_Size+3);
    memmove(Mi_Wen, &aes_out[RsaByte_Size * 2], RsaByte_Size + 3);

    LargeNumber_Power_Mod(p_minw, Mi_Wen, Mi_Yue, RSA_TempBuf, Mo_Zhi);

#if (0)
    p_minw[3] = Mi_Wen[RsaByte_Size + 0];
    p_minw[7] = Mi_Wen[RsaByte_Size + 1];
    p_minw[11] = Mi_Wen[RsaByte_Size + 2];
#endif

#if (0)
    free(Mi_Yue);
    free(Mo_Zhi);
    free(Mi_Wen);
    free(RSA_TempBuf);
#endif
    //rim();
    //__enable_interrupt();
}

uint8_t RealData_Detect(uint8_t *p, uint8_t len, uint8_t Frame_len)
{
    uint16_t crc16_val;
    uint8_t crc_len;

    if ((p[0] != HKFreamHeader) && (p[0] != AESFreamHeader))
        return 1;

    if (p[0] == HKFreamHeader)
        crc_len = Frame_len - 3;
    else
        crc_len = Frame_len - 2;

    crc16_val = CRC16_2(p, len);
    if ((crc16_val >> 8) != p[crc_len]) //比较CRC16高字节区
        return 1;
    if ((crc16_val & 0xff) != p[crc_len + 1])
        return 1;

    return 0;
}

uint8_t FrameData_Detect(uint8_t *p, uint8_t len)
{
    uint16_t crc16_val;
    uint8_t crc_len;

    if ((p[0] != HKFreamHeader) && (p[0] != AESFreamHeader))
        return 1;

    if (p[0] == HKFreamHeader)
        crc_len = len - 3;
    else
        crc_len = len - 2;
    crc16_val = CRC16_2(p, crc_len);
    if ((crc16_val >> 8) != p[crc_len])
        return 1;
    if ((crc16_val & 0xff) != p[crc_len + 1])
        return 1;

    return 0;
}

uint8_t Frame_Check(uint8_t *p, uint8_t Len) //给无线编码使用的校验函数
{
    uint8_t ComposeFrame_Len;
    uint16_t crc16_val;

    p[Region_HeaderNumber] = HKFreamHeader;

    ComposeFrame_Len = Len;
    crc16_val = CRC16_2(p, ComposeFrame_Len);
    p[ComposeFrame_Len] = crc16_val >> 8;
    ComposeFrame_Len++;
    p[ComposeFrame_Len] = crc16_val & 0xff;
    ComposeFrame_Len++;
    p[ComposeFrame_Len] = HKFreamEnd;
    ComposeFrame_Len++;

    return ComposeFrame_Len;
}
uint8_t Frame_Compose(uint8_t *p) //给上位机的帧进行校验
{
    uint8_t ComposeFrame_Len;
    uint16_t crc16_val;

    p[Region_HeaderNumber] = HKFreamHeader;

    p[Region_CmdNumber] &= ~(1 << 5);

    ComposeFrame_Len = p[Region_DataLenNumber] + Region_DataAFNNumber;
    crc16_val = CRC16_2(p, ComposeFrame_Len);
    p[ComposeFrame_Len] = crc16_val >> 8;
    ComposeFrame_Len++;
    p[ComposeFrame_Len] = crc16_val & 0xff;
    ComposeFrame_Len++;
    p[ComposeFrame_Len] = HKFreamEnd;
    ComposeFrame_Len++;

    return ComposeFrame_Len;
}

void Retry_Start(HKFrame_TypDef *p_framebuf, uint8_t *p_source, uint8_t source_len)
{
    uint8_t i;
    Retry_TypDef *p_retry;

    for (i = 0; i < Retry_Buf_Size; i++)
    {
        if ((p_framebuf->RetryBuf_Space & (1 << i)) == 0)
        {
            p_framebuf->RetryBuf_Space |= (1 << i);
            p_retry = &p_framebuf->Retry_Buf[i]; //
            p_retry->RetryBuf_Number = i;
            p_retry->Retry_SendLen = source_len;
            memmove(p_retry->Retry_DataBuf, p_source, source_len);
            //rim();
            //p_retry->Retry_StartTime = SYSTICK_CNT;
            //sim();
            p_retry->RetryCnt = 2;

            return;
        }
    }
}

/****************************Convert_Mode 为0：加密   为1：解密*******************************/
uint8_t Encrypt_Convert(uint8_t *p_source, uint8_t len, uint8_t Convert_Mode)
{
#ifdef Use_Rout
    uint8_t rout_region[3 + RoutSeries_Size];
#endif
    uint16_t crc16_val;
    uint8_t i, res_len;
    uint8_t Encode_Round;
    uint8_t Encode_Mantissa;
    uint8_t Encode_Len = p_source[Region_DataLenNumber];

    Encode_Round = Encode_Len / 16;
    Encode_Mantissa = Encode_Len % 16;
#ifdef Use_Rout
    if ((p_source[Region_SeqNumber] & 0x80) && Convert_Mode)
    {
        //memmove(rout_region, &p_source[Region_DataAFNNumber + p_source[Region_DataLenNumber]], 3+RoutSeries_Size);
        memmove(rout_region, &p_source[len - (3 + 3 + RoutSeries_Size)], 3 + RoutSeries_Size);
    }
#endif
    for (i = 0; i < Encode_Round; i++)
    {
        memmove(aes_in, &p_source[Region_DataAFNNumber + 16 * i], 16);
        if (Convert_Mode)
            inv_cipher(aes_in, aes_out, aes_w);
        else
            cipher(aes_in, aes_out, aes_w);
        memmove(&p_source[Region_DataAFNNumber + 16 * i], aes_out, 16);
    }
    if (Encode_Mantissa)
    {
        if (Convert_Mode)
        {
            memmove(aes_in, &p_source[Region_DataAFNNumber + 16 * Encode_Round], 16);
            inv_cipher(aes_in, aes_out, aes_w);
            memmove(&p_source[Region_DataAFNNumber + 16 * i], aes_out, Encode_Mantissa);
            res_len = len - 16 + Encode_Mantissa;
        }
        else
        {
            memmove(aes_in, &p_source[Region_DataAFNNumber + 16 * Encode_Round], Encode_Mantissa);
            for (i = 0; i < (16 - Encode_Mantissa); i++)
            {
                p_source[Region_DataAFNNumber + 16 * Encode_Round + Encode_Mantissa + i] = 0;
            }
            //memmove(&aes_in[Encode_Mantissa], 0, (16 - Encode_Mantissa));
            cipher(aes_in, aes_out, aes_w);
            memmove(&p_source[Region_DataAFNNumber + 16 * Encode_Round], aes_out, 16);
            res_len = len + 16 - Encode_Mantissa;
        }
    }
    else
        res_len = len;
#ifdef Use_Rout
    if ((p_source[Region_SeqNumber] & 0x80) && Convert_Mode)
    {
        memmove(&p_source[Region_DataAFNNumber + p_source[Region_DataLenNumber]], rout_region, 3 + RoutSeries_Size);
        res_len += (3 + RoutSeries_Size);
    }

#endif
    crc16_val = CRC16_2(p_source, res_len - 3);
    p_source[res_len - 3] = crc16_val >> 8;
    p_source[res_len - 2] = crc16_val & 0xff;
    p_source[res_len - 1] = HKFreamEnd;

    return res_len;
}

///////////////////////////////////////////////////////////////////////////////////

//LED控制命令的处理
#if 0
void Led_Control_Process(uint8_t led_setval, DevicePara_TypDef *p_device)
{
    if ((led_setval & 0xf0) == 0x40)
    {
        uint8_t color_val;

        if ((led_setval & 0x0f) <= 0x03)
        {
            color_val = led_setval & 0x03;
        }
        else
            color_val = 0x01;
        if (color_val == 0x00)
        {
            if (p_device->MiddleLed_Contorl == 0x10)
            {
                p_device->MiddleLed_Contorl = 0x02;
            }
        }
        else if (p_device->MiddleLed_Contorl == 0x00)
        {
            p_device->MiddleLed_Contorl = 0x01;
            p_device->Led4_CotrolPara = color_val | 0x40;
        }
    }

    else if ((led_setval & 0xf0) == 0x50)
    {
        LED_Clear(1);
        p_device->Led_Mode5DelayCnt = 0;
        p_device->Led_Mode5Volume = led_setval & 0x0f;
        if ((p_device->Led_CotrolPara & 0xf0) != 0x50)
            p_device->Led_LastCotrolPara = p_device->Led_CotrolPara;
        p_device->Led_CotrolPara = led_setval;
    }

    else
    {
        if (p_device->Led_CotrolPara != led_setval)
        {
            LED_Clear(1);
            p_device->Led_CotrolPara = led_setval;
        }
    }
}
#endif

//返回0表示解出来的AES符合格式，返回非1表示解出来的AES不符合格式,返回2表示AES已存在
uint8_t SecretKey_Process(uint8_t *p)
{
    uint8_t res_val = 0;
	uint8_t temp[4]={0};
                        
    //if(memcmp(p, &aes_out[RsaByte_Size*2], 19) != 0)
    //if(!Secret_KeyOk_Flag)   //密钥一旦ok，不再接受其它密文
    if (1)
    {
        Eeprom_Read(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2); //读出私钥到RAM

        memmove(&aes_out[RsaByte_Size * 2], p, RsaByte_Size + 3); //移入密文和群组地址到RAM备用
        aes_out[AesBuf_Size - 1] = Secret_Key_Yes;

#if (0) //需要保存,群组地址的密文需保持一致
        Eeprom_ErasePage(KEY_StartAddr);
        Eeprom_Write(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2);
#endif
        Rsa_Decode(aes_out);
#if (0)
        if (Secret_KeyOk_Flag != 1)
        {
            aes_w = (uint8_t *)malloc(Nb * (Nr + 1) * 4);
        }
#endif

        if ((aes_out[3] != p[RsaByte_Size + 0]) || (aes_out[7] != p[RsaByte_Size + 1]) || (aes_out[11] != p[RsaByte_Size + 2])) //核对错误
        {
            //Secret_KeyOk_Flag = 0;
            res_val = 1;
        }
        else
        {
            memmove(LANGroup_Addr, &aes_out[RsaByte_Size * 3], 3); //移入群组地址到群组地址BUF
            key_expansion(aes_out, aes_w);

            Eeprom_ErasePage(KEY_StartAddr);
            Eeprom_Write(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2);
			memcpy(temp,LANGroup_Addr,3);					//把家庭组地址复制到一个4个字节的数组，位了保存时候字节对齐
            Eeprom_Write(GD_ADDR,(uint16_t*)temp,4/2);      //保存家庭组地址到MCU Flash
            Secret_KeyOk_Flag = 1;
            res_val = 0;

            Get_WireLessChannel(Wireless_Channel);
#ifdef Use_Rx_Hop
            Wireless_Init(); //Initial Wireless，开始初始化无线
#endif
            //WIRELESS_STATUS = Wireless_Free;
            Si4438_Receive_Start(Wireless_Channel[0]); //Start Receive
        }
    }
    else
    {
        res_val = 2;
    }

    return res_val;
}

uint8_t MACRead_Process(uint8_t *p_buf)
{
    Eeprom_Read(MAC_EAddr, (uint16_t *)Local_MAC_Addr, MAC_Data_Len + 5); //

    if (0 == FrameData_Detect(Local_MAC_Addr, MAC_Data_Len + 5))
    {
        memmove(&p_buf[Region_DataValNumber], &Local_MAC_Addr[3], MAC_Data_Len);
        p_buf[Region_DataLenNumber] = 12;
        return 1;
    }
    else
        return 0;
}

void Local_CmdFrame_Process(uint8_t *p_source, HKFrame_TypDef *p_framebuf, DevicePara_TypDef *p_device)
{
    uint8_t reply_flag = 0;
    uint8_t send_len;
    uint8_t *p = p_framebuf->FrameProcess_Buf;
    uint8_t ask = 0;

    if (memcmp(&p_source[Region_DataIDNumber], Led_Control_ID, 3) == 0)
    {
        //Led_Control_Process(p_source[Region_DataValNumber], p_device);
        p[Region_DataLenNumber] = 0;
        reply_flag = 1;
    }

    else if (memcmp(&p_source[Region_DataIDNumber], SecretKey_ID, 3) == 0)
    { //获得密文后重新初始化了无线
        ask = SecretKey_Process(&p_source[Region_DataValNumber]);

        if (ask == 0)
        {
            p[Region_DataLenNumber] = 0;
        }
        else if (ask == 1)
        {
            p_source[Region_CmdNumber] |= 0x40;
            p[Region_DataLenNumber] = 1;
            p[Region_DataAFNNumber] = Format_Error;
        }
        else
        {
            p_source[Region_CmdNumber] |= 0x40;
            p[Region_DataLenNumber] = 1;
            p[Region_DataAFNNumber] = FrameFunction_Exist;
        }
        reply_flag = 1;
    }

#ifdef Use_RSA_Debug
    else if (memcmp(&p_source[Region_DataIDNumber], SecretTest_ID, 3) == 0)
    {
        Eeprom_Read(KEY_StartAddr, (uint16_t *)aes_out, AesBuf_Size / 2);                   //读出私钥到RAM
        memmove(&aes_out[RsaByte_Size * 2], &p_source[Region_DataValNumber], RsaByte_Size); //移入密文和群组地址到RAM
        Rsa_Decode(aes_out);
        p[Region_DataLenNumber] = RsaByte_Size + 4;
        memmove(&p[Region_DataAFNNumber], &p_source[Region_DataAFNNumber], 4);
        memmove(&p[Region_DataValNumber], aes_out, RsaByte_Size);
        reply_flag = 1;
    }
#endif

    else if (memcmp(&p_source[Region_DataIDNumber], Provide_LogicAddrTab_Id, 3) == 0)
    {
#ifdef Use_Rout
        Receive_LogicAddrTab_Process(p_source, &Rout_TryBuf);
#endif
        p[Region_DataLenNumber] = 0;
        reply_flag = 1;
    }

    else if (memcmp(&p_source[Region_DataIDNumber], GetVersion_ID, 3) == 0)
    {
        //GetVersion_Process(p_source, &Rout_TryBuf);
        memmove(&p[Region_DataAFNNumber], &p_source[Region_DataAFNNumber], 4);
        p[Region_DataValNumber] = Version_Number >> 8;
        p[Region_DataValNumber + 1] = Version_Number & 0xff;
        p[Region_DataLenNumber] = 6;

        reply_flag = 1;
    }

    else if (memcmp(&p_source[Region_DataIDNumber], TimeCalib_ID, 3) == 0)
    {
        //__disable_interrupt();
        RunningTime_Buf.year = ((p[Region_DataValNumber] >> 4) & 0x0f) * 10 + (p[Region_DataValNumber] & 0x0f);
        RunningTime_Buf.month = ((p[Region_DataValNumber + 1] >> 4) & 0x01) * 10 + (p[Region_DataValNumber + 1] & 0x0f);
        RunningTime_Buf.date = ((p[Region_DataValNumber + 2] >> 4) & 0x03) * 10 + (p[Region_DataValNumber + 2] & 0x0f);
        RunningTime_Buf.hour = ((p[Region_DataValNumber + 3] >> 4) & 0x03) * 10 + (p[Region_DataValNumber + 3] & 0x0f);
        RunningTime_Buf.minute = ((p[Region_DataValNumber + 4] >> 4) & 0x07) * 10 + (p[Region_DataValNumber + 4] & 0x0f);
        RunningTime_Buf.second = ((p[Region_DataValNumber + 5] >> 4) & 0x07) * 10 + (p[Region_DataValNumber + 5] & 0x0f);
        //__enable_interrupt();

        p[Region_DataLenNumber] = 0;

        reply_flag = 1;
    }

    else if (memcmp(&p_source[Region_DataIDNumber], MACRead_ID, 3) == 0)
    {
        uint8_t Rel = MACRead_Process(p); //读取MAC地址帧的处理

        if (!Rel)
        {
            p_source[Region_CmdNumber] |= 0x40;
            p[Region_DataLenNumber] = 5;
            p[Region_DataValNumber] = Data_Error;
        }
        memmove(&p[Region_DataAFNNumber], &p_source[Region_DataAFNNumber], 4);

        reply_flag = 1;
    }

    if (reply_flag)
    {
        if (!ask)
            memmove(&p[Region_AddrNumber], Self_LogicAddr, LogicAddr_Len);
        else
            memmove(&p[Region_AddrNumber], &p_source[Region_AddrNumber], LogicAddr_Len); //按照下行的地址信息上报
        p[Region_SeqNumber] = p_source[Region_SeqNumber];                                //
        p[Region_CmdNumber] = p_source[Region_CmdNumber] | 0x80;                         //
        send_len = Frame_Compose(p);                                                     //
        UpUart_DataTx(p, send_len, &UpCom_TxBuf);                                        //
    }
}

void Clear_EventFrame_Process(uint8_t *p_source, HKFrame_TypDef *p_framebuf)
{
    uint8_t i;
    Retry_TypDef *p_retry;

    for (i = 0; i < Retry_Buf_Size; i++)
    {
        p_retry = &p_framebuf->Retry_Buf[i];
        if (p_retry->RetryCnt)
        {
            if ((p_retry->Retry_DataBuf[Region_SeqNumber] & 0x0f) == (p_source[Region_SeqNumber] & 0x0f))
            {
                if (memcmp(&p_retry->Retry_DataBuf[Region_AddrNumber], &p_source[Region_AddrNumber], 4) == 0)
                {
                    p_retry->RetryCnt = 0;
                    p_framebuf->RetryBuf_Space &= ~(1 << p_retry->RetryBuf_Number);
                    return;
                }
            }
        }
    }
}

void RemoteUp_CmdFrame_Process(uint8_t *p_source, uint8_t len, uint8_t *p_buf)
{
    uint8_t send_len, transpond_flag = 0; //
    uint8_t frame_cmd = p_source[Region_CmdNumber];
    uint8_t WakeUp_Cnt = 20;
    uint8_t WakeUp_Data[3] = {0XAB, 0XCD, 0XEF};

    p_source[Region_SeqNumber] &= 0x7f;

    if (memcmp(&p_source[Region_DataIDNumber], Set_LogicAddr_Id, 3) == 0) //如为组网帧
    {
        if ((memcmp(&p_source[Region_AddrNumber + 1], LANGroup_Addr, 3) != 0) || (memcmp(&p_source[Region_DataValNumber + MAC_Data_Len], &aes_out[RsaByte_Size * 2], RsaByte_Size) != 0)) //如群组地址或密文不一样
        {
            frame_cmd |= 0x40;
            p_buf[Region_DataLenNumber] = 1;
            p_buf[Region_DataAFNNumber] = Parameter_Error;
            transpond_flag = 0;
        }
        else
        {
            p_buf[Region_DataLenNumber] = 0;
            transpond_flag = 1;
        }
    }
    else //通讯帧
    {
        if (memcmp(&p_source[Region_AddrNumber + 1], LANGroup_Addr, 3) != 0) //群组地址不一样
        {
            frame_cmd |= 0x40;
            p_buf[Region_DataLenNumber] = 1;
            //p_buf[Region_DataAFNNumber] = LANG_Addr_Error;
            p_buf[Region_DataAFNNumber] = Data_Error;
            transpond_flag = 0;
        }
        else if ((p_source[Region_SeqNumber] & 0x60) && (Secret_KeyOk_Flag == 0)) //如为加密帧但密钥未ok
        {
            frame_cmd |= 0x40;
            p_buf[Region_DataLenNumber] = 1;
            p_buf[Region_DataAFNNumber] = Key_NotReady;
            transpond_flag = 0;
        }
        else
        {
            p_buf[Region_DataLenNumber] = 0;
            transpond_flag = 1;
        }
    }

    if (transpond_flag) //需要转发就转发出去
    {
        if (p_source[Region_SeqNumber] & 0x60) //
        {
            send_len = Encrypt_Convert(p_source, len, 0); //加密
        }
        else
        {
            send_len = len;
        }

#ifdef Use_Rout
        memmove(RoutFrame_buf, p_source, send_len);
        RoutPath_Read(p_source[Region_AddrNumber]);
        Rout_TempTab.Current_Path_Number = 0;
        Rout_TryBuf.Distance_Addr = p_source[Region_AddrNumber];
        RoutRegion[2] = Rout_TryBuf.Distance_Addr;
        Rout_TryBuf.Frame_Len = send_len;
        Dis_TryFlag = 0;
        //Try_Cnt = 0;
        //Frame_WaitOver_Time = 75;
        Frame_Wait_Cnt = 1;
#endif

        if (memcmp(&p_source[Region_DataIDNumber], Set_LogicAddr_Id, 3) == 0) //组网帧用了默认频道
        {
#ifdef Use_Rout
            Frame_WaitOver_Time = 75 + Network_Delay; //8秒
#endif

            if ((p_source[Region_CmdNumber] & 0x07) == 2) //要唤醒的方式通信
            {
                Si4438_Receive_Start(Default_Channel); //听信道
                SysDelay_Xms(20);
                while (WakeUp_Cnt)
                {
                    WIRELESS_ERROR_CODE = Si4438_Transmit_Start(&Wireless_Buf, Default_Channel, WakeUp_Data, 3);
                    WakeUp_Cnt--;
                    SysDelay_Xms(25);
                    //IWDG_FEED();
                }
            }

			WIRELESS_ERROR_CODE = Si4438_Transmit_Start(&Wireless_Buf, Default_Channel, p_source, send_len);
        }
        else //非组网帧
        {
#ifdef Use_Rout
            Frame_WaitOver_Time = 75;
#endif

            if ((p_source[Region_CmdNumber] & 0x07) == 2) //要唤醒的方式通信
            {
                Si4438_Receive_Start(Wireless_Channel[0]); //听信道
                SysDelay_Xms(20);
                while (WakeUp_Cnt)
                {
                    WIRELESS_ERROR_CODE = Si4438_Transmit_Start(&Wireless_Buf, Wireless_Channel[0], WakeUp_Data, 3);
                    WakeUp_Cnt--;
                    SysDelay_Xms(25);
                    //IWDG_FEED();
                }
            }
            WIRELESS_ERROR_CODE = Si4438_Transmit_Start(&Wireless_Buf, Wireless_Channel[0], p_source, send_len);
        }
    }
    if (transpond_flag)
        memmove(&p_buf[Region_AddrNumber], Self_LogicAddr, 4);
    else
        //按照下行的地址信息上报，经处理发现帧有误
        memmove(&p_buf[Region_AddrNumber], &p_source[Region_AddrNumber], 4); //

    p_buf[Region_SeqNumber] = p_source[Region_SeqNumber]; //
    p_buf[Region_CmdNumber] = frame_cmd | 0x80;           //
    send_len = Frame_Compose(p_buf);                      //
    UpUart_DataTx(p_buf, send_len, &UpCom_TxBuf);         //
}

void RemoteDown_CmdFrame_Process(uint8_t *p_source, uint8_t len)
{
    uint8_t send_len;

    if (memcmp(&p_source[Region_AddrNumber + 1], LANGroup_Addr, 3) != 0)
        return; //群组地址不一样，返回

#ifdef Use_Rout
    Frame_Wait_Cnt = 0;
#endif
    if (p_source[Region_SeqNumber] & 0x60)
    {
        if (!Secret_KeyOk_Flag)
            return;
        send_len = Encrypt_Convert(p_source, len, 1); //做解密给上位机
    }
    else
    {
        send_len = Frame_Compose(p_source);
    }

#ifdef Use_Rout
    if (p_source[Region_SeqNumber] & 0x80)
    {
        //RoutPath_Read(p_source[Region_AddrNumber]);
        RoutPath_Save(&Rout_TempTab, &p_source[Region_DataAFNNumber + p_source[Region_DataLenNumber]], p_source[Region_AddrNumber]); //保存路由表
        send_len = Frame_RoutRegion_Delete(p_source);
    }
#endif
    UpUart_DataTx(p_source, send_len, &UpCom_TxBuf);
}

void RemoteDown_EventFrame_Process(uint8_t *p_source, uint8_t len, HKFrame_TypDef *p_framebuf)
{
#ifdef Use_Rout
    uint8_t rout_region[3 + RoutSeries_Size];
#endif

    uint8_t send_len;
    uint8_t *p_buf = p_framebuf->FrameProcess_Buf;

    if (memcmp(&p_source[Region_AddrNumber + 1], LANGroup_Addr, 3) != 0)
        return; //群组地址不一样，返回

    if (p_framebuf->RetryBuf_Space == 0Xff)
        return;

    if (p_source[Region_SeqNumber] & 0x60)
    {
        if (!Secret_KeyOk_Flag)
            return;
        send_len = Encrypt_Convert(p_source, len, 1); //解密
    }
    else
        send_len = len;
        ///////////////////////////////////////////////////////
#ifdef Use_Rout
    if (p_source[Region_SeqNumber] & 0x80)
    {
        //memmove(rout_region, &p_source[len - (3 +(3+RoutSeries_Size))], 3+RoutSeries_Size);
        memmove(rout_region, &p_source[Region_DataAFNNumber + p_source[Region_DataLenNumber]], 3 + RoutSeries_Size);
        //RoutPath_Read(p_source[Region_AddrNumber]);
        RoutPath_Save(&Rout_TempTab, rout_region, p_source[Region_AddrNumber]);
        send_len = Frame_RoutRegion_Delete(p_source);
    }
#endif
    send_len = Frame_Compose(p_source);
    //////////////////////////////////////////////////
    //Retry_Start(p_framebuf, p_source, send_len);
    UpUart_DataTx(p_source, send_len, &UpCom_TxBuf);

    memmove(&p_buf[Region_AddrNumber], &p_source[Region_AddrNumber], LogicAddr_Len);
    p_buf[Region_SeqNumber] = p_source[Region_SeqNumber];
    p_buf[Region_CmdNumber] = p_source[Region_CmdNumber] & 0x7f; //cmd为0X18
    p_buf[Region_DataLenNumber] = 0;
    send_len = Frame_Compose(p_buf);

    if (p_buf[Region_SeqNumber] & 0x60)
    {
        if (Secret_KeyOk_Flag)
        {
            send_len = Encrypt_Convert(p_buf, send_len, 0); //加密
#ifdef Use_Rout
            if (p_source[Region_SeqNumber] & 0x80)
            {
                send_len = Frame_RoutRegion_Add(p_buf, send_len, rout_region);
            }
#endif
            WIRELESS_ERROR_CODE = Si4438_Transmit_Start(&Wireless_Buf, Wireless_Channel[0], p_buf, send_len);
        }
    }
    else
    {
#ifdef Use_Rout
        if (p_source[Region_SeqNumber] & 0x80)
        {
            send_len = Frame_RoutRegion_Add(p_buf, send_len, rout_region);
        }
#endif
        WIRELESS_ERROR_CODE = Si4438_Transmit_Start(&Wireless_Buf, Wireless_Channel[0], p_buf, send_len);
    }
}

//對幀控制碼各命令的處理           事件帧：受用户干预产生的通讯帧
void Frame_Process(uint8_t *p_source, uint8_t len, HKFrame_TypDef *p_framebuf, DevicePara_TypDef *p_device)
{
    uint8_t frame_type = p_source[Region_CmdNumber] & 0x98;

    switch (frame_type)
    {
    case Local_CmdFrame:
        Local_CmdFrame_Process(p_source, p_framebuf, p_device);
        break;

    case Local_EventFrame:
        Clear_EventFrame_Process(p_source, p_framebuf);
        break;

    case RemoteUp_CmdFrame:
        RemoteUp_CmdFrame_Process(p_source, len, p_framebuf->FrameProcess_Buf);
        break;

    case RemoteUp_EventFrame: //主机要求不重发了，清标志
        Clear_EventFrame_Process(p_source, p_framebuf);
        break;

    case RemoteDown_CmdFrame:
        RemoteDown_CmdFrame_Process(p_source, len);
        break;

    case RemoteDown_EventFrame:
        RemoteDown_EventFrame_Process(p_source, len, p_framebuf);
        break;

    default:
        break;
    }
}

void UpReport_Start(HKFrame_TypDef *p_framebuf)
{
    uint8_t *p = p_framebuf->FrameProcess_Buf;
    uint8_t send_len;

    if (++p_framebuf->Upreport_FrameSeq > 15)
        p_framebuf->Upreport_FrameSeq = 0;
#if (Devece_Attrs)
    memmove(&p[Region_AddrNumber], Self_LogicAddr, LogicAddr_Len);
    p[Region_SeqNumber] = p_framebuf->Upreport_FrameSeq;
    p[Region_CmdNumber] = Local_UpReportCmd;
#else
    memmove(&p[Region_AddrNumber], Self_LogicAddr, LogicAddr_Len);
    p[Region_SeqNumber] = p_framebuf->Upreport_FrameSeq | 0x20;
    p[Region_CmdNumber] = Remote_UpReportCmd;
#endif

    send_len = Frame_Compose(p);

#if (Devece_Attrs)
    UpUart_DataTx(p, send_len, &UpCom_TxBuf);
#else

#endif
}


//电器端使用（重试函数）
void Retransmission_Process(HKFrame_TypDef *p_framebuf)
{
    uint8_t i;
    Retry_TypDef *p_retry;

    if (UpCom_TxBuf.Tx_Status != UartTx_Finished)
        return;
    for (i = 0; i < Retry_Buf_Size; i++)
    {
        p_retry = &p_framebuf->Retry_Buf[i];

        if (p_retry->RetryCnt > 0)
        {
            if (SYSTICK_CNT - p_retry->Retry_StartTime > Frame_RetryTimes)
            {
                UpUart_DataTx(p_retry->Retry_DataBuf, p_retry->Retry_SendLen, &UpCom_TxBuf);
                if (p_retry->RetryCnt)
                    p_retry->RetryCnt--;
                if (p_retry->RetryCnt > 0)
                {
                    //rim();
                    p_retry->Retry_StartTime = SYSTICK_CNT;
                    //sim();
                }
                else
                {
                    p_framebuf->RetryBuf_Space &= ~(1 << p_retry->RetryBuf_Number);
                }
                break;
            }
        }
    }
}

void SysTick_Handle(void)
{
    static uint16_t time_cnt;

    if ((SYSTICK_CNT & 0x07) == 0)
    {
        TICK_8ms = 1;
    }

    if (++time_cnt >= 1000) //1秒
    {
        time_cnt = 0;
        if (++RunningTime_Buf.second >= 60)
        {
            RunningTime_Buf.second = 0;
            if (++RunningTime_Buf.minute >= 60)
            {
                RunningTime_Buf.minute = 0;
                if (++RunningTime_Buf.hour >= 24)
                {
                    uint8_t date_size;

                    RunningTime_Buf.hour = 0;
                    switch (RunningTime_Buf.month)
                    {
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                    case 8:
                    case 10:
                    case 12:
                        date_size = 31;
                        break;

                    case 4:
                    case 6:
                    case 9:
                    case 11:
                        date_size = 30;
                        break;

                    case 2:
                        if ((RunningTime_Buf.year & 0x03) == 0)
                            date_size = 29;
                        else
                            date_size = 28;
                        break;

                    default:
                        break;
                    }
                    if (++RunningTime_Buf.date > date_size)
                    {
                        RunningTime_Buf.date = 1;
                        if (++RunningTime_Buf.month > 12)
                        {
                            RunningTime_Buf.month = 1;
                            if (++RunningTime_Buf.year >= 100)
                                RunningTime_Buf.year = 0;
                        }
                    }
                }
            }
        }
    }
}
