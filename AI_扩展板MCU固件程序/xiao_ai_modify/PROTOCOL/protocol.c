
#include "protocol.h"

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//各个命令的数据标识
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


sUartRx_t sUart1Rx;		//定义一个串口协议接收结构体变量
//sCarrierRx_t sUart2Rx=     //定义一个串口协议接收结构体变量
//{
//    Carrier_FrameHead1,
//    0,
//	0,
//    {0},
//    0,

//};
sUartRx_t sUart2Rx =
{
    UartRx_FrameHead,
	0,
	{0},
	0,
};
uint8_t frameNume = 0;		//发送帧命时使用的帧包号（0-15）
uint8_t wait_frameNum = 0;		//等待的回应的帧号

/**
*********************************************************************************************************
*  函 数 名: vTouchProcess
*  功能说明: 串口协议接收
*  形    参: @rx_data 串口接收的数据
			 @pu_buf 存放协议数据结构体
*  返 回 值: 无
*********************************************************************************************************
*/
void vUartRxFrame(uint8_t rx_data, sUartRx_t *pu_buf,uint8_t mode)
{
    static uint8_t Len_Cnt;
    switch (pu_buf->status)
    {
    case UartRx_FrameHead:
        if (rx_data == HKFreamHeader) //公钥未烧入，不接收HK帧
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
		if(mode == 0)
		{
		    if (++Len_Cnt >= pu_buf->frame_buff[Region_DataLenNumber])
			{
				Len_Cnt = 0;
				pu_buf->status = UartRx_FrameCs;
			}
		}
		else if(mode == 1)
		{
			if (++Len_Cnt >= (pu_buf->frame_buff[Region_DataLenNumber]/16+16))   //加密过后的数据协议长度和实际收到的数据长度关系是 实际长度 ＝ 长度/16+16
			{
				Len_Cnt = 0;
				pu_buf->status = UartRx_FrameCs;
			}
		}
       

        break;

    case UartRx_FrameCs:
        pu_buf->frame_buff[pu_buf->total_len] = rx_data;
        pu_buf->total_len++;
        if (++Len_Cnt >= 2)
            pu_buf->status = UartRx_FrameEnd;
        break;

    case UartRx_FrameEnd:
        if (rx_data == HKFreamEnd) //判断结束符是否正确
        {
            //UpCom_RXINT_DIS(); //关中断
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
        ///////////////////////////接收AES帧 //////////////////
    case UartRx_AesCmd:                                                      //正在接收数据命令
        if (rx_data == GKWrite_Cmd_Request || rx_data == MACWrite_Cmd_Request) //只能接受烧公钥命令
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

    case UartRx_Aeslen: //正在接收长度字节
        if (rx_data == 0)
        {
            pu_buf->frame_buff[2] = rx_data;
            pu_buf->total_len = 3;
            pu_buf->status = UartRx_AesFrameCs1;
        }
        else if (rx_data > GK_Data_Len) //只能接受烧公钥命令
        {
            pu_buf->time_out_cnt = 0;
            pu_buf->status = UartRx_FrameHead; //接收错误，返回到接收帧头的起始状态
        }

        else
        {
            pu_buf->frame_buff[2] = rx_data;
            pu_buf->total_len = 3;
            pu_buf->status = UartRx_AesData;
        }
        break;

    case UartRx_AesData: //正在接收数据
        pu_buf->frame_buff[pu_buf->total_len++] = rx_data;
        if (pu_buf->total_len >= pu_buf->frame_buff[2] + 3)
            pu_buf->status = UartRx_AesFrameCs1;
        break;

    case UartRx_AesFrameCs1: //正在接收校验和
        pu_buf->frame_buff[pu_buf->total_len++] = rx_data;
        pu_buf->status = UartRx_AesFrameCs2;
        break;

    case UartRx_AesFrameCs2: //正在接收校验和
        pu_buf->frame_buff[pu_buf->total_len++] = rx_data;
        //UART_RXDIS();    //停止接收
        //UpCom_RXINT_DIS(); //关中断
        pu_buf->time_out_cnt = 0;
        pu_buf->status = UartRx_Finished;
        break;

    default:
        break;
    }
}

/**
*********************************************************************************************************
*  函 数 名: Carrier2UartFrameRec
*  功能说明: 载波串口接收数据
*  形    参: @rx_data 串口接收的数据
			 @pu_buf 存放协议数据结构体
*  返 回 值: 无
*********************************************************************************************************
*/
void Carrier2UartFrameRec(uint8_t rec, sCarrierRx_t *pu_buf)
{
//    pu_buf->time_out_cnt = 0;
//    pu_buf->frame_buff[pu_buf->total_len] = rec;
//    pu_buf->total_len ++;
	static uint8_t frame_len = 0;
    switch (pu_buf->status)
    {
    case Carrier_FrameHead1:
        if(rec == 0x69)
        {
			pu_buf->rec_len = 0;
			frame_len = 0;
			
			pu_buf->frame_buff[pu_buf->rec_len++] = rec;
			pu_buf->status = Carrier_FrameHead2;
        }
        break;
    case Carrier_FrameHead2:
		if(rec == 0x69)
        {
			pu_buf->frame_buff[pu_buf->rec_len++] = rec;
			pu_buf->status = Carrier_Datalen;
        }
		else
		{
		   pu_buf->status = Carrier_FrameHead1;
		}
        break;
    case Carrier_Datalen:
		frame_len = rec;
		pu_buf->frame_buff[pu_buf->rec_len++] = rec;
		pu_buf->status = Carrier_Datalen_c;
        break;
    case Carrier_Datalen_c:
		if((rec + frame_len)==0xff)
		{
			pu_buf->frame_buff[pu_buf->rec_len++] = rec;
		   pu_buf->status = Carrier_Data;
		}
		else
		{
			pu_buf->status = Carrier_FrameHead1;
		}
        break;
	case Carrier_Data:
		 pu_buf->frame_buff[pu_buf->rec_len++] = rec;
		if(pu_buf->rec_len == frame_len)
		{
			pu_buf->status = Carrier_FrameCRCH;
		}
		
		break;
    case Carrier_FrameCRCH:
		pu_buf->frame_buff[pu_buf->rec_len++] = rec;
		pu_buf->status = Carrier_FrameCRCL;
        break;
    case Carrier_FrameCRCL:
		pu_buf->frame_buff[pu_buf->rec_len++] = rec;
		pu_buf->status = Carrier_FrameEnd1;
        break;
    case Carrier_FrameEnd1:
		if(rec == 0x96)
		{
		   pu_buf->frame_buff[pu_buf->rec_len++] = rec;
		    pu_buf->status = Carrier_FrameEnd2;
		}
		else
		{
		  pu_buf->status = Carrier_FrameHead1;
		}
		
        break;
    case Carrier_FrameEnd2:
		if(rec == 0x96)
		{
		   pu_buf->frame_buff[pu_buf->rec_len++] = rec;
		   pu_buf->status = Carrier_Finished;
		   pu_buf->total_len = pu_buf->rec_len;
		}
		else
		{
		  pu_buf->status = Carrier_FrameHead1;
		}
        break;
    default:
        break;
    }
    if(pu_buf->rec_len > (UART_BUFF_LEN-1))
    {
        pu_buf->rec_len = 0;
    }

}


/**
*********************************************************************************************************
*  函 数 名: vUartFrameCmdInitDeal
*  功能说明: 初始化数据标识相关命令处理
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/


/**
*********************************************************************************************************
*  函 数 名: Frame_Compose
*  功能说明: 整合一帧符合协议的帧数据
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
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
*  函 数 名: FrameCmdLocalAck
*  功能说明: 扩展板自身应答给安卓板
*  形    参: @cmdBuff 接收到的协议命令数据
			 @cmdLen 接收到的协议命令长度
			 @userDat 要上报的用户数据
			 @userLen 要上报的数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void FrameCmdLocalAck(uint8_t *cmdBuff,uint8_t cmdLen,uint8_t *userDat,uint8_t userLen)
{
    uint8_t temp[50]= {0};
    uint8_t frameLen  = 0;

    if(cmdLen > 50)return;
    memcpy(temp, cmdBuff, cmdLen);
    temp[Region_CmdNumber] |= FCMD_DIR_BIT;	//帧传输方向
    if(userLen>0)
    {
        temp[Region_DataLenNumber] = userLen+4;	//帧长度 (数据功能码(1)+数据标识(3)+用户数据(userLen))
    }
    else
    {
        temp[Region_DataLenNumber] = 0;
    }
    memcpy(&temp[Region_DataValNumber],userDat,userLen); //复制用户数据
    frameLen = Frame_Compose(temp);

    UseComSendBytes(temp, frameLen);
}

//
/**
*********************************************************************************************************
*  函 数 名: FrameCmdLocalAck
*  功能说明: 扩展板自身应答给安卓板
*  形    参: @cmdBuff 接收到的协议命令数据
			 @cmdLen 接收到的协议命令长度
			 @userDat 要上报的用户数据
			 @userLen 要上报的数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void FrameCmdLocalAck1(uint8_t *cmdBuff,uint8_t cmdLen,uint8_t *userDat,uint8_t userLen)
{
    uint8_t temp[50]= {0};
    uint8_t frameLen  = 0;

    if(cmdLen > 50)return;
    memcpy(temp, cmdBuff, cmdLen);
    temp[1] = 0xFF;
    temp[2] = 0xFF;
    temp[3] = 0xFF;
    temp[4] = 0xFF;
    temp[Region_CmdNumber] |= FCMD_DIR_BIT;	//帧传输方向
    if(userLen>0)
    {
        temp[Region_DataLenNumber] = userLen+4;	//帧长度 (数据功能码(1)+数据标识(3)+用户数据(userLen))
    }
    else
    {
        temp[Region_DataLenNumber] = 0;
    }
    memcpy(&temp[Region_DataValNumber],userDat,userLen); //复制用户数据
    frameLen = Frame_Compose(temp);

    UseComSendBytes(temp, frameLen);
}
/**
*********************************************************************************************************
*  函 数 名: vAesSave
*  功能说明: 处理初始化协议命令
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
static void vAES_Save(uint8_t *buff,uint8_t len)
{
    STMFLASH_Write(DEVICE_AES_SAVE_ADDR,(uint16_t*)&buff[Region_DataValNumber],sizeof(Device_aes_t)/2);     //写入
    STMFLASH_Read(DEVICE_AES_SAVE_ADDR,(uint16_t*)&deviceInfo.aes_field,sizeof(Device_aes_t)/2);             //读出
    if(memcmp(&deviceInfo.aes_field,&buff[Region_DataValNumber],sizeof(Device_aes_t))==0)                    //对比
    {
        DebugPrintf("\n密文保存成功");
    }
    else
    {
        DebugPrintf("\n密文保存失败");
    }
}


/**
*********************************************************************************************************
*  函 数 名: vUartFrameCmdInitDeal
*  功能说明: 处理初始化协议命令
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
static void  vUartFrameCmdInitDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2 = (buff[10]<<8)+buff[11];
    uint8_t report_flag = 1;		//表示是否要执行上报，1执行，0不执行
    switch(cmd2)
    {
    case 0xFFFF:
        DebugPrintf("\n设备配网");
        vDeviceMatchNet(buff,len);
        break;
    case 0xFFFE:
        DebugPrintf("\n设备列表");
        if(buff[Region_DataLenNumber]>=4)    //确保后面的设备列表的长度大于等于0
        {
            vDeviceListSave(&buff[Region_DataValNumber],buff[Region_DataLenNumber]-4);
        }
        RouteTableRef();
        break;
    case 0xFFFD:
        break;
    case 0xFFFC:
        break;
    case 0xFFFB:
        break;
    case 0xFF00:
		FrameCmdLocalAck(buff,len,0,0);         //应答
		report_flag = 0;
        DebugPrintf("\n密文下发");
        vAES_Save(buff,len);
        vAppWirelessInit();
        AES_Init();                 //是密文生效
        break;
    case 0x0001:        //主站要求读取软件版本号
        vVersionReport(buff,len,Version_Number);
        report_flag = 0;	//要求后面不要上报了
        break;
    case 0x0000:		//校时

        break;
    default:
        break;
    }
    if(report_flag == 1)
        FrameCmdLocalAck(buff,len,0,0);         //应答
}

/**
*********************************************************************************************************
*  函 数 名: vUartFrameCmdStateDeal
*  功能说明: 处理状态量协议命令
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
static void vUartFrameCmdStateDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2 = (buff[10]<<8)+buff[11];
    uint8_t report_flag = 1;		//表示是否要执行上报，1执行，0不执行
    switch(cmd2)
    {
    case 0x0000:
        DebugPrintf("\n读设备MAC地址");
        vDeviceMacReport(buff,len,deviceInfo.mac_frame.mac);
        report_flag = 0;	//要求后面不要上报了
        break;
    case 0x0001:
        DebugPrintf("\n确认设备是否在线");
        break;
    case 0x0002:

        break;
    case 0x0020:
        break;
    case 0x0021:
        break;
    case 0x0100:
        DebugPrintf("\n电池充电状态");
        break;
    case 0x0101:
        break;
    default:
        break;
    }
    if(report_flag == 1)
        FrameCmdLocalAck(buff,len,0,0);         //应答
}
/**
*********************************************************************************************************
*  函 数 名: vUartFrameCmdClassify
*  功能说明: 处理控制量协议命令
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
static void vUartFrameCmdCtrlDeal(uint8_t *buff,uint8_t len)
{
    uint16_t cmd2;
    uint8_t report_flag = 1;		//表示是否要执行上报，1执行，0不执行

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
        //DebugPrintf("\n触摸面板LED模式控制");
        xQueueSend(xQueueLedTask, &buff[Region_DataValNumber], (TickType_t)10);			//通过邮箱发送核心板控制LED的协议帧到LED任务
        break;
    default:
        break;
    }
    if(report_flag == 1)
        FrameCmdLocalAck(buff,len,0,0);         //应答
}

/**
*********************************************************************************************************
*  函 数 名: vUartFrameCmdDeviceFuncCtrlDeal
*  功能说明: 电器功能控制协议命令处理
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
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
*  函 数 名: vUartFrameCmdSensorDeal
*  功能说明: 传感器类协议命令处理
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
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
*  函 数 名: vUartFrameCmdDeviceFuncCtrlDeal
*  功能说明: 门禁类协议命令处理
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
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
*  函 数 名: vUartFrameCmdClassify
*  功能说明: 分类不同的串口命令协议帧
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
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
*  函 数 名: vUartAesCmdDeal
*  功能说明: 处理串口Aes协议帧（烧录自身MAC地址）
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
static void vUartAesCmdDeal(uint8_t *buff,uint8_t len)
{
    uint8_t i = 0;
    uint8_t cmd1 = buff[1];

    switch(cmd1)
    {
    case 0x20:
        DebugPrintf("\r\n烧录自身MAC地址 MAC = ");
        for(i=0; i<8; i++)
        {
            printf("%02X ",buff[i+3]);
        }
        STMFLASH_Write(DEVICE_MAC_SAVE_ADDR,(uint16_t*)buff,sizeof(Device_mac_t)/2);  //写入MAC地址
        STMFLASH_Read(DEVICE_MAC_SAVE_ADDR,(uint16_t*)&deviceInfo.mac_frame,sizeof(Device_mac_t)/2);    //读出MAC地址
        if(memcmp(&deviceInfo.mac_frame,buff,sizeof(Device_mac_t)/2)==0)              //对比MAC地址
        {
            DebugPrintf("\r\nMAC地址烧录成功");
            UseComSendBytes("\r\nMAC地址烧录成功",
                            sizeof("\r\nMAC地址烧录成功"));
        }
        else
        {
            DebugPrintf("\r\nMAC地址烧录失败");
            UseComSendBytes("\r\nMAC地址烧录失败",
                            sizeof("\r\nMAC地址烧录失败"));
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
*  函 数 名: uFrameCmd_CRC_Check
*  功能说明: 判断串口帧数据是否正确
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
eFrameCheckType xUartFrameCmdCheck(uint8_t *buff,uint8_t len)
{
    uint16_t crc_16;
    uint8_t crc16_h;
    uint8_t crc16_l;

    if(buff[0] != HKFreamHeader)		//帧头是否为0xAC
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
*  函 数 名: uFrameCmd_CRC_Check
*  功能说明: 判断串口帧数据是否正确
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
eFrameCheckType xUartAesCmdCheck(uint8_t *buff,uint8_t len)
{
    uint16_t crc_16;
    uint8_t crc16_h;
    uint8_t crc16_l;

    if(buff[0] != AESFreamHeader)		//帧头是否为0x68
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
*  函 数 名: vUartFrameProcess
*  功能说明: 串口烧录MAC协议处理
*  形    参: @pbuff 存放协议数据结构体
*  返 回 值: 无
*********************************************************************************************************
*/
void vUartAesProcess(sUartRx_t *pbuff)
{
	uint8_t com_rdata;
    if(pbuff->status == UartRx_Finished)
    {
        if(xUartAesCmdCheck(pbuff->frame_buff,pbuff->total_len) == FRAME_OK)
        {
            //UartSendBytes(USART1, pbuff->frame_buff,pbuff->total_len);
            vUartAesCmdDeal(pbuff->frame_buff,pbuff->total_len);
        }
        pbuff->status = UartRx_FrameHead;
    }
	else if(comGetChar(COM1,&com_rdata)==1)
	{
		 vUartRxFrame(com_rdata,&sUart1Rx,0);
	}

}

/**
*********************************************************************************************************
*  函 数 名: vUartFrameProcess
*  功能说明: 串口应用协议处理
*  形    参: @pbuff 存放协议数据结构体
*  返 回 值: 无
*********************************************************************************************************
*/

void vUartFrameProcess(sUartRx_t *pbuff)
{
    uint8_t cmdflag = 0;
	uint8_t com_rdata;

    if(pbuff->status == UartRx_Finished)
    {
        if(xUartFrameCmdCheck(pbuff->frame_buff,pbuff->total_len) == FRAME_OK)
        {
            cmdflag = pbuff->frame_buff[Region_CmdNumber] & (FCMD_REMOTE_BIT|FCMD_EVNT_BIT);
            switch(cmdflag)
            {
            case 0x00:     	//本地普通
                DebugPrintf("\n本地普通帧");
                vUartFrameCmdClassify(pbuff->frame_buff,pbuff->total_len);
                break;
            case 0x08:     	//本地事件(主要是核心板的应答)
                DebugPrintf("\n本地事件帧");
                break;
            case 0x10:		//远程普通(电器设备的控制)
                DebugPrintf("\n远程普通帧");
                vFrameUartRemoteCmdDeal(pbuff->frame_buff,pbuff->total_len);
                break;
            case 0x18:		//远程事件
                DebugPrintf("\n远程事件帧");
                break;
            }
        }
        pbuff->status = UartRx_FrameHead;
    }
	else if(comGetChar(COM1,&com_rdata)==1)
	{
		 vUartRxFrame(com_rdata,&sUart1Rx,0);
	}
}

/**
*********************************************************************************************************
*  函 数 名: uFrameCmd_CRC_Check
*  功能说明: 判断无线帧数据是否正确
*  形    参: @buff 要处理的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
eFrameCheckType xWirelessRouterCmdCheck(uint8_t *buff,uint8_t len)
{
    uint16_t crc_16;
    uint8_t crc16_h;
    uint8_t crc16_l;

    if((buff[0] != ROUTER_FRAME_HDADER)||(buff[1] != ROUTER_FRAME_HDADER))		//帧头是否为0x69 0x69
        return FRAME_ERR;

    if((buff[2] + buff[3]) != 0xFF)		//帧长度和长度取反
        return FRAME_ERR;

    if((buff[len-2] != ROUTER_FRAME_END)||(buff[len-1] != ROUTER_FRAME_END)) //帧尾是否为0x96 0x96
        return FRAME_ERR;


    crc_16 = CRC16_2(buff,len-4);
    crc16_h = (uint8_t)(crc_16 >> 8);
    crc16_l = (uint8_t)(crc_16 & 0x00ff);
    if((crc16_h == buff[len-4])&&(crc16_l == buff[len-3]))
        return FRAME_OK;
    else
        return FRAME_ERR;
}


//判断是否为自己的家庭组
uint8_t  uxIsMyGroup(uint8_t *group)
{

//	if((group[0]!=deviceInfo.aes_field.addr_GA[0])||
//	(group[1]!=deviceInfo.aes_field.addr_GA[1])||
//	(group[2]!=deviceInfo.aes_field.addr_GA[2]))
//	{
//		return 0;
//	}

    if((group[0]!=deviceInfo.aes_field.addr_GA[1])||(group[1]!=deviceInfo.aes_field.addr_GA[2]))
    {
        return 0;
    }
    return 1;
}


/**
*********************************************************************************************************
*  函 数 名: vMastAckToDevice
*  功能说明: 扩展板应答电器设备
*  形    参: @buff 接收到电器设备发来的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void vMastAckToDevice(uint8_t *buff,uint8_t len,uint8_t mode)
{

    QUEUE_WIRELESS_SEND_t queueMsg;

    memcpy(queueMsg.msg, buff,len);
    queueMsg.toCh = Wireless_Channel[0];

    queueMsg.msg[Region_CmdNumber] &= ~FCMD_DIR_BIT;    //传输方向清零，表示主站发出
    queueMsg.msg[Region_DataLenNumber] = 0;		//应答数据长度为0
    queueMsg.msg[Region_CmdNumber] &= ~0x07;		//74编码
    //queueMsg.msg[Region_SeqNumber] &= 0xf0;

    queueMsg.len = Frame_Compose(queueMsg.msg);
    //加密
    if((queueMsg.msg[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
    {
        DebugPrintf("\n需要加密");

        Encrypt_Convert(queueMsg.msg,queueMsg.len ,&queueMsg.len, 1);   //加密
    }
    else
    {
        DebugPrintf("\n不用加密");
    }
	if(mode == 0)
	{
		xQueueSend(xQueueWirelessTx,&queueMsg, (TickType_t)10);			//直接发到无线发射任务
	}
	else
	{
	    CarrierSendBytes(queueMsg.msg,queueMsg.len);
	}
}

/**
*********************************************************************************************************
*  函 数 名: vMastAckToDevice
*  功能说明: 扩展板应答电器设备(路由功能)
*  形    参: @buff 接收到电器设备发来的数据
			 @len  数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void vMastAckToDeviceAtRoute(uint8_t *buff,uint8_t len,uint8_t mode)
{

    QUEUE_WIRELESS_SEND_t queueMsg;

    memcpy(queueMsg.msg, buff,len);
    queueMsg.toCh = Wireless_Channel[0];

    queueMsg.msg[Region_CmdNumber] &= ~FCMD_DIR_BIT;    //传输方向清零，表示主站发出
    queueMsg.msg[Region_DataLenNumber] = 0;		//应答数据长度为0
    //queueMsg.msg[Region_SeqNumber] &= 0xf0;

    queueMsg.len = Frame_Compose(queueMsg.msg);
    //加密
    if((queueMsg.msg[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
    {
        DebugPrintf("\n需要加密");

        Encrypt_Convert(queueMsg.msg,queueMsg.len ,&queueMsg.len, 1);   //加密
    }
    else
    {
        DebugPrintf("\n不用加密");
    }

    //路由帧数据打包
    queueMsg.len = FrameRouterCompose(queueMsg.msg[Region_AddrNumber], //目的设备地址
                                      queueMsg.msg,                                        //配网命令数据
                                      queueMsg.len,                                          //配网命令长度
                                      queueMsg.msg,                                 //缓存配网命令的邮箱
                                      0,                                            //路由表
                                      0);
	if(mode == 0)
	{
		xQueueSend(xQueueWirelessTx, &queueMsg, (TickType_t)10);			//直接发到无线发射任务
	}
	else
	{
	    CarrierSendBytes(queueMsg.msg,queueMsg.len);
	}
	
}

///**
//*********************************************************************************************************
//*  函 数 名: vMastRouterFrameDela
//*  功能说明: 处理主站发出去的无线数据
//*  形    参: @buff 要处理的数据
//			 @len  数据长度
//*  返 回 值: 无
//*********************************************************************************************************
//*/
//void vMastRouterFrameDela(uint8_t *buff,uint8_t len)
//{
//	uint8_t frame_type = 0;  //帧类型(0组网，1通信)
//	FRAME_ROUTER_SLAVE_CMD_t *pbuff;
//
//	pbuff = (FRAME_ROUTER_SLAVE_CMD_t*)buff;
//	frame_type = pbuff->ctrl.type;
//	if(frame_type == 1)  //通信
//	{
//		DebugPrintf("\n路由通信帧");
//	}
//	else		//组网
//	{
//	   DebugPrintf("\n路由组网帧");
//	}
//}


///**
//*********************************************************************************************************
//*  函 数 名: vSlaveRouterFrameDela
//*  功能说明: 处理从站发出去的无线数据
//*  形    参: @buff 要处理的数据
//			 @len  数据长度
//*  返 回 值: 无
//*********************************************************************************************************
//*/
//void vSlaveRouterFrameDela(uint8_t *buff,uint8_t len)
//{
//	uint8_t frame_type = 0;  //帧类型(0组网，1通信)
//	FRAME_ROUTER_SLAVE_CMD_t *pbuff;
//
//	pbuff = (FRAME_ROUTER_SLAVE_CMD_t*)buff;
//	frame_type = pbuff->ctrl.type;
//	if(frame_type == 1)  //通信
//	{
//		DebugPrintf("\n路由通信帧");
//		vMastAckToDevice(buff,len);
//	}
//	else		//组网
//	{
//	   DebugPrintf("\n路由组网帧");
//	}
//}

/**
*********************************************************************************************************
*  函 数 名: DeviceWireless2Uart()
*  功能说明: 将收到设备的无线数据经过解码和解密后，转发到串口
*  形    参: @pframe 收到的电器无线数据
			@frame_len     收到的电器无线数据长度
*  返 回 值: 无
*********************************************************************************************************
*/


/**
*********************************************************************************************************
*  函 数 名: vUartFrameProcess
*  功能说明: 处理无线接收到的一帧数据
*  形    参: @wireless 存放协议数据结构体
*  返 回 值: 无
*********************************************************************************************************
*/
extern TaskHandle_t StartTask_Handler;
void vWirelessFrameDeal(uint8_t *packBuff,uint8_t packLen,uint8_t mode)
{
    //uint8_t *pframe = wireless->Wireless_RxData;
    uint8_t frame_len = packLen;
    uint8_t frame_flag = 0;
    uint8_t cmdUpFlag = 1;		//是否上报到串口
    uint8_t index = 0;       //应用数据（0xAC协议帧的）开始位置



    if(xWirelessRouterCmdCheck(packBuff,frame_len) == FRAME_OK)              //路由协议
    {

        frame_len = packBuff[2];
        //解码
        if(frame_len<140)
        {
            FrameRouteData_74Convert(packBuff,packLen,&frame_len,0);
        }

        index = 13+packBuff[12];//算出应用数据的开始位置  (0x69协议开始的13个字节+路由表长度)
        //解密
        if(packBuff[index] == 0xAC)             //加密是在AC协议帧才有，所有判断路由协议帧中是否有AC协议
        {
            if((packBuff[index + Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
            {
                DebugPrintf("\n需要解密");
                if(frame_len < (index +4))return;

                Encrypt_Convert(&packBuff[index],frame_len-index-4 , &frame_len, 0);   //解密

            }
        }
        else
        {
            DebugPrintf("\n不用解密");
        }

        //判断是否为自己的家庭组
        if(uxIsMyGroup(&packBuff[5])==0)return;

        //判断是否为有路由长度
        if(packBuff[12]>0)
        {
            DebugPrintf("\n存在路由表");
        }

        frame_flag =  (packBuff[4] & 0x81);                //协议主站从站组网或者通信帧位判断

        switch(frame_flag)
        {
        case 0x00:      //主站组网
            cmdUpFlag = 0;       //不用再通过串口发送
            frame_flag =  (packBuff[4] & 0x06);                //心跳，通信模式
            switch(frame_flag)
            {
            case 0x00:		//非心跳正常功耗帧
                break;
            case 0x02:      //非心跳低功耗帧
                break;
            case 0x04:      //心跳正常功耗帧
                //DebugPrintf("\n心跳正常功耗帧");
                cmdUpFlag = 0;       //不用再通过串口发送
                break;
            case 0x06:      //心跳低功耗帧
                break;
            }
            break;
        case 0x01:		//主站通信
            frame_flag =  (packBuff[4] & 0x06);                //心跳，通信模式
            switch(frame_flag)
            {
            case 0x00:		//非心跳正常功耗帧
                break;
            case 0x02:      //非心跳低功耗帧
                break;
            case 0x04:      //心跳正常功耗帧
                cmdUpFlag = 0;       //不用再通过串口发送
                //DebugPrintf("\n心跳正常功耗帧");
                break;
            case 0x06:      //心跳低功耗帧
                break;
            }
            break;
        case 0x80:		//从站组网
            frame_flag =  (packBuff[4] & 0x06);                //心跳，通信模式
            switch(frame_flag)
            {
            case 0x00:      //非心跳正常功耗帧
                //if(wait_frameNum == (pframe[Region_SeqNumber]&0x0f))
            {
                xTaskNotify(StartTask_Handler,0x00000002,eSetBits);   //设备配网成功应答
            }
            UseComSendBytes(packBuff+index, (packBuff+index)[Region_DataLenNumber]+11);
            cmdUpFlag = 0;       //不用再通过串口发送
            DebugPrintf("\n从站回应配网成功");
            break;
            case 0x02:      //非心跳低功耗帧
                break;
            case 0x04:      //心跳正常功耗帧
                cmdUpFlag = 0;       //不用再通过串口发送
                DebugPrintf("\n从站组网心跳正常功耗帧");
                break;
            case 0x06:      //心跳低功耗帧
                break;
            }
            break;
        case 0x81:		//从站通信
            frame_flag =  (packBuff[4] & 0x06);                //心跳，通信模式

            switch(frame_flag)
            {
            case 0x00:      //非心跳正常功耗帧
                if(packBuff[index+Region_CmdNumber]&0x08)		//判断是否为事件帧
                {
                    DebugPrintf("\n从站事件帧");
                    if(vDeviceIsExistList(&deviceInfo.match,packBuff[index+Region_AddrNumber]) == 1)
                    {
                        //UseComSendBytes(&pframe[index],(pframe+index)[Region_DataLenNumber]+11);
                        vMastAckToDeviceAtRoute(&packBuff[index],(packBuff+index)[Region_DataLenNumber]+11,mode) ;
                    }
                    else
                    {
                        cmdUpFlag = 0;		//不上报事件
                    }
                }
                else
                {
                    //if(wait_frameNum == (pframe[Region_SeqNumber]&0x0f))
                    {
                        xTaskNotify(StartTask_Handler,0x00000001,eSetBits);   //设备控制成功应答
                    }
                }


                break;
            case 0x02:      //非心跳低功耗帧
                break;
            case 0x04:      //心跳正常功耗帧
                cmdUpFlag = 0;       //不用再通过串口发送
                DebugPrintf("\n从站通信跳正常功耗帧");
                if(packBuff[12]>0)		// 路由表长度大于0
                {
                    SaveDeviceRouteTable(packBuff[4],&packBuff[13],packBuff[12]);        //保存路由表
                }
                break;
            case 0x06:      //心跳低功耗帧
                break;
            }
            break;
        }
        if(cmdUpFlag == 1)    //需要发串口数据
        {
            UseComSendBytes(packBuff+index, (packBuff+index)[Region_DataLenNumber]+11);
        }

    }
    else if(packBuff[0]==0xAC)     //非路由协议，旧协议
    {
        if(uxIsMyGroup(&packBuff[3])==0)return;
       // frame_len = packBuff[Region_DataLenNumber];
        if((mode == 0)&&(frame_len<=70))
        {
            FrameData_74Convert(packBuff,packLen,&frame_len,0);
        }
        //解密
        if((packBuff[Region_SeqNumber]&FSEQ_ENC_BIT) > 0)
        {
            DebugPrintf("\n需要解密");

            Encrypt_Convert(packBuff,frame_len , &frame_len, 0);   //解密
        }

        if(xUartFrameCmdCheck(packBuff,frame_len) == FRAME_ERR)return;    //帧校验错误

        frame_flag = packBuff[Region_CmdNumber]&0x98;

        switch(frame_flag)
        {
        case 0x10:  		//主站，普通帧
            DebugPrintf("\n旧协议，主站，普通帧");
            break;
        case 0x18:          //主站，事件帧
            DebugPrintf("\n旧协议，主站，事件帧");
            break;
        case 0x90:          //从站，普通帧
            DebugPrintf("\n旧协议，从站，普通帧");
//            if(wait_frameNum == (packBuff[Region_SeqNumber]&0x0f))
            {
                if((packBuff[Region_DataIDNumber]==0xFF)&&		//是配网帧回应
                        (packBuff[Region_DataIDNumber+1]==0xFF)&&
                        (packBuff[Region_DataIDNumber+2]==0xFF))
                {
                    DebugPrintf("\n设备配网成功应答");
                    xTaskNotify(StartTask_Handler,0x00000002,eSetBits);   //设备配网成功应答
                }
                else
                {

                    xTaskNotify(StartTask_Handler,0x00000001,eSetBits);   //设备通信应答成功
                }
            }

            break;
        case 0x98:          //从站，事件帧
            DebugPrintf("\n旧协议，从站，事件帧");
            if(vDeviceIsExistList(&deviceInfo.match,packBuff[Region_AddrNumber]) == 1)
            {
                vMastAckToDevice(packBuff,frame_len,mode);
            }
            else
            {
                cmdUpFlag = 0;		//不上报事件
            }
            break;
        }
        if(cmdUpFlag == 1)    //需要发串口数据
        {
            UseComSendBytes(packBuff, frame_len);
        }

    }

}










