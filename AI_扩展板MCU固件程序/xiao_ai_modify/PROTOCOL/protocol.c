
#include "protocol.h"

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//������������ݱ�ʶ
const uint8_t CMD_INIT[][3]= {
    {0xFF,0xFF,0xFF},
    {0xFF,0xFF,0xFE},
    {0xFF,0xFF,0xFD},
    {0xFF,0xFF,0xFC},
    {0xFF,0xFF,0xFB},
    {0xFF,0x00,0x01},
    {0xFF,0x00,0x00},
};


const uint8_t CMD_STATUE[][3]= {
    {0x00,0x00,0x00},
    {0x00,0x00,0x01},
    {0x00,0x00,0x02},
    {0x00,0x00,0x20},
    {0x00,0x00,0x21},
    {0x00,0x01,0x00},
    {0x00,0x01,0x01},
};

const uint8_t CMD_CTRL[][3]= {
    {0x01,0x00,0x00},
    {0x01,0x00,0x01},
    {0x01,0x00,0x02},
    {0x01,0x00,0x10},
    {0x01,0x00,0x11},
    {0x01,0x00,0x21},
};
const uint8_t CMD_DEVICE_CTRL[][3]= {
    {0x02,0x00,0x01},
    {0x02,0x01,0x01},
    {0x02,0x02,0x01},
    {0x02,0x02,0x02},
    {0x02,0x02,0x03},
    {0x02,0xF0,0x01},
    {0x02,0xF0,0x02},
};


sUartRx_t sUart1Rx;		//����һ������Э����սṹ�����
uint8_t frameNume = 0;		//����֡��ʱʹ�õ�֡���ţ�0-15��
uint8_t wait_frameNum = 0;		//�ȴ��Ļ�Ӧ��֡��

