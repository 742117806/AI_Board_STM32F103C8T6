#include "touch.h"
#include "delay.h"

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY1����
//2��KEY2����
//3��KEY3���� 
//4��KEY4����
//ע��˺�������Ӧ���ȼ�,KEY1>KEY2>KEY3>KEY4!!

uint8_t TouchKeyScan(uint8_t mode)
{	 
	static uint8_t key_up=1;//�������ɿ���־
	static uint16_t isPresCnt = 0;

	if(mode)key_up=1;  //֧������		
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
		else if(KEY4==0)return KEY4_PRES;
	}
	else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)
	{
		if(key_up == 0)   //˵��֮ǰ�Ѿ����£����ڸո��ͷŰ���
		{
			key_up=1; 
			return KEY_UP;
		}
		key_up=1; 
		
	}

	if(KEY1==1)
	{
	   isPresCnt = 0;
	   
	}
	else
	{
		isPresCnt ++;
		if(isPresCnt > 800)
		{
			isPresCnt = 0;
			return KEY1_LONG_PRES;
		}
	}
	
 	return 0;// �ް�������
}


