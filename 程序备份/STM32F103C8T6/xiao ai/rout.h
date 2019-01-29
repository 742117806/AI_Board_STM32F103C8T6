#ifndef ROUT_H_
#define ROUT_H_


#include "stm32f1xx.h"
#include <string.h>
#include "crc16.h"
#include "wireless_app.h"

/*
1. 每次上电时按照逻辑地址是否存在读出EEPROM中相应逻辑地址的路由表，并根据路由级数初始化Commun_SucessVal，
   （0级为255，1级为223，2级为191，3级为159，4级为127，5级为95）再保存到EEPROM中，如相应逻辑地址无路由表，
    添加一个0级路由表到相应逻辑地址。
2. 在收到上层的逻辑地址列表时，检查EEPROM中的相应逻辑地址是否有增加或减少，如有增加，初始化相应的逻辑地址
   路由表；如有减少，删除相应的逻辑地址路由表，并对所有逻辑地址的路由表进行检查，如有使用到此地址的路由表，
   将路由表删除。
3. 每次下发帧时，按成功率依次调用路由表下发（成功率一样，级数小的优先），如超时（超时时间依据路由表级数
  （可为0）确定）未收到正确的返回数据，表示路径失败，此时重试此路径（重试次数（可为0）定义成参数可选，
   级数不同，参数不同）。如路由表调用完还未成功，再重试调用路由表（重试次数（可为0）定义成参数可选），
   重试完路由表后还未成功，表示调用路由表失败。
4. 每次调用路径下发帧时，都要维护路由表结构数据，路径通讯成功值，连续失败数，连续成功数等，并做相应处理。
5. 调用路由表下发失败时，采用轮循法创建路由表下发数据。使用轮循法选择路由地址时，级数从小到大，同级除本身
   地址外，按照从小到到或从大到小顺序选择路由地址。在下发未成功时，一直轮循创建路由表，直到收到上行的其它
   帧或超时（宏定义一个超时参数）。
6. 如创建的路由表下发成功，即停止轮循。此时根据路由级数初始化Commun_SucessVal（0级为255，1级为223，2级为191，
   3级为159，4级为127，5级为95），并将路由表信息保存到EEPROM中。
7. 不管创建的路由表是否下发成功，在每次轮循结束时，应当记录当前所停止轮循的的路由地址信息，以便下一次轮循时
   也可从断点处继续执行轮循。

8. 如使用某条路径下发帧连续失败计数达到一定值A？时（级数不同，参数也不同），将Commun_SucessVal值设为64（如大于64），
   超过一定数B？将此路由表删除（0级除外）；如使用某条路径下发帧连续成功计数达到一定数C？时（级数不同，参数也不同），
   将Commun_SucessVal值设为192（如小于192），超过一定数D？时，将此值设为255。
9. 路径通讯成功值的维护。每次收到正确的下行帧时，表示通讯成功，将相应路由表中的成功值增大。如路由表不存在，则创建
   路由表并初始化Commun_SucessVal，如果路由表个数超出，则删除成功值最少的那个路由表。如下发帧超时未收到回应帧，表示
   通讯失败，此时将相应路由表中的成功值减少。每次的增大量和减少量依不同的级数而不同。

10. 帧配置码的第7位为1，表示有路由表，为0表示无路由表。为了与先前的软件兼容，路由表加在校验码之前。
11. 路由表结构（四个域组成）：路由表级数（1Byte）+路由剩余级数域（1Byte）+ 帧所属节点地址域（1Byte）+路由表（n Byte）。
   路由长度域：表示路由表的总级数。
   路由剩余级数域：表示转发所剩余路由级数。例如3级路由表，总级数就是3，第一次组帧时，
               剩余级数就是3，每转发一次，将剩余级数减1，并重新设置帧的逻辑地址。
                   地址信息为自身的节点收到帧后，判断剩余级数是否为0：当剩余级数不为0时，组帧后转发此帧；当剩余级
               数为0时，如是下行帧，处理此帧数据和路由信息（以便上报）；如是上行帧，不予处理。
                   扩展板每次收到下行帧时，应当判断帧是否为路由帧，如果不是路由帧，向核心板转发；如果是路由帧，判
               断剩余级数是否为0，剩余级数不是0不予处理；剩余级数为0时，如是上行帧不予处理；如果是下行帧，对帧进行
               路由信息处理，然后向核心板转发。
   帧所属节点地址域：表示此帧所属的节点地址。当为上行帧时，表示目标地址，当为下行帧时，表示源地址。
   路由表域：  需要转发的路由表地址。
   重新组帧时：先设置剩余级数，再设置逻辑地址，然后转发。
               上行帧：剩余级数如是0节点处理此帧，否则收到后将剩余级数自减1，再将剩余级数减去1后所得的值的那个地址设置为逻辑地址，
               下行帧：剩余级数如是0扩展板处理此帧，否则收到后将剩余级数自减1，将总级数减剩余级数的那个地址设置为逻辑地址

12. 示例（3级路由表，帧下发到ox0e，路由经一级0x0b，二级0x0c，三级0x0d）：
    路由表结构为：
    路由表级数，当前剩余路由级数，帧所属节点地址，三级路由地址，二级路由地址，一级路由地址
上行帧：
扩展板收到核心板的上行帧后，调用或创建路由表
   １）扩展板组帧路由表下发:
　    ０x03, 0x03, 0x0e, 0x0d, 0x0c, 0x0b   ；第一级路由地址0x0b加入到逻辑地址
    2）一级路由地址0x0b组帧路由表下发：
    　０x03, 0x02, 0x0e, 0x0d, 0x0c, 0x0b   ；第二级路由地址0x0c加入到逻辑地址
    3）二级路由地址0x0c组帧路由表下发：
    　０x03, 0x01, 0x0e, 0x0d, 0x0c, 0x0b   ；第三级路由地址0x0d加入到逻辑地址
    4）三级路由地址0x0d组帧路由表下发：
    　０x03, 0x00, 0x0e, 0x0d, 0x0c, 0x0b   ；目标地址0x0e加入到逻辑地址
下行帧：
    1）目标地址0x0e组帧路由表上发：
    　０x03, 0x03, 0x0e, 0x0d, 0x0c, 0x0b   ；目标地址0x0d加入到逻辑地址
    2）三级路由地址0x0d组帧路由表上发：
    　０x03, 0x02, 0x0e, 0x0d, 0x0c, 0x0b   ；第二级路由地址0x0c加入到逻辑地址
    3）二级路由地址0x0c组帧路由表上发：
    　０x03, 0x01, 0x0e, 0x0d, 0x0c, 0x0b   ；第一级路由地址0x0b加入到逻辑地址
    4）一级路由地址0x0b组帧路由表上发：
    　０x03, 0x00, 0x0e, 0x0d, 0x0c, 0x0b   ；源地址0x0e加入到逻辑地址
扩展板收到下行帧后，对路径信息进行维护后，去掉路由表组帧上发到核心板。
*/

