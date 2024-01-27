#include "delay.h"
#include "key.h"

u8 func_flag = 1;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(KEY_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_GPIO_PIN_UP | KEY_GPIO_PIN_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

u8 Key_GetNum(void)
{
	
	if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN_UP) == 0)
	{
		delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN_UP) == 0);
		delay_ms(20);
		func_flag--;
		if(func_flag == 0)   func_flag = 5;
	}
	
	if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN_DOWN) == 0)
	{
		delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN_DOWN) == 0);
		delay_ms(20);
		func_flag++;
		if(func_flag == 6)   func_flag = 1;
	}
	
	return func_flag;
}