/**
*********************************************************************************************************
*  �� �� ��: vTouchProcess
*  ����˵��: ����Э�����
*  ��    ��: @rx_data ���ڽ��յ�����
			 @pu_buf ���Э�����ݽṹ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vUartRxFrame(uint8_t rx_data, sUartRx_t *pu_buf)
{
    static uint8_t Len_Cnt;
    switch (pu_buf->status)
    {
    case UartRx_FrameHead:
        if (rx_data == HKFreamHeader) //��Կδ���룬������HK֡
        {
            pu_buf->frame_buff[0] = rx_data;
            pu_buf->total_len = 1;
            Len_Cnt = 0;
            pu_buf->time_out_cnt = 1;
            pu_buf->status = UartRx_FrameAddr;
        }
        else if (rx_data == AESFreamHeader)
        {
            pu_buf->frame_buff[0] = rx_data;
            pu_buf->time_out_cnt = 1;
            pu_buf->status = UartRx_AesCmd;
        }
        break;
    case UartRx_FrameAddr:
        pu_buf->frame_buff[pu_buf->total_len] = rx_data;
        pu_buf->total_len++;
        if (++Len_Cnt >= LogicAddr_Len)
            pu_buf->status = UartRx_FrameSeq;
        break;

    case UartRx_FrameSeq:
        pu_buf->frame_buff[pu_buf->total_len] = rx_data;
        pu_buf->total_len++;
        pu_buf->status = UartRx_DataCmd;
        break;

    case UartRx_DataCmd:
        pu_buf->frame_buff[pu_buf->total_len] = rx_data;
        pu_buf->total_len++;
        pu_buf->status = UartRx_Datalen;
        break;

    case UartRx_Datalen:
        if (rx_data > UART_BUFF_LEN)
        {
            pu_buf->time_out_cnt = 0;
            pu_buf->status = UartRx_FrameHead;
        }
        else
        {
            pu_buf->frame_buff[pu_buf->total_len] = rx_data;
            pu_buf->total_len++;
            Len_Cnt = 0;
            if (rx_data == 0)
                pu_buf->status = UartRx_FrameCs;
            else
                pu_buf->status = UartRx_Data;
        }
        break;
    case UartRx_Data:
        pu_buf->frame_buff[pu_buf->total_len] = rx_data;
        pu_buf->total_len++;
        if (++Len_Cnt >= pu_buf->frame_buff[Region_DataLenNumber])
        {
            Len_Cnt = 0;
            pu_buf->status = UartRx_FrameCs;
        }

        break;

    case UartRx_FrameCs:
        pu_buf->frame_buff[pu_buf->total_len] = rx_data;
        pu_buf->total_len++;
        if (++Len_Cnt >= 2)
            pu_buf->status = UartRx_FrameEnd;
        break;

    case UartRx_FrameEnd:
        if (rx_data == HKFreamEnd) //�жϽ������Ƿ���ȷ
        {
            //UpCom_RXINT_DIS(); //���ж�
            pu_buf->frame_buff[pu_buf->total_len] = rx_data;
            pu_buf->total_len++;
            pu_buf->time_out_cnt = 0;
            pu_buf->status = UartRx_Finished;
        }
        else
        {
            pu_buf->time_out_cnt = 0;
            pu_buf->status = UartRx_FrameHead;
        }
        break;
    ///////////////////////////����AES֡ //////////////////
    case UartRx_AesCmd:                                                      //���ڽ�����������
        if (rx_data == GKWrite_Cmd_Request || rx_data == MACWrite_Cmd_Request) //ֻ�ܽ����չ�Կ����
        {
            pu_buf->frame_buff[1] = rx_data;
            pu_buf->status = UartRx_Aeslen;
        }
        else
        {
            pu_buf->time_out_cnt = 0;
            pu_buf->status = UartRx_FrameHead;
        }
        break;

    case UartRx_Aeslen: //���ڽ��ճ����ֽ�
        if (rx_data == 0)
        {
            pu_buf->frame_buff[2] = rx_data;
            pu_buf->total_len = 3;
            pu_buf->status = UartRx_AesFrameCs1;
        }
        else if (rx_data > GK_Data_Len) //ֻ�ܽ����չ�Կ����
        {
            pu_buf->time_out_cnt = 0;
            pu_buf->status = UartRx_FrameHead; //���մ��󣬷��ص�����֡ͷ����ʼ״̬
        }

        else
        {
            pu_buf->frame_buff[2] = rx_data;
            pu_buf->total_len = 3;
            pu_buf->status = UartRx_AesData;
        }
        break;

    case UartRx_AesData: //���ڽ�������
        pu_buf->frame_buff[pu_buf->total_len++] = rx_data;
        if (pu_buf->total_len >= pu_buf->frame_buff[2] + 3)
            pu_buf->status = UartRx_AesFrameCs1;
        break;

    case UartRx_AesFrameCs1: //���ڽ���У���
        pu_buf->frame_buff[pu_buf->total_len++] = rx_data;
        pu_buf->status = UartRx_AesFrameCs2;
        break;

    case UartRx_AesFrameCs2: //���ڽ���У���
        pu_buf->frame_buff[pu_buf->total_len++] = rx_data;
        //UART_RXDIS();    //ֹͣ����
        //UpCom_RXINT_DIS(); //���ж�
        pu_buf->time_out_cnt = 0;
        pu_buf->status = UartRx_Finished;
        break;

    default:
        break;
    }
}

/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdInitDeal
*  ����˵��: ��ʼ�����ݱ�ʶ��������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/


/**
*********************************************************************************************************
*  �� �� ��: Frame_Compose
*  ����˵��: ����һ֡����Э���֡����
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Frame_Compose(uint8_t *p)
{
    uint8_t ComposeFrame_Len;
    uint16_t crc16_val;


    p[Region_HeaderNumber] = HKFreamHeader;

    p[Region_CmdNumber] &= ~(1 << 5);
    ComposeFrame_Len = p[Region_DataLenNumber] + (Region_DataLenNumber + 1);
    crc16_val = CRC16_2(p, ComposeFrame_Len);
    p[ComposeFrame_Len] = crc16_val >> 8;
    ComposeFrame_Len++;
    p[ComposeFrame_Len] = crc16_val & 0xff;
    ComposeFrame_Len++;
    p[ComposeFrame_Len] = HKFreamEnd;
    ComposeFrame_Len++;

    return ComposeFrame_Len;
}

//
/**
*********************************************************************************************************
*  �� �� ��: FrameCmdLocalAck
*  ����˵��: ��չ������Ӧ�����׿��
*  ��    ��: @cmdBuff ���յ���Э����������
			 @cmdLen ���յ���Э�������
			 @userDat Ҫ�ϱ����û�����
			 @userLen Ҫ�ϱ������ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void FrameCmdLocalAck(uint8_t *cmdBuff,uint8_t cmdLen,uint8_t *userDat,uint8_t userLen)
{
    uint8_t temp[50]= {0};
    uint8_t frameLen  = 0;

    if(cmdLen > 50)return;
    memcpy(temp, cmdBuff, cmdLen);
    temp[Region_CmdNumber] |= FCMD_DIR_BIT;	//֡���䷽��
    if(userLen>0)
    {
        temp[Region_DataLenNumber] = userLen+4;	//֡���� (���ݹ�����(1)+���ݱ�ʶ(3)+�û�����(userLen))
    }
    else
    {
        temp[Region_DataLenNumber] = 0;
    }
    memcpy(&temp[Region_DataValNumber],userDat,userLen); //�����û�����
    frameLen = Frame_Compose(temp);

    UseComSendBytes(temp, frameLen);
}
/**
*********************************************************************************************************
*  �� �� ��: vAesSave
*  ����˵��: �����ʼ��Э������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vAES_Save(uint8_t *buff,uint8_t len)
{
    STMFLASH_Write(DEVICE_AES_SAVE_ADDR,(uint16_t*)&buff[Region_DataValNumber],sizeof(Device_aes_t)/2);     //д��
    STMFLASH_Read(DEVICE_AES_SAVE_ADDR,(uint16_t*)&deviceInfo.aes_field,sizeof(Device_aes_t)/2);             //����
    if(memcmp(&deviceInfo.aes_field,&buff[Region_DataValNumber],sizeof(Device_aes_t))==0)                    //�Ա�
    {
        DebugPrintf("\n���ı���ɹ�");
    }
    else
    {
        DebugPrintf("\n���ı���ʧ��");
    }
}


/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdInitDeal
*  ����˵��: �����ʼ��Э������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void  vUartFrameCmdInitDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2 = (buff[10]<<8)+buff[11];
    uint8_t report_flag = 1;		//��ʾ�Ƿ�Ҫִ���ϱ���1ִ�У�0��ִ��
    switch(cmd2)
    {
    case 0xFFFF:
        DebugPrintf("\n�豸����");
        vDeviceMatchNet(buff,len);
        break;
    case 0xFFFE:
        DebugPrintf("\n�豸�б�");
        vDeviceListSave(&buff[Region_DataValNumber],buff[Region_DataLenNumber]-4);
        break;
    case 0xFFFD:
        break;
    case 0xFFFC:
        break;
    case 0xFFFB:
        break;
    case 0xFF00:
        DebugPrintf("\n�����·�");
        vAES_Save(buff,len);
        AES_Init();                 //��������Ч
        break;
    case 0x0001:        //��վҪ���ȡ����汾��
        vVersionReport(buff,len,Version_Number);
        report_flag = 0;	//Ҫ����治Ҫ�ϱ���
        break;
    case 0x0000:		//Уʱ

        break;
    default:
        break;
    }
    if(report_flag == 1)
        FrameCmdLocalAck(buff,len,0,0);         //Ӧ��
}

/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdStateDeal
*  ����˵��: ����״̬��Э������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vUartFrameCmdStateDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2 = (buff[10]<<8)+buff[11];
    uint8_t report_flag = 1;		//��ʾ�Ƿ�Ҫִ���ϱ���1ִ�У�0��ִ��
    switch(cmd2)
    {
    case 0x0000:
        DebugPrintf("\n���豸MAC��ַ");
        vDeviceMacReport(buff,len,deviceInfo.mac_frame.mac);
        report_flag = 0;	//Ҫ����治Ҫ�ϱ���
        break;
    case 0x0001:
        DebugPrintf("\nȷ���豸�Ƿ�����");
        break;
    case 0x0002:

        break;
    case 0x0020:
        break;
    case 0x0021:
        break;
    case 0x0100:
        DebugPrintf("\n��س��״̬");
        break;
    case 0x0101:
        break;
    default:
        break;
    }
    if(report_flag == 1)
        FrameCmdLocalAck(buff,len,0,0);         //Ӧ��
}
/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdClassify
*  ����˵��: ���������Э������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vUartFrameCmdCtrlDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2;
    uint8_t report_flag = 1;		//��ʾ�Ƿ�Ҫִ���ϱ���1ִ�У�0��ִ��

    cmd2 = (buff[10]<<8)+buff[11];

    switch(cmd2)
    {
    case 0x0000:

        break;
    case 0x0001:

        break;
    case 0x0002:

        break;
    case 0x0010:
        break;
    case 0x0011:
        break;
    case 0x0021:
        //DebugPrintf("\n�������LEDģʽ����");
        xQueueSend(xQueueLedTask, &buff[Region_DataValNumber], (TickType_t)10);			//ͨ�����䷢�ͺ��İ����LED��Э��֡��LED����
        break;
    default:
        break;
    }
    if(report_flag == 1)
        FrameCmdLocalAck(buff,len,0,0);         //Ӧ��
}

/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdDeviceFuncCtrlDeal
*  ����˵��: �������ܿ���Э�������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vUartFrameCmdDeviceFuncCtrlDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2 = (buff[10]<<8)+buff[11];

    switch(cmd2)
    {
    case 0x0001:
        break;
    case 0x0101:
        break;
    case 0x0201:
        break;
    case 0x0202:
        break;
    case 0x0203:
        break;
    case 0xF001:
        break;
    case 0xF002:
        break;
    default:
        break;
    }
}


/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdSensorDeal
*  ����˵��: ��������Э�������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vUartFrameCmdSensorDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2 = (buff[10]<<8)+buff[11];

    switch(cmd2)
    {
    case 0x0001:
        break;
    case 0x0002:
        break;
    case 0x0003:
        break;
    case 0x0004:
        break;
    case 0x0101:
        break;
    case 0x0102:
        break;
    case 0x0103:
        break;
    case 0x0104:
        break;
    case 0xFE01:
        break;
    case 0xFFFF:
        break;
    default:
        break;
    }
}
/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdDeviceFuncCtrlDeal
*  ����˵��: �Ž���Э�������
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vUartFrameCmdEntranceGuardDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2 = (buff[10]<<8)+buff[11];

    switch(cmd2)
    {
    case 0x0001:
        break;
    default:
        break;

    }

}

/**
*********************************************************************************************************
*  �� �� ��: vUartFrameCmdClassify
*  ����˵��: ���಻ͬ�Ĵ�������Э��֡
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vUartFrameCmdClassify(uint8_t *buff,uint8_t len)
{
    uint8_t cmd1 = buff[9];

    switch(cmd1)
    {
    case 0xFF:
        vUartFrameCmdInitDeal(buff,len);
        break;
    case 0x00:
        vUartFrameCmdStateDeal(buff,len);
        break;
    case 0x01:
        vUartFrameCmdCtrlDeal(buff,len);
        break;
    case 0x02:
        vUartFrameCmdDeviceFuncCtrlDeal(buff,len);
        break;
    case 0x03:
        vUartFrameCmdSensorDeal(buff,len);
        break;
    case 0x04:
        vUartFrameCmdEntranceGuardDeal(buff,len);
        break;
    default:
        break;
    }
}

/**
*********************************************************************************************************
*  �� �� ��: vUartAesCmdDeal
*  ����˵��: ������AesЭ��֡����¼����MAC��ַ��
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
static void vUartAesCmdDeal(uint8_t *buff,uint8_t len)
{
    uint8_t i = 0;
    uint8_t cmd1 = buff[1];

    switch(cmd1)
    {
    case 0x20:
        DebugPrintf("\r\n��¼����MAC��ַ MAC = ");
        for(i=0; i<8; i++)
        {
            printf("%02X ",buff[i+3]);
        }
        STMFLASH_Write(DEVICE_MAC_SAVE_ADDR,(uint16_t*)buff,sizeof(Device_mac_t)/2);  //д��MAC��ַ
        STMFLASH_Read(DEVICE_MAC_SAVE_ADDR,(uint16_t*)&deviceInfo.mac_frame,sizeof(Device_mac_t)/2);    //����MAC��ַ
        if(memcmp(&deviceInfo.mac_frame,buff,sizeof(Device_mac_t)/2)==0)              //�Ա�MAC��ַ
        {
            DebugPrintf("\r\nMAC��ַ��¼�ɹ�");
        }
        else
        {
            DebugPrintf("\r\nMAC��ַ��¼ʧ��");
        }

        break;
    case 0x21:
        break;
    case 0x22:
        break;
    default:
        break;

    }


}
/**
*********************************************************************************************************
*  �� �� ��: uFrameCmd_CRC_Check
*  ����˵��: �жϴ���֡�����Ƿ���ȷ
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
eFrameCheckType xUartFrameCmdCheck(uint8_t *buff,uint8_t len)
{
    uint16_t crc_16;
    uint8_t crc16_h;
    uint8_t crc16_l;

    if(buff[0] != HKFreamHeader)		//֡ͷ�Ƿ�Ϊ0xAC
        return FRAME_ERR;
    crc_16 = CRC16_2(buff,len-3);
    crc16_h = (uint8_t)(crc_16 >> 8);
    crc16_l = (uint8_t)(crc_16 & 0x00ff);
    if((crc16_h == buff[len-3])&&(crc16_l == buff[len-2]))
        return FRAME_OK;
    else
        return FRAME_ERR;
}

/**
*********************************************************************************************************
*  �� �� ��: uFrameCmd_CRC_Check
*  ����˵��: �жϴ���֡�����Ƿ���ȷ
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
eFrameCheckType xUartAesCmdCheck(uint8_t *buff,uint8_t len)
{
    uint16_t crc_16;
    uint8_t crc16_h;
    uint8_t crc16_l;

    if(buff[0] != AESFreamHeader)		//֡ͷ�Ƿ�Ϊ0x68
        return FRAME_ERR;
    crc_16 = CRC16_2(buff,len-2);
    crc16_h = (uint8_t)(crc_16 >> 8);
    crc16_l = (uint8_t)(crc_16 & 0x00ff);
    if((crc16_h == buff[len-2])&&(crc16_l == buff[len-1]))
        return FRAME_OK;
    else
        return FRAME_ERR;
}

/**
*********************************************************************************************************
*  �� �� ��: vUartFrameProcess
*  ����˵��: ������¼MACЭ�鴦��
*  ��    ��: @pbuff ���Э�����ݽṹ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vUartAesProcess(sUartRx_t *pbuff)
{
    if(pbuff->status == UartRx_Finished)
    {
        if(xUartAesCmdCheck(pbuff->frame_buff,pbuff->total_len) == FRAME_OK)
        {
            //UartSendBytes(USART1, pbuff->frame_buff,pbuff->total_len);
            vUartAesCmdDeal(pbuff->frame_buff,pbuff->total_len);
        }
        pbuff->status = UartRx_FrameHead;
    }

}

/**
*********************************************************************************************************
*  �� �� ��: vUartFrameProcess
*  ����˵��: ����Ӧ��Э�鴦��
*  ��    ��: @pbuff ���Э�����ݽṹ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/

void vUartFrameProcess(sUartRx_t *pbuff)
{
    uint8_t cmdflag = 0;

    if(pbuff->status == UartRx_Finished)
    {
        if(xUartFrameCmdCheck(pbuff->frame_buff,pbuff->total_len) == FRAME_OK)
        {
            cmdflag = pbuff->frame_buff[Region_CmdNumber] & (FCMD_REMOTE_BIT|FCMD_EVNT_BIT);
            switch(cmdflag)
            {
            case 0x00:     	//������ͨ
				DebugPrintf("\n������ͨ֡");
				vUartFrameCmdClassify(pbuff->frame_buff,pbuff->total_len);
                break;
            case 0x08:     	//�����¼�(��Ҫ�Ǻ��İ��Ӧ��)
				DebugPrintf("\n�����¼�֡");
                break;
            case 0x10:		//Զ����ͨ(�����豸�Ŀ���)
				DebugPrintf("\nԶ����ͨ֡");
				vFrameUartRemoteCmdDeal(pbuff->frame_buff,pbuff->total_len);
                break;
            case 0x18:		//Զ���¼�
				DebugPrintf("\nԶ���¼�֡");
                break;
            }
        }
        pbuff->status = UartRx_FrameHead;
    }
}

/**
*********************************************************************************************************
*  �� �� ��: uFrameCmd_CRC_Check
*  ����˵��: �ж�����֡�����Ƿ���ȷ
*  ��    ��: @buff Ҫ���������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
eFrameCheckType xWirelessRouterCmdCheck(uint8_t *buff,uint8_t len)
{
    uint16_t crc_16;
    uint8_t crc16_h;
    uint8_t crc16_l;

    if((buff[0] != ROUTER_FRAME_HDADER)||(buff[1] != ROUTER_FRAME_HDADER))		//֡ͷ�Ƿ�Ϊ0x69 0x69
        return FRAME_ERR;
		
	if((buff[2] + buff[3]) != 0xFF)		//֡���Ⱥͳ���ȡ��
        return FRAME_ERR;
		
	if((buff[len-2] != ROUTER_FRAME_END)||(buff[len-1] != ROUTER_FRAME_END)) //֡β�Ƿ�Ϊ0x96 0x96
		return FRAME_ERR;
		
		
    crc_16 = CRC16_2(buff,len-4);
    crc16_h = (uint8_t)(crc_16 >> 8);
    crc16_l = (uint8_t)(crc_16 & 0x00ff);
    if((crc16_h == buff[len-4])&&(crc16_l == buff[len-3]))
        return FRAME_OK;
    else
        return FRAME_ERR;
}



/**
*********************************************************************************************************
*  �� �� ��: vMastAckToDevice
*  ����˵��: ��չ��Ӧ������豸
*  ��    ��: @buff ���յ������豸����������
			 @len  ���ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vMastAckToDevice(uint8_t *buff,uint8_t len)
{

	QUEUE_WIRELESS_SEND_t queueMsg;
	
	memcpy(queueMsg.msg, buff,len);
	queueMsg.toCh = Wireless_Channel[0];
	
	queueMsg.msg[Region_CmdNumber] &= ~FCMD_DIR_BIT;    //���䷽�����㣬��ʾ��վ����
	queueMsg.msg[Region_DataLenNumber] = 0;		//Ӧ�����ݳ���Ϊ0
	//queueMsg.msg[Region_SeqNumber] &= 0xf0;
	
	queueMsg.len = Frame_Compose(queueMsg.msg);
		//����
	if((queueMsg.msg[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
	{
	     DebugPrintf("\n��Ҫ����");
		 
	     Encrypt_Convert(queueMsg.msg,queueMsg.len ,&queueMsg.len, 1);   //����
	}
	else
	{
	    DebugPrintf("\n���ü���");
	}
	xQueueSend(xQueueWirelessTx,&queueMsg, (TickType_t)10);			//ֱ�ӷ������߷�������			
}

///**
//*********************************************************************************************************
//*  �� �� ��: vMastRouterFrameDela
//*  ����˵��: ������վ����ȥ����������
//*  ��    ��: @buff Ҫ���������
//			 @len  ���ݳ���
//*  �� �� ֵ: ��
//*********************************************************************************************************
//*/
//void vMastRouterFrameDela(uint8_t *buff,uint8_t len)
//{
//	uint8_t frame_type = 0;  //֡����(0������1ͨ��)
//	FRAME_ROUTER_SLAVE_CMD_t *pbuff;
//	
//	pbuff = (FRAME_ROUTER_SLAVE_CMD_t*)buff;
//	frame_type = pbuff->ctrl.type;
//	if(frame_type == 1)  //ͨ��
//	{
//		DebugPrintf("\n·��ͨ��֡");
//	}
//	else		//����
//	{
//	   DebugPrintf("\n·������֡");
//	} 
//}


