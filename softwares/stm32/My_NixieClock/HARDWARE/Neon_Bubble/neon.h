#ifndef __NEON_H
#define __NEON_H

#include "stm32f10x.h"
#include "sys.h"

#define NEON_BUBBLE_OPEN()              GPIO_SetBits(NEON_GPIO_PORT, NEON_GPIO_PIN)
#define NEON_BUBBLE_CLOSE()             GPIO_ResetBits(NEON_GPIO_PORT, NEON_GPIO_PIN)
#define NEON_BUBBLE_REVERSE()           GPIO_WriteBit(NEON_GPIO_PORT, NEON_GPIO_PIN, (BitAction)(1-GPIO_ReadOutputDataBit(NEON_GPIO_PORT, NEON_GPIO_PIN)))

void Neon_Bubble_Init(void);

#endif
