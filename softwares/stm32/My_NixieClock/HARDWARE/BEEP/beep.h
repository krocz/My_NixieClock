#ifndef __BEEP_H
#define __BEEP_H
#include "sys.h"

#define BEAT_TIME 0.5   // 定义一拍的长度是0.5s
#define PSC 72
#define TIM_HZ ((72e6) / PSC)

extern u8 play_song_flag;

void BEEP_GPIO_Init(void);
void play_song(u8 number_of_song);
void set_new_tim_info(u16 hz, float beat_time);
#endif