/*
#define MainRout_Path_Size      3
#define Node_Size               255
//////////某路径连续成功或连续失败的处理参数//////////////
//连续成功数A（后面的数字带表路由级数，不同的级数参数不同）
#define MainRout_ContinuPass_A0             3
#define MainRout_ContinuPass_A1             6
#define MainRout_ContinuPass_A2             9
#define MainRout_ContinuPass_A3             12
//#define MainRout_ContinuPass_A4             15
//#define MainRout_ContinuPass_A5             18

#define MainRout_SucessValThreshold_A       223

//连续成功数B
#define MRP_ContinuPass_B0             5
#define MRP_ContinuPass_B1             10
#define MRP_ContinuPass_B2             15
#define MRP_ContinuPass_B3             20
//#define MRP_ContinuPass_B4             25
//#define MRP_ContinuPass_B5             30

#define MRP_SucessValThreshold_B       255

//连续失败数C
#define MRP_ContinuFail_C0             18
#define MRP_ContinuFail_C1             15
#define MRP_ContinuFail_C2             12
#define MRP_ContinuFail_C3             9
//#define MRP_ContinuFail_C4             6
//#define MRP_ContinuFail_C5             3

#define MRP_SucessValThreshold_C       64

//连续失败数D
#define MRP_ContinuFail_D0             30    //0级路由暂时设置为永不删除
#define MRP_ContinuFail_D1             25
#define MRP_ContinuFail_D2             20
#define MRP_ContinuFail_D3             15
//#define MRP_ContinuFail_D4             10
//#define MRP_ContinuFail_D5             5

#define MRP_SucessValThreshold_D       0


/////////各级路由成功值的单次增量和减量参数///////////////
#define MRP_SucessVal_IncS0            2
#define MRP_SucessVal_IncS1            2
#define MRP_SucessVal_IncS2            2
#define MRP_SucessVal_IncS3            2
#define MRP_SucessVal_IncS4            2
#define MRP_SucessVal_IncS5            2

#define MRP_SucessVal_SubS0            4        //成功率等于66.7%会保持成功值不变(4/(2+4)*100%)
#define MRP_SucessVal_SubS1            5        //成功率等于71.4%会保持成功值不变(5/(2+5)*100%)
#define MRP_SucessVal_SubS2            6        //成功率等于75%会保持成功值不变(6/(2+6)*100%)
#define MRP_SucessVal_SubS3            7        //成功率等于77.8%会保持成功值不变(7/(2+7)*100%)
//#define MRP_SucessVal_SubS4            8        //成功率等于80%会保持成功值不变(8/(2+8)*100%)
//#define MRP_SucessVal_SubS5            9        //成功率等于81.8%会保持成功值不变(9/(2+9)*100%)


////////////各级路由成功值的最大最小值参数////////////////
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


////////////创建各级路由时成功值的初始值参数/////////////////
#define MRP_SucessVal_InitialS0            255
#define MRP_SucessVal_InitialS1            250
#define MRP_SucessVal_InitialS2            245
#define MRP_SucessVal_InitialS3            240
//#define MRP_SucessVal_InitialS4            235
//#define MRP_SucessVal_InitialS5            230

///////////////////路由调用次数////////////////////////
#define MRP_CallTimes_S0                  1    //最少为1，下同
#define MRP_CallTimes_S1                  1
#define MRP_CallTimes_S2                  1
#define MRP_CallTimes_S3                  1
#define MRP_CallTimes_S4                  1
#define MRP_CallTimes_S5                  1


#define MRP_CallTimes_Tab                 2            //对整个路由表的调用次数

//帧路由域的最大长度： 路由长度（1Byte）+路由级数（1Byte）+ 帧所属节点地址（1Byte）+路由表（n Byte）
#define MRP_RoutRegion_LenMax             (3+Rout_series_Size) 




//主机的节点路径
typedef struct
{
  uint8_t Path_Series_Val;                        //路径级数
  uint8_t Path_Addr[Rout_series_Size];            //路径地址,越靠近目标地址的越靠前
  uint8_t Path_ContinulFail_Cnt;                  //此路径通讯传输连续失败数，级数不同，参数也不同                                          
  uint8_t Path_ContinulPass_Cnt;                  //此路径通讯传输连续成功数，级数不同，参数也不同                                           
  uint8_t Path_SucessVal;                         //此路径通讯成功值（初始值为128）
                                            
  //uint16_t Path_Total_Cnt;                      //此路径通讯传输总数            
  //uint16_t Path_Pass_Cnt;                       //此路径通讯传输成功数
  //uint8_t Path_Pass_Rate;                       //此路径通讯传输成功率
}HKMainRout_Path_TypDef;


//主机的节点路由总表
typedef struct
{
  //uint8_t Current_Scan_Series;                  //当前扫描级数
  //uint8_t Current_Scan_Addr[Rout_series_Size];  //当前扫描地址
  //uint8_t Stop_Scan_Case;                       //停止扫描原因
  uint8_t Path_Number;                          //路由表的路径数
  uint8_t Path_Tab[MainRout_Path_Size][Rout_series_Size + 1];  //需要加一个级数
  HKMainRout_Path_TypDef Path_Tab[MainRout_Path_Size];   //每个逻辑地址的路径表（按优先级来排序，0最高） 
}HKMainRout_Tab_TypDef;


//主机待处理的临时路由表，分配在RAM中
typedef struct
{
  uint8_t   Path_CurrentVal;                     //当前正在调用的路径
  uint8_t   Path_Call_Cnt;                        //当前路径的调用次数
  uint8_t   RoutTab_Call_Cnt;                    //总个路由表的调用次数
  uint8_t   RoutTab_Call_FinishFLag;             //为0表示路由表正在调用中，为1表示路由表已调用完成，准备寻找新的路由
  uint8_t   Path_Call_AllowTimes;               //路径充许调用的次数
  HKMainRout_Tab_TypDef   Node_RoutTab;         //节点的路由表
}HKMainRout_RamTab_TypDef;

//定义路径寻找结构体

typedef struct
{
  //正常传输帧
  //uint8_t Rout_Tab[2+Rout_series_Size];                            //级数+节点地址+路由地址
  uint8_t Current_Access_Number;                //主机正在访问的节点地址，用于正常下发帧
  uint8_t Current_Try_Number;                  //当前尝试路径的号码
  uint8_t Current_Try_Series;                  //当前尝试路径的级数
  uint8_t Current_Relay_Number;                      //当前正在给尝试路径中继的号码
  //uint8_t SearchTarget_Addr;                    //正在查找路径的目标地址
  //uint8_t Current_Scan_Addr[Rout_series_Size];  //当前扫描地址
  
  
  uint8_t PathTry_Flag;            //路径寻找标志
  
  
  uint8_t Node_Number;
  uint8_t NodeStaus_Tab[2][Node_Size];     //一维表示节点地址表，二维表示节点状态表
  //uint8_t NodeStaus_Tab[Node_Size];    //第15-12位为0表示级数未确认，否则已确认好，
                                        //第11-8位表示此地址所在的级，低8位表示此逻辑地址值
  
  uint16_t Try_Gap_Cnt;               //一帧尝试完成后，开始启用间隙计数，以便到时启用另一帧
  uint16_t Try_Gap_Time;               //间隔时间，根据有路由与无路由的地址比例确认
  
}Node_Try_TypDef;
*/

