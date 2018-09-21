#ifndef ROUT_H_
#define ROUT_H_


#include "stm32f1xx.h"
#include <string.h>
#include "crc16.h"
#include "wireless_app.h"

/*
1. ÿ���ϵ�ʱ�����߼���ַ�Ƿ���ڶ���EEPROM����Ӧ�߼���ַ��·�ɱ�������·�ɼ�����ʼ��Commun_SucessVal��
   ��0��Ϊ255��1��Ϊ223��2��Ϊ191��3��Ϊ159��4��Ϊ127��5��Ϊ95���ٱ��浽EEPROM�У�����Ӧ�߼���ַ��·�ɱ�
    ���һ��0��·�ɱ���Ӧ�߼���ַ��
2. ���յ��ϲ���߼���ַ�б�ʱ�����EEPROM�е���Ӧ�߼���ַ�Ƿ������ӻ���٣��������ӣ���ʼ����Ӧ���߼���ַ
   ·�ɱ����м��٣�ɾ����Ӧ���߼���ַ·�ɱ����������߼���ַ��·�ɱ���м�飬����ʹ�õ��˵�ַ��·�ɱ�
   ��·�ɱ�ɾ����
3. ÿ���·�֡ʱ�����ɹ������ε���·�ɱ��·����ɹ���һ��������С�����ȣ����糬ʱ����ʱʱ������·�ɱ���
  ����Ϊ0��ȷ����δ�յ���ȷ�ķ������ݣ���ʾ·��ʧ�ܣ���ʱ���Դ�·�������Դ�������Ϊ0������ɲ�����ѡ��
   ������ͬ��������ͬ������·�ɱ�����껹δ�ɹ��������Ե���·�ɱ����Դ�������Ϊ0������ɲ�����ѡ����
   ������·�ɱ��δ�ɹ�����ʾ����·�ɱ�ʧ�ܡ�
4. ÿ�ε���·���·�֡ʱ����Ҫά��·�ɱ�ṹ���ݣ�·��ͨѶ�ɹ�ֵ������ʧ�����������ɹ����ȣ�������Ӧ����
5. ����·�ɱ��·�ʧ��ʱ��������ѭ������·�ɱ��·����ݡ�ʹ����ѭ��ѡ��·�ɵ�ַʱ��������С����ͬ��������
   ��ַ�⣬���մ�С������Ӵ�С˳��ѡ��·�ɵ�ַ�����·�δ�ɹ�ʱ��һֱ��ѭ����·�ɱ�ֱ���յ����е�����
   ֡��ʱ���궨��һ����ʱ��������
6. �紴����·�ɱ��·��ɹ�����ֹͣ��ѭ����ʱ����·�ɼ�����ʼ��Commun_SucessVal��0��Ϊ255��1��Ϊ223��2��Ϊ191��
   3��Ϊ159��4��Ϊ127��5��Ϊ95��������·�ɱ���Ϣ���浽EEPROM�С�
7. ���ܴ�����·�ɱ��Ƿ��·��ɹ�����ÿ����ѭ����ʱ��Ӧ����¼��ǰ��ֹͣ��ѭ�ĵ�·�ɵ�ַ��Ϣ���Ա���һ����ѭʱ
   Ҳ�ɴӶϵ㴦����ִ����ѭ��

8. ��ʹ��ĳ��·���·�֡����ʧ�ܼ����ﵽһ��ֵA��ʱ��������ͬ������Ҳ��ͬ������Commun_SucessValֵ��Ϊ64�������64����
   ����һ����B������·�ɱ�ɾ����0�����⣩����ʹ��ĳ��·���·�֡�����ɹ������ﵽһ����C��ʱ��������ͬ������Ҳ��ͬ����
   ��Commun_SucessValֵ��Ϊ192����С��192��������һ����D��ʱ������ֵ��Ϊ255��
9. ·��ͨѶ�ɹ�ֵ��ά����ÿ���յ���ȷ������֡ʱ����ʾͨѶ�ɹ�������Ӧ·�ɱ��еĳɹ�ֵ������·�ɱ����ڣ��򴴽�
   ·�ɱ���ʼ��Commun_SucessVal�����·�ɱ������������ɾ���ɹ�ֵ���ٵ��Ǹ�·�ɱ����·�֡��ʱδ�յ���Ӧ֡����ʾ
   ͨѶʧ�ܣ���ʱ����Ӧ·�ɱ��еĳɹ�ֵ���١�ÿ�ε��������ͼ���������ͬ�ļ�������ͬ��

10. ֡������ĵ�7λΪ1����ʾ��·�ɱ�Ϊ0��ʾ��·�ɱ�Ϊ������ǰ��������ݣ�·�ɱ����У����֮ǰ��
11. ·�ɱ�ṹ���ĸ�����ɣ���·�ɱ�����1Byte��+·��ʣ�༶����1Byte��+ ֡�����ڵ��ַ��1Byte��+·�ɱ�n Byte����
   ·�ɳ����򣺱�ʾ·�ɱ���ܼ�����
   ·��ʣ�༶���򣺱�ʾת����ʣ��·�ɼ���������3��·�ɱ��ܼ�������3����һ����֡ʱ��
               ʣ�༶������3��ÿת��һ�Σ���ʣ�༶����1������������֡���߼���ַ��
                   ��ַ��ϢΪ����Ľڵ��յ�֡���ж�ʣ�༶���Ƿ�Ϊ0����ʣ�༶����Ϊ0ʱ����֡��ת����֡����ʣ�༶
               ��Ϊ0ʱ����������֡�������֡���ݺ�·����Ϣ���Ա��ϱ�������������֡�����账��
                   ��չ��ÿ���յ�����֡ʱ��Ӧ���ж�֡�Ƿ�Ϊ·��֡���������·��֡������İ�ת���������·��֡����
               ��ʣ�༶���Ƿ�Ϊ0��ʣ�༶������0���账��ʣ�༶��Ϊ0ʱ����������֡���账�����������֡����֡����
               ·����Ϣ����Ȼ������İ�ת����
   ֡�����ڵ��ַ�򣺱�ʾ��֡�����Ľڵ��ַ����Ϊ����֡ʱ����ʾĿ���ַ����Ϊ����֡ʱ����ʾԴ��ַ��
   ·�ɱ���  ��Ҫת����·�ɱ��ַ��
   ������֡ʱ��������ʣ�༶�����������߼���ַ��Ȼ��ת����
               ����֡��ʣ�༶������0�ڵ㴦���֡�������յ���ʣ�༶���Լ�1���ٽ�ʣ�༶����ȥ1�����õ�ֵ���Ǹ���ַ����Ϊ�߼���ַ��
               ����֡��ʣ�༶������0��չ�崦���֡�������յ���ʣ�༶���Լ�1�����ܼ�����ʣ�༶�����Ǹ���ַ����Ϊ�߼���ַ

12. ʾ����3��·�ɱ�֡�·���ox0e��·�ɾ�һ��0x0b������0x0c������0x0d����
    ·�ɱ�ṹΪ��
    ·�ɱ�������ǰʣ��·�ɼ�����֡�����ڵ��ַ������·�ɵ�ַ������·�ɵ�ַ��һ��·�ɵ�ַ
����֡��
��չ���յ����İ������֡�󣬵��û򴴽�·�ɱ�
   ������չ����֡·�ɱ��·�:
��    ��x03, 0x03, 0x0e, 0x0d, 0x0c, 0x0b   ����һ��·�ɵ�ַ0x0b���뵽�߼���ַ
    2��һ��·�ɵ�ַ0x0b��֡·�ɱ��·���
    ����x03, 0x02, 0x0e, 0x0d, 0x0c, 0x0b   ���ڶ���·�ɵ�ַ0x0c���뵽�߼���ַ
    3������·�ɵ�ַ0x0c��֡·�ɱ��·���
    ����x03, 0x01, 0x0e, 0x0d, 0x0c, 0x0b   ��������·�ɵ�ַ0x0d���뵽�߼���ַ
    4������·�ɵ�ַ0x0d��֡·�ɱ��·���
    ����x03, 0x00, 0x0e, 0x0d, 0x0c, 0x0b   ��Ŀ���ַ0x0e���뵽�߼���ַ
����֡��
    1��Ŀ���ַ0x0e��֡·�ɱ��Ϸ���
    ����x03, 0x03, 0x0e, 0x0d, 0x0c, 0x0b   ��Ŀ���ַ0x0d���뵽�߼���ַ
    2������·�ɵ�ַ0x0d��֡·�ɱ��Ϸ���
    ����x03, 0x02, 0x0e, 0x0d, 0x0c, 0x0b   ���ڶ���·�ɵ�ַ0x0c���뵽�߼���ַ
    3������·�ɵ�ַ0x0c��֡·�ɱ��Ϸ���
    ����x03, 0x01, 0x0e, 0x0d, 0x0c, 0x0b   ����һ��·�ɵ�ַ0x0b���뵽�߼���ַ
    4��һ��·�ɵ�ַ0x0b��֡·�ɱ��Ϸ���
    ����x03, 0x00, 0x0e, 0x0d, 0x0c, 0x0b   ��Դ��ַ0x0e���뵽�߼���ַ
��չ���յ�����֡�󣬶�·����Ϣ����ά����ȥ��·�ɱ���֡�Ϸ������İ塣
*/

