#ifndef __FRAMETYPE_H
#define __FRAMETYPE_H
#include "stm32f1xx.h"


//��ͨ��֡��صĶ���
#define HKData_LenMax                               228          //���ݱ�ʶ4+����8MAX, �������߼���ַ֡����
#define HKFrame_LenMax                              (11+HKData_LenMax+16)     //ͷ1+��ַ4+�������1+������1+���ݳ���1 +У��2+β1+����Ԥ���ռ�


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



/*
�豸��Ϣ
*/
typedef struct DeviceInfo_
{
	uint8_t mac[8];			//�豸MAC��ַ
	uint8_t aes[16];		//��Կ
	uint8_t addr_DA;		//�߼���ַ
	uint8_t addr_GA[3];		//Ⱥ�ڵ�ַ
}DeviceInfo_t;

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

#endif