///**
//*********************************************************************************************************
//*  �� �� ��: vSlaveRouterFrameDela
//*  ����˵��: �����վ����ȥ����������
//*  ��    ��: @buff Ҫ���������
//			 @len  ���ݳ���
//*  �� �� ֵ: ��
//*********************************************************************************************************
//*/
//void vSlaveRouterFrameDela(uint8_t *buff,uint8_t len)
//{
//	uint8_t frame_type = 0;  //֡����(0������1ͨ��)
//	FRAME_ROUTER_SLAVE_CMD_t *pbuff;
//	
//	pbuff = (FRAME_ROUTER_SLAVE_CMD_t*)buff;
//	frame_type = pbuff->ctrl.type;
//	if(frame_type == 1)  //ͨ��
//	{
//		DebugPrintf("\n·��ͨ��֡");
//		vMastAckToDevice(buff,len);
//	}
//	else		//����
//	{
//	   DebugPrintf("\n·������֡");
//	} 
//}

/**
*********************************************************************************************************
*  �� �� ��: DeviceWireless2Uart()
*  ����˵��: ���յ��豸���������ݾ�������ͽ��ܺ�ת��������
*  ��    ��: @pframe �յ��ĵ�����������
			@frame_len     �յ��ĵ����������ݳ���
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void DeviceWireless2Uart(uint8_t *pframe, uint8_t frame_len)
{
	//����
//	if(frame_len<70)
//	{
//		if((pframe[0]==0x69)&&(pframe[1]==0x69))
//		{
//			FrameRouteData_74Convert(pframe,frame_len,&frame_len,0);
//		}
//		else if(pframe[0]==0xAC)
//		{
//			FrameData_74Convert(pframe,frame_len,&frame_len,0);
//		}
//	}
//	
//	//����
//	if((pframe[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
//	{
//		 DebugPrintf("\n��Ҫ����");
//		 
//		 Encrypt_Convert(pframe,frame_len , &frame_len, 0);   //����
//	}
//	else
//	{
//		DebugPrintf("\n���ý���");
//	}
	UseComSendBytes(pframe, frame_len);
}

/**
*********************************************************************************************************
*  �� �� ��: vUartFrameProcess
*  ����˵��: �������߽��յ���һ֡����
*  ��    ��: @wireless ���Э�����ݽṹ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
extern TaskHandle_t StartTask_Handler;
void vWirelessFrameDeal(WLS *wireless)
{
	uint8_t *pframe = wireless->Wireless_RxData;
	uint8_t frame_len = wireless->Wireless_PacketLength;
	uint8_t frame_flag = 0;
	uint8_t cmdUpFlag = 1;		//�Ƿ��ϱ�������
	uint8_t index = 0;       //Ӧ�����ݣ�0xACЭ��֡�ģ���ʼλ��
	
	
	//����
	if(xWirelessRouterCmdCheck(pframe,frame_len) == FRAME_OK)              //·��Э��
	{
		frame_len = pframe[2];
		if(frame_len<140)
		{
			FrameRouteData_74Convert(pframe,wireless->Wireless_PacketLength,&frame_len,0);
		}

		index = 13+pframe[12];//���Ӧ�����ݵĿ�ʼλ��  (0x69Э�鿪ʼ��13���ֽ�+·�ɱ���)
        			//����
		if(((pframe+index)[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
		{
			 DebugPrintf("\n��Ҫ����");
			 
			 Encrypt_Convert(pframe+index,frame_len-index-4 , &frame_len, 0);   //����
		}
		else
		{
			DebugPrintf("\n���ý���");
		}
		
		frame_flag =  (pframe[4] & 0x81);                //Э����վ��վ��������ͨ��֡λ�ж�
		switch(frame_flag)
		{
			case 0x00:      //��վ����
				frame_flag =  (pframe[4] & 0x06);                //������ͨ��ģʽ
				switch(frame_flag)
				{
				    case 0x00:		//��������������֡
						break;
					case 0x02:      //�������͹���֡
						break;
					case 0x04:      //������������֡
						cmdUpFlag = 0;       //������ͨ�����ڷ���
						break;
					case 0x06:      //�����͹���֡
						break;	
				}
				break;
			case 0x01:		//��վͨ��
				frame_flag =  (pframe[4] & 0x06);                //������ͨ��ģʽ
				switch(frame_flag)
				{
				    case 0x00:		//��������������֡
						break;
					case 0x02:      //�������͹���֡
						break;
					case 0x04:      //������������֡
						cmdUpFlag = 0;       //������ͨ�����ڷ���
						break;
					case 0x06:      //�����͹���֡
						break;	
				}
				break;
			case 0x80:		//��վ����
				frame_flag =  (pframe[4] & 0x06);                //������ͨ��ģʽ
				switch(frame_flag)
				{
				    case 0x00:      //��������������֡
						//if(wait_frameNum == (pframe[Region_SeqNumber]&0x0f))
						{
							xTaskNotify(StartTask_Handler,0x00000002,eSetBits);   //�豸�����ɹ�Ӧ��
						}
						UseComSendBytes(pframe+index, (pframe+index)[Region_DataLenNumber]+11);
						cmdUpFlag = 0;       //������ͨ�����ڷ���
						DebugPrintf("\n��վ��Ӧ�����ɹ�");
						break;
					case 0x02:      //�������͹���֡
						break;
					case 0x04:      //������������֡
						cmdUpFlag = 0;       //������ͨ�����ڷ���
						break;
					case 0x06:      //�����͹���֡
						break;	
				}
				break;
			case 0x81:		//��վͨ��
				frame_flag =  (pframe[4] & 0x06);                //������ͨ��ģʽ
				
				switch(frame_flag)
				{
				    case 0x00:      //��������������֡
						if(((pframe+index)[Region_DataIDNumber]==0xFF)&&		//������֡��Ӧ
							((pframe+index)[Region_DataIDNumber+1]==0xFF)&&
							((pframe+index)[Region_DataIDNumber+2]==0xFF))
						{
							//if(wait_frameNum == (pframe[Region_SeqNumber]&0x0f))
							{
								xTaskNotify(StartTask_Handler,0x00000001,eSetBits);   //�豸�����ɹ�Ӧ��
							}
						}
						else
						{
							DebugPrintf("\n");
						}
						
						break;
					case 0x02:      //�������͹���֡
						break;
					case 0x04:      //������������֡
						cmdUpFlag = 0;       //������ͨ�����ڷ���
						break;
					case 0x06:      //�����͹���֡
						break;	
				}
				break;		
		}
		if(cmdUpFlag == 1)    //��Ҫ����������
		{	
			UseComSendBytes(pframe+index, (pframe+index)[Region_DataLenNumber]+11);
		}
		
	}
	else if(pframe[0]==0xAC)
	{
		frame_len = pframe[Region_DataLenNumber];
		if(frame_len<70)
		{
			FrameData_74Convert(pframe,wireless->Wireless_PacketLength,&frame_len,0);
		}
			//����
		if((pframe[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
		{
			 DebugPrintf("\n��Ҫ����");
			 
			 Encrypt_Convert(pframe,frame_len , &frame_len, 0);   //����
		}
		else
		{
			DebugPrintf("\n���ý���");
		}
		
		if(xUartFrameCmdCheck(pframe,frame_len) == FRAME_OK)          //��·��Э�飬��Э��
		{
			frame_flag = pframe[Region_CmdNumber]&0x98;
			
			switch(frame_flag)
			{
				case 0x10:  		//��վ����ͨ֡
					DebugPrintf("\n��Э�飬��վ����ͨ֡");
					break;
				case 0x18:          //��վ���¼�֡
					DebugPrintf("\n��Э�飬��վ���¼�֡");
					break;
				case 0x90:          //��վ����ͨ֡
					DebugPrintf("\n��Э�飬��վ����ͨ֡");
					if(wait_frameNum == (pframe[Region_SeqNumber]&0x0f))
					{
						if((pframe[Region_DataIDNumber]==0xFF)&&		//������֡��Ӧ
						(pframe[Region_DataIDNumber+1]==0xFF)&&
						(pframe[Region_DataIDNumber+2]==0xFF))
						{
							DebugPrintf("\n�豸�����ɹ�Ӧ��");
							xTaskNotify(StartTask_Handler,0x00000002,eSetBits);   //�豸�����ɹ�Ӧ��
						}
						else
						{

							xTaskNotify(StartTask_Handler,0x00000001,eSetBits);   //�豸ͨ��Ӧ��ɹ�
						}
					}
					
					break;
				case 0x98:          //��վ���¼�֡
					DebugPrintf("\n��Э�飬��վ���¼�֡");
					vMastAckToDevice(pframe,frame_len);
					break;
			}

		}
		if(cmdUpFlag == 1)    //��Ҫ����������
		{	
			UseComSendBytes(pframe, frame_len);
		}
	}
	
	

}










