#ifndef __FRAMETYPE_H
#define __FRAMETYPE_H
#include "stm32f1xx.h"
#include "deviceInfo.h"


//��ͨ��֡��صĶ���
#define HKData_LenMax                               228          //���ݱ�ʶ4+����8MAX, �������߼���ַ֡����
#define HKFrame_LenMax                              (11+HKData_LenMax+16)     //ͷ1+��ַ4+�������1+������1+���ݳ���1 +У��2+β1+����Ԥ���ռ�

#define DEVICE_INDEX_OFFSET		31			//�豸��ַƫ�������豸ȡ��ַ��Χ31-254��

//������¼MAC����
typedef struct WRITE_MAC_CMD_
{
	uint8_t FameHead;		// ֡ͷ
	uint8_t CmdFunc;		//��������
	uint8_t DataLen;			//���ݳ���
	uint8_t mac[8];			//�豸MAC
	uint8_t crc[2];
}WRITE_MAC_CMD_t;


//֡������
typedef struct FSQ_
{
	uint8_t frameNum:4;			//֡���
	uint8_t ctrlField:1;		//������0һ���ֽڣ�1�����ֽ�
	uint8_t encryptType:2;		//�������ͣ�0���ܣ�1��һ�ּ��ܣ�2�ڶ��ּ��ܣ�3�����ּ���
	uint8_t routeFlag:1;		//·�ɱ�־
}FSQ_t;

//֡������
typedef struct CTRL_
{
	uint8_t c_AFN:3;		//���ƹ����룬0��վӦ��ʹ�ã�1������֡����
	uint8_t eventFlag:1;	//�¼���־��0��ͨ֡��1�¼�֡
	uint8_t	relayFlag:1;	//�м̱�־��0����֡��1ת��֡
	uint8_t followUpFlag:1;	//����֡��־��0�޺���֡��1�к���֡
	uint8_t recAckFlag:1;	//����վ����Ӧ���־��0��ȷӦ��1�쳣Ӧ��
	uint8_t dir:1;			//���ͷ���0��վ������1��վ����
}CTRL_t;

//Э��֡������
typedef struct FRAME_DATA_
{
	uint8_t AFN;		//���ݹ�����
	uint8_t Index[3];	//���ݱ�ʶ
	uint8_t content[235];	//��·����		
}FRAME_DATA_t;
//֡����
typedef struct FRAME_CMD_
{
	uint8_t FameHead;		//֡ͷ
	uint8_t addr_DA;		//�߼���ַ
	uint8_t addr_GA[3];		//Ⱥ�ڵ�ַ
	FSQ_t 	FSQ;			//֡������
	CTRL_t Ctrl;			//֡������
	uint8_t DataLen;		//���ݳ���
	FRAME_DATA_t userData;	//�û�������
//֡����			
	
}FRAME_CMD_t;
//�豸������·������
typedef struct JOINE_NET_CMD_
{
	uint8_t mac[8];
	uint8_t aes[16];		//����rsa�������ó�һ��aes���ܻ���ܵ���Կ
	
}JOINE_NET_CMD_t;

//��׿���·�ͨ����Կ��Ⱥ���ַ�����ݽṹ
typedef struct WRITE_AES_CMD_
{
	uint8_t aes[16];		//����rsa�������ó�һ��aes���ܻ���ܵ���Կ
	uint8_t addr_GA[3];		//Ⱥ�ڵ�ַ

}WRITE_AES_CMD_t;



typedef enum LedCmdType_
{
	AROUND_OFF,         //��Χ��Ϩ��
	AROUND_BREATH,      //��Χ�ƺ���ģʽ
	AROUND_ON,	        //��Χ�Ƶ���
	AROUND_FLOW,        //��Χ����ˮģʽ
	CENTRE_MODE,		//�м��ģʽ
	AROUND_INDEX,		//��Χ�ư�������	
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

//·�ɲ�Э��֡������ṹ
typedef struct FRAME_ROUTER_CTRL_
{
	uint8_t type:1;			//֡���ͣ�1ͨ�ţ�0����
	uint8_t mode:1;			//֡ģʽ��0������1LDC����ʡ��
	uint8_t heat:1;			//����֡��־
	uint8_t nn:4;			//Ԥ��
	uint8_t dir:1;			//���䷽��0����1��	
}FRAME_ROUTER_CTRL_t;

//·�ɼ�����ÿһ����Ӧ��ͨ�ŷ�ʽ
typedef struct FRAME_ROUTER_NUMBER_
{
    uint8_t type:4;  //ͨ�ŷ�ʽ����ÿһλ��ʶ����·�ɣ�0��ʾ�������ز���1��ʾ����
	uint8_t index:4;		//��ǰ·�ɵ��Ｖ��ָʾ
}FRAME_ROUTER_NUMBER_t; 
//·�ɲ�Э��֡�ṹ
typedef struct FRAME_ROUTER_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//���ȣ����������ֽڵ�������ǰ���ֽ�����
	uint8_t len_c;     //���Ȱ�λȡ��
	FRAME_ROUTER_CTRL_t ctrl;		//֡����
    uint8_t netNum[2];			//����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
	uint8_t des_addr;			//Ŀ���ַ
	uint8_t src_addr;			//Դ��ַ
	FRAME_ROUTER_NUMBER_t routerNum;	//·�ɼ���
	uint8_t router_len;			//·�ɱ���
		
}FRAME_ROUTER_CMD_t;

