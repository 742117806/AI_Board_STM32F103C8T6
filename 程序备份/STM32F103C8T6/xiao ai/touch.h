#ifndef TOUCH_H
#define TOUCH_H

//#include "peri_includes.h"
#include "frame_process.h"
#include "main.h"


#define Touch_ShortTime            1u    //�̰�ֵ
#define Touch_FilterTime           8u    //�˲�ֵ
#define Touch_LongTime             (125*15)   //����ֵ
#define Touch_DisTime              100u   //���ֵ

#define Key_NoVal                 0u  
#define Key_ShortVal              1u
#define Key_LongVal               2u  




#define   Touch_Unchanged          0u
#define   Touch_RiseEdge           1u
#define   Touch_FallEdge           2u




#define Key1_ShortVal              0x01
#define Key1_LongVal               0x10
#define Key2_ShortVal              0x02
#define Key2_LongVal               0x20
#define Key3_ShortVal              0x04
#define Key3_LongVal               0x40
#define Key4_ShortVal              0x08
#define Key4_LongVal               0x80









/************************* �ⲿ���� ***************************/
void Touch1_Detect(DevicePara_TypDef *p_fashionpara);
void Touch2_Detect(DevicePara_TypDef *p_fashionpara);
void Touch3_Detect(DevicePara_TypDef *p_fashionpara);
void Touch4_Detect(DevicePara_TypDef *p_fashionpara);


#endif

