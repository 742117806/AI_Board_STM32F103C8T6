#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 
#define KEY_LONG_TIME	200   //����ʱ������

//������ض���
#define KEY1_READ()	PCin(15)
#define KEY2_READ()	PCin(13)
#define KEY3_READ()	PBin(6)
#define KEY4_READ()	PCin(14)

//������ֵö�ٽṹ
typedef enum 
{
	KEY_NONE,		//û�а���
	KEY1_PRES_S,		//�̰�
	KEY1_PRES_L,		//����
	KEY2_PRES_S,		//�̰�
	KEY2_PRES_L,		//����
	KEY3_PRES_S,		//�̰�
	KEY3_PRES_L,		//����
	KEY4_PRES_S,		//�̰�
	KEY4_PRES_L,		//����
	KEY_ALL_REL,		//���а������ͷ���
}eKEY_VALUE;

void vKeyInit(void);//��ʼ��
eKEY_VALUE eKeyScan(uint8_t mode);

		 				    
#endif