/*
#define MainRout_Path_Size      3
#define Node_Size               255
//////////ĳ·�������ɹ�������ʧ�ܵĴ������//////////////
//�����ɹ���A����������ִ���·�ɼ�������ͬ�ļ���������ͬ��
#define MainRout_ContinuPass_A0             3
#define MainRout_ContinuPass_A1             6
#define MainRout_ContinuPass_A2             9
#define MainRout_ContinuPass_A3             12
//#define MainRout_ContinuPass_A4             15
//#define MainRout_ContinuPass_A5             18

#define MainRout_SucessValThreshold_A       223

//�����ɹ���B
#define MRP_ContinuPass_B0             5
#define MRP_ContinuPass_B1             10
#define MRP_ContinuPass_B2             15
#define MRP_ContinuPass_B3             20
//#define MRP_ContinuPass_B4             25
//#define MRP_ContinuPass_B5             30

#define MRP_SucessValThreshold_B       255

//����ʧ����C
#define MRP_ContinuFail_C0             18
#define MRP_ContinuFail_C1             15
#define MRP_ContinuFail_C2             12
#define MRP_ContinuFail_C3             9
//#define MRP_ContinuFail_C4             6
//#define MRP_ContinuFail_C5             3

#define MRP_SucessValThreshold_C       64

//����ʧ����D
#define MRP_ContinuFail_D0             30    //0��·����ʱ����Ϊ����ɾ��
#define MRP_ContinuFail_D1             25
#define MRP_ContinuFail_D2             20
#define MRP_ContinuFail_D3             15
//#define MRP_ContinuFail_D4             10
//#define MRP_ContinuFail_D5             5

#define MRP_SucessValThreshold_D       0


/////////����·�ɳɹ�ֵ�ĵ��������ͼ�������///////////////
#define MRP_SucessVal_IncS0            2
#define MRP_SucessVal_IncS1            2
#define MRP_SucessVal_IncS2            2
#define MRP_SucessVal_IncS3            2
#define MRP_SucessVal_IncS4            2
#define MRP_SucessVal_IncS5            2

#define MRP_SucessVal_SubS0            4        //�ɹ��ʵ���66.7%�ᱣ�ֳɹ�ֵ����(4/(2+4)*100%)
#define MRP_SucessVal_SubS1            5        //�ɹ��ʵ���71.4%�ᱣ�ֳɹ�ֵ����(5/(2+5)*100%)
#define MRP_SucessVal_SubS2            6        //�ɹ��ʵ���75%�ᱣ�ֳɹ�ֵ����(6/(2+6)*100%)
#define MRP_SucessVal_SubS3            7        //�ɹ��ʵ���77.8%�ᱣ�ֳɹ�ֵ����(7/(2+7)*100%)
//#define MRP_SucessVal_SubS4            8        //�ɹ��ʵ���80%�ᱣ�ֳɹ�ֵ����(8/(2+8)*100%)
//#define MRP_SucessVal_SubS5            9        //�ɹ��ʵ���81.8%�ᱣ�ֳɹ�ֵ����(9/(2+9)*100%)


////////////����·�ɳɹ�ֵ�������Сֵ����////////////////
#define MRP_SucessVal_MinS0            192
#define MRP_SucessVal_MinS1            0
#define MRP_SucessVal_MinS2            0
#define MRP_SucessVal_MinS3            0
#define MRP_SucessVal_MinS4            0
#define MRP_SucessVal_MinS5            0

#define MRP_SucessVal_MaxS0            255
#define MRP_SucessVal_MaxS1            255
#define MRP_SucessVal_MaxS2            255
#define MRP_SucessVal_MaxS3            255
//#define MRP_SucessVal_MaxS4            255
//#define MRP_SucessVal_MaxS5            255


////////////��������·��ʱ�ɹ�ֵ�ĳ�ʼֵ����/////////////////
#define MRP_SucessVal_InitialS0            255
#define MRP_SucessVal_InitialS1            250
#define MRP_SucessVal_InitialS2            245
#define MRP_SucessVal_InitialS3            240
//#define MRP_SucessVal_InitialS4            235
//#define MRP_SucessVal_InitialS5            230

///////////////////·�ɵ��ô���////////////////////////
#define MRP_CallTimes_S0                  1    //����Ϊ1����ͬ
#define MRP_CallTimes_S1                  1
#define MRP_CallTimes_S2                  1
#define MRP_CallTimes_S3                  1
#define MRP_CallTimes_S4                  1
#define MRP_CallTimes_S5                  1


#define MRP_CallTimes_Tab                 2            //������·�ɱ�ĵ��ô���

//֡·�������󳤶ȣ� ·�ɳ��ȣ�1Byte��+·�ɼ�����1Byte��+ ֡�����ڵ��ַ��1Byte��+·�ɱ�n Byte��
#define MRP_RoutRegion_LenMax             (3+Rout_series_Size) 




//�����Ľڵ�·��
typedef struct
{
  uint8_t Path_Series_Val;                        //·������
  uint8_t Path_Addr[Rout_series_Size];            //·����ַ,Խ����Ŀ���ַ��Խ��ǰ
  uint8_t Path_ContinulFail_Cnt;                  //��·��ͨѶ��������ʧ������������ͬ������Ҳ��ͬ                                          
  uint8_t Path_ContinulPass_Cnt;                  //��·��ͨѶ���������ɹ�����������ͬ������Ҳ��ͬ                                           
  uint8_t Path_SucessVal;                         //��·��ͨѶ�ɹ�ֵ����ʼֵΪ128��
                                            
  //uint16_t Path_Total_Cnt;                      //��·��ͨѶ��������            
  //uint16_t Path_Pass_Cnt;                       //��·��ͨѶ����ɹ���
  //uint8_t Path_Pass_Rate;                       //��·��ͨѶ����ɹ���
}HKMainRout_Path_TypDef;


//�����Ľڵ�·���ܱ�
typedef struct
{
  //uint8_t Current_Scan_Series;                  //��ǰɨ�輶��
  //uint8_t Current_Scan_Addr[Rout_series_Size];  //��ǰɨ���ַ
  //uint8_t Stop_Scan_Case;                       //ֹͣɨ��ԭ��
  uint8_t Path_Number;                          //·�ɱ��·����
  uint8_t Path_Tab[MainRout_Path_Size][Rout_series_Size + 1];  //��Ҫ��һ������
  HKMainRout_Path_TypDef Path_Tab[MainRout_Path_Size];   //ÿ���߼���ַ��·���������ȼ�������0��ߣ� 
}HKMainRout_Tab_TypDef;


//�������������ʱ·�ɱ�������RAM��
typedef struct
{
  uint8_t   Path_CurrentVal;                     //��ǰ���ڵ��õ�·��
  uint8_t   Path_Call_Cnt;                        //��ǰ·���ĵ��ô���
  uint8_t   RoutTab_Call_Cnt;                    //�ܸ�·�ɱ�ĵ��ô���
  uint8_t   RoutTab_Call_FinishFLag;             //Ϊ0��ʾ·�ɱ����ڵ����У�Ϊ1��ʾ·�ɱ��ѵ�����ɣ�׼��Ѱ���µ�·��
  uint8_t   Path_Call_AllowTimes;               //·��������õĴ���
  HKMainRout_Tab_TypDef   Node_RoutTab;         //�ڵ��·�ɱ�
}HKMainRout_RamTab_TypDef;

//����·��Ѱ�ҽṹ��

typedef struct
{
  //��������֡
  //uint8_t Rout_Tab[2+Rout_series_Size];                            //����+�ڵ��ַ+·�ɵ�ַ
  uint8_t Current_Access_Number;                //�������ڷ��ʵĽڵ��ַ�����������·�֡
  uint8_t Current_Try_Number;                  //��ǰ����·���ĺ���
  uint8_t Current_Try_Series;                  //��ǰ����·���ļ���
  uint8_t Current_Relay_Number;                      //��ǰ���ڸ�����·���м̵ĺ���
  //uint8_t SearchTarget_Addr;                    //���ڲ���·����Ŀ���ַ
  //uint8_t Current_Scan_Addr[Rout_series_Size];  //��ǰɨ���ַ
  
  
  uint8_t PathTry_Flag;            //·��Ѱ�ұ�־
  
  
  uint8_t Node_Number;
  uint8_t NodeStaus_Tab[2][Node_Size];     //һά��ʾ�ڵ��ַ����ά��ʾ�ڵ�״̬��
  //uint8_t NodeStaus_Tab[Node_Size];    //��15-12λΪ0��ʾ����δȷ�ϣ�������ȷ�Ϻã�
                                        //��11-8λ��ʾ�˵�ַ���ڵļ�����8λ��ʾ���߼���ֵַ
  
  uint16_t Try_Gap_Cnt;               //һ֡������ɺ󣬿�ʼ���ü�϶�������Ա㵽ʱ������һ֡
  uint16_t Try_Gap_Time;               //���ʱ�䣬������·������·�ɵĵ�ַ����ȷ��
  
}Node_Try_TypDef;
*/

