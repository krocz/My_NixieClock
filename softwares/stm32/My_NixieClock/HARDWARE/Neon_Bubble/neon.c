#include "neon.h"

void Neon_Bubble_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(NEON_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = NEON_GPIO_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //ÍÆÍìÊä³ö
	GPIO_Init(NEON_GPIO_PORT, &GPIO_InitStructure); 
}