/*
typedef struct
{
  uint8_t Current_Access_Number;                  //正在访问的节点地址，用于正常下发帧
  
  uint8_t Current_Try_Number;                     //当前尝试路径的号码
  uint8_t Current_Try_Series;                     //当前尝试路径的级数
  uint8_t Current_Relay_Number;                   //当前正在给尝试路径中继的号码

  //uint8_t Node_Number;
  //uint8_t NodeStaus_Tab[2][RoutNode_Size];        //一维表示节点地址表，二维表示节点状态表
  uint8_t NodeStaus_Tab[RoutNode_Size];                       //bit8表示是否已有路由，bit4表示此逻辑地址是否存在，bit3:0表示此节点所在的级数
  uint16_t Try_Gap_Cnt;                           //一帧尝试完成后，开始启用间隙计数，以便到时启用另一帧
  uint16_t Try_Gap_Time;                          //间隔时间，根据有路由与无路由的地址比例确认
  
}HKPath_Try_TypDef;
*/











///////////////////////////////////////////////////////////////////////////////////////
#define Network_Delay               1000     //1000*8m秒


#define RoutPath_Size                  3               //路由的最大路径数
#define RoutSeries_Size                3               //路由的最大级数
#define RoutNode_Size                 255             //路由的最大节点数0-254
#define Try_Cnt_Max                   10              //最大尝试次数
//#define RoutNode_Start                 0             //路由的起始节点数