/*
typedef struct
{
  uint8_t Current_Access_Number;                  //���ڷ��ʵĽڵ��ַ�����������·�֡
  
  uint8_t Current_Try_Number;                     //��ǰ����·���ĺ���
  uint8_t Current_Try_Series;                     //��ǰ����·���ļ���
  uint8_t Current_Relay_Number;                   //��ǰ���ڸ�����·���м̵ĺ���

  //uint8_t Node_Number;
  //uint8_t NodeStaus_Tab[2][RoutNode_Size];        //һά��ʾ�ڵ��ַ����ά��ʾ�ڵ�״̬��
  uint8_t NodeStaus_Tab[RoutNode_Size];                       //bit8��ʾ�Ƿ�����·�ɣ�bit4��ʾ���߼���ַ�Ƿ���ڣ�bit3:0��ʾ�˽ڵ����ڵļ���
  uint16_t Try_Gap_Cnt;                           //һ֡������ɺ󣬿�ʼ���ü�϶�������Ա㵽ʱ������һ֡
  uint16_t Try_Gap_Time;                          //���ʱ�䣬������·������·�ɵĵ�ַ����ȷ��
  
}HKPath_Try_TypDef;
*/











///////////////////////////////////////////////////////////////////////////////////////
#define Network_Delay               1000     //1000*8m��


#define RoutPath_Size                  3               //·�ɵ����·����
#define RoutSeries_Size                3               //·�ɵ������
#define RoutNode_Size                 255             //·�ɵ����ڵ���0-254
#define Try_Cnt_Max                   10              //����Դ���
//#define RoutNode_Start                 0             //·�ɵ���ʼ�ڵ���

//#pragma pack(push) //�������״̬
//#pragma pack(4)//�趨Ϊ4�ֽڶ��� 

//1ҳΪ1024�ֽڣ����Դ洢1024/16 = 64���߼���ַ
//һ��Ҫ��4K���洢��

typedef  struct
{
  uint8_t Current_Path_Number;
  uint8_t Path_Number;  //·�ɱ��·����
  uint8_t AA;         //Ϊ�˶���ı��ñ���
  uint8_t BB;         //Ϊ�˶���ı��ñ���
  uint8_t Path_Tab[RoutPath_Size][RoutSeries_Size + 1];           //��Ҫ��һ������ 
}HKRout_Tab_TypDef;  //��ռ��16�ֽ�

//#pragma pack(pop)//�ָ�����״̬
//#pragma pack () 
typedef struct
{
  uint8_t Distance_Addr;                      //·�����Ե�Ŀ���ַ
  uint8_t Frame_Len;                          //ԭ��֡���ܳ���
  uint8_t Current_Try_Series;                 //��ǰ����·���ļ���
  uint8_t Current_Try_Number1;               //��ǰ����·����1����ַ����
  uint8_t Current_Try_Number2;               //��ǰ����·����2����ַ����
  //uint8_t Current_Try_Number3;             //��ǰ����·����3����ַ����

  uint8_t Node_Number;                        //��ַ�б����
  uint8_t LogicAddr_Tab[RoutNode_Size];      //�߼���ַ��  
}HKPath_Try_TypDef;