//·�ɲ�Э����վ����֡�ṹ
typedef struct FRAME_ROUTER_MASTER_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//���ȣ����������ֽڵ�������ǰ���ֽ�����
	uint8_t len_c;     //���Ȱ�λȡ��
	FRAME_ROUTER_CTRL_t ctrl;		//֡����
    uint8_t netNum[2];			//����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
	uint8_t des_addr;			//Ŀ���ַ
	uint8_t src_addr[3];			//Դ��ַ
	FRAME_ROUTER_NUMBER_t routerNum;	//·�ɼ���
	uint8_t router_len;			//·�ɱ���
		
}FRAME_ROUTER_MASTER_CMD_t;

//·�ɲ�Э����վ����֡�ṹ (�������ʱ��Ŀ���ַ��8���ֽڵ�MAC��ַ)
typedef struct FRAME_ROUTER_MASTER_EXT_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//���ȣ����������ֽڵ�������ǰ���ֽ�����
	uint8_t len_c;     //���Ȱ�λȡ��
	FRAME_ROUTER_CTRL_t ctrl;		//֡����
    uint8_t netNum[2];			//����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
	uint8_t des_addr[8];			//Ŀ���ַ
	uint8_t src_addr[3];			//Դ��ַ
	FRAME_ROUTER_NUMBER_t routerNum;	//·�ɼ���
	uint8_t router_len;			//·�ɱ���
		
}FRAME_ROUTER_MASTER_EXT_CMD_t;

//·�ɲ�Э���վ����֡�ṹ
typedef struct FRAME_ROUTER_SLAVE_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//���ȣ����������ֽڵ�������ǰ���ֽ�����
	uint8_t len_c;     //���Ȱ�λȡ��
	FRAME_ROUTER_CTRL_t ctrl;		//֡����
    uint8_t netNum[2];			//����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
	uint8_t des_addr[3];			//Ŀ���ַ
	uint8_t src_addr;			//Դ��ַ
	FRAME_ROUTER_NUMBER_t routerNum;	//·�ɼ���
	uint8_t router_len;			//·�ɱ���
		
}FRAME_ROUTER_SLAVE_CMD_t;



//·�ɲ�Э��֡�ṹ (�������ʱ��Ŀ���ַ��8���ֽڵ�MAC��ַ)
typedef struct FRAME_ROUTER_EXT_CMD_
{
	uint8_t head_h;     //0x69
	uint8_t head_l;     //0x69
	uint8_t len;		//���ȣ����������ֽڵ�������ǰ���ֽ�����
	uint8_t len_c;     //���Ȱ�λȡ��
	FRAME_ROUTER_CTRL_t ctrl;		//֡����
    uint8_t netNum[2];			//����ţ�ȡ�߼���ַ�ĵ�λ2���ֽ�
	uint8_t des_addr[8];			//Ŀ���ַ
	uint8_t src_addr;			//Դ��ַ
	FRAME_ROUTER_NUMBER_t routerNum;	//·�ɼ���
	uint8_t router_len;			//·�ɱ���
	

}FRAME_ROUTER_EXT_CMD_t;

//����ת���ߵĶ��нṹ��
typedef struct QUEUE_UART_TO_WIRELESS_
{
	uint8_t msg[256];		//��������
	uint8_t len;			//���ݳ���
	uint8_t toCh;			//Ҫ���͵���������ͨ����
	
}QUEUE_WIRELESS_SEND_t;

//���������ط���Ҫ�ȵ������ݱ�ʶ3���ֽں�֡���
typedef struct RETRY_WAITE_FOR_
{
	uint8_t index[3];    //���ݱ�ʶ
	uint8_t frameNum;	//֡���
	uint8_t retryCnt;		//�ط�����
	uint8_t flag;		//�ȴ���Ӧ��־��1Ҫ�ȴ��ظ����Ѿ��ȵ��ظ�
} RETRY_WAITE_FOR_t;


/* ·�ɱ�ṹ��ض��� */
//ÿ���ڵ�ĵ�ַ�Ͷ�Ӧ���ź�ǿ��ֵ
typedef struct NODE_
{
	uint8_t addr;
	uint8_t rssi;
}NODE_t;


//�ڵ�����������·��
typedef struct PATH_
{
	uint8_t len;		//·�ɱ���
	uint8_t addr[3];   	//1���м� , 2���м�, 3���м�
}PATH_t;


//Ŀ���豸
typedef struct DES_DEVICE_
{
	//uint8_t des_addr;		//Ŀ�ĵ��豸��ַ
	uint8_t des_rssi;		//Ŀ�ĵ��豸�ź�ǿ��
	PATH_t path1;
	PATH_t path2;
	PATH_t path3;
}DES_DEVICE_t;

//��ǰӦ���·�ɱ�ͱ��Ƚṹ
typedef struct ROUTER_TAB_ACK_
{
	uint8_t len;
	uint8_t table[3];
	
}ROUTER_TAB_ACK_t;

//�����豸�ڽ��ڵ���ź�ǿ�Ƚṹ
typedef struct ROUTER_BROTHER_RSSI_
{
	uint8_t addr1;
	uint8_t rssi1;		//3���ڽ��ڵ� 
	uint8_t addr2;
	uint8_t rssi2;		//3���ڽ��ڵ� 
	uint8_t addr3;
	uint8_t rssi3;		//3���ڽ��ڵ� 
}ROUTER_BROTHER_RSSI_t;


#endif