//#pragma pack(push) //保存对齐状态
//#pragma pack(4)//设定为4字节对齐 

//1页为1024字节，可以存储1024/16 = 64个逻辑地址
//一共要用4K来存储。

typedef  struct
{
  uint8_t Current_Path_Number;
  uint8_t Path_Number;  //路由表的路径数
  uint8_t AA;         //为了对齐的备用变量
  uint8_t BB;         //为了对齐的备用变量
  uint8_t Path_Tab[RoutPath_Size][RoutSeries_Size + 1];           //需要加一个级数 
}HKRout_Tab_TypDef;  //共占用16字节

//#pragma pack(pop)//恢复对齐状态
//#pragma pack () 
typedef struct
{
  uint8_t Distance_Addr;                      //路径尝试的目标地址
  uint8_t Frame_Len;                          //原先帧的总长度
  uint8_t Current_Try_Series;                 //当前尝试路径的级数
  uint8_t Current_Try_Number1;               //当前尝试路径的1级地址号码
  uint8_t Current_Try_Number2;               //当前尝试路径的2级地址号码
  //uint8_t Current_Try_Number3;             //当前尝试路径的3级地址号码

  uint8_t Node_Number;                        //地址列表个数
  uint8_t LogicAddr_Tab[RoutNode_Size];      //逻辑地址表  
}HKPath_Try_TypDef;




/************************* 外部变量 ***************************/
extern uint8_t Dis_TryFlag;
extern uint8_t Try_Cnt;
extern uint16_t Frame_Wait_Cnt;                 
extern uint16_t Frame_WaitOver_Time;           


extern HKRout_Tab_TypDef Rout_TempTab;               
extern HKPath_Try_TypDef Rout_TryBuf;              
extern uint8_t RoutRegion[3+RoutSeries_Size];       
extern uint8_t RoutFrame_buf[255]; 



/************************* 外部函数 ***************************/
uint8_t Frame_RoutRegion_Add(uint8_t *p_source, uint8_t len, uint8_t *pp);   //添加路由
uint8_t Frame_RoutRegion_Delete(uint8_t *p_source);  //路由域删除
void RoutPath_Read(uint8_t node);                    //根据节点读出路由到临时路由表
void RoutPath_Save(HKRout_Tab_TypDef *p_tab, uint8_t *pp, uint8_t node);   //将路径保存到路由表中
void Path_Try_Process(uint8_t *p_souce, uint8_t *pp, HKPath_Try_TypDef *p_ps, HKRout_Tab_TypDef *pr);
void Receive_LogicAddrTab_Process(uint8_t *p_logic, HKPath_Try_TypDef *p_ps);

#endif

