/************************* �ⲿ���� ***************************/
extern uint8_t Dis_TryFlag;
extern uint8_t Try_Cnt;
extern uint16_t Frame_Wait_Cnt;                 
extern uint16_t Frame_WaitOver_Time;           


extern HKRout_Tab_TypDef Rout_TempTab;               
extern HKPath_Try_TypDef Rout_TryBuf;              
extern uint8_t RoutRegion[3+RoutSeries_Size];       
extern uint8_t RoutFrame_buf[255]; 



/************************* �ⲿ���� ***************************/
uint8_t Frame_RoutRegion_Add(uint8_t *p_source, uint8_t len, uint8_t *pp);   //���·��
uint8_t Frame_RoutRegion_Delete(uint8_t *p_source);  //·����ɾ��
void RoutPath_Read(uint8_t node);                    //���ݽڵ����·�ɵ���ʱ·�ɱ�
void RoutPath_Save(HKRout_Tab_TypDef *p_tab, uint8_t *pp, uint8_t node);   //��·�����浽·�ɱ���
void Path_Try_Process(uint8_t *p_souce, uint8_t *pp, HKPath_Try_TypDef *p_ps, HKRout_Tab_TypDef *pr);
void Receive_LogicAddrTab_Process(uint8_t *p_logic, HKPath_Try_TypDef *p_ps);

#endif

















