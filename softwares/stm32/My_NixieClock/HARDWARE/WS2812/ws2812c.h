#ifndef __WS2812C_H
#define __WS2812C_H

#include "stm32f10x.h"
#include "sys.h"

extern uint32_t WS2812C_Buf[];	//0xGGRRBB

void Timer4_init(void);
void WS2812C_Init(void);
void WS2812C_ClearBuf(void);
void WS2812C_SetBuf(uint32_t Color);
void WS2812C_UpdateBuf(void);

#endif
