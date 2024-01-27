#ifndef __HV57708_H
#define __HV57708_H
#include "sys.h"
/*
 *     HV57708_CLK - CLK
 *     HV57708_LE  - LE
 *     HV57708_POL - POL
 *     HV57708_DI1 - Din1/Dout4(A)
 *     HV57708_DI2 - Din2/Dout3(A)
 *     HV57708_DI3 - Din3/Dout2(A)
 *     HV67708_DI4 - Din4/Dout1(A)
*/
#define HV57708_MODE_NORMAL 1     //正向输出模式
#define HV57708_MODE_INVERTION 0  //反向输出模式

void Delay(u32 time);
void HV57708_Init(u8 HV57708_Mode);
void HV57708_SendData(u32 datapart2, u32 datapart1);
void HV57708_OutputData(void);
void HV57708_Display(u8 *data, u8 show_decimal_point);
//void SetDisplay(unsigned char data[]);
void HV57708_Protection(void);
void Timer3_Init(void);
void Show_With_Crossfade(u8 *last_data, u8 *now_data);
void HV57708_Display_None(void);

#endif
