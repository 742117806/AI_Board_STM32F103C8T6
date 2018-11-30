

#include "includes.h"


/*
*********************************************************************************************************
*  �� �� ��: vKeyInit
*  ����˵��: ������ʼ��KEY1,KEY2,KEY3,KEY4(PC15,PC13,PB6,PC14)
*  ��    ��: ��
*  �� �� ֵ: ��
*********************************************************************************************************
*/
void vKeyInit(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PC�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_14;	//KEY1,KEY2,KEY4,�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	 //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //KEY3�˿�����
    GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //�����趨������ʼ��GPIOB_6

}

/*
*********************************************************************************************************
*  �� �� ��: u8KeyScan
*  ����˵��: ����ɨ��
*  ��    ��: @mode (0,��֧��������;1,֧��������)
*  �� �� ֵ: ������ֵ
*********************************************************************************************************
*/
eKEY_VALUE eKeyScan(uint8_t mode)
{	
	static u8 key_up=1;//�������ɿ���־
	static u16 key_long_cnt = 0;    //������������
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1_READ()==0||KEY2_READ()==0||KEY3_READ()==0||KEY4_READ()==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1_READ()==0)return KEY1_PRES_S;
		else if(KEY2_READ()==0)return KEY2_PRES_S;
		else if(KEY3_READ()==0)return KEY3_PRES_S;
		else if(KEY4_READ()==0)return KEY4_PRES_S;
	}
	else if(KEY1_READ()==1&&KEY2_READ()==1&&KEY3_READ()==1&&KEY4_READ()==1) //���а������ͷ���
	{
		key_up=1; 		
		return KEY_ALL_REL;
	}
	else
	{
		
		key_long_cnt ++;
		if(key_long_cnt > KEY_LONG_TIME)
		{
			key_long_cnt = 0;
			if(KEY1_READ()==0)return KEY1_PRES_L;
			else if(KEY2_READ()==0)return KEY2_PRES_L;
			else if(KEY3_READ()==0)return KEY3_PRES_L;
			else if(KEY4_READ()==0)return KEY4_PRES_L;
		}
	}
 	return KEY_NONE;// �ް�������

}

