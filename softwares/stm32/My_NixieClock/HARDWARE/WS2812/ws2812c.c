#include "dma1.h"
#include "ws2812c.h"

#define WS2812C_LED_QUANTITY	4

uint32_t WS2812C_Buf[WS2812C_LED_QUANTITY];	//0xGGRRBB
uint16_t WS2812C_Bit[24*WS2812C_LED_QUANTITY+1];
uint8_t WS2812C_Flag;

void Timer4_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(WS2812C_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = WS2812C_PWM_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WS2812C_GPIO_PORT, &GPIO_InitStructure);

	TIM_DeInit(TIM4);
	TIM_TimeBaseStructure.TIM_Period = 90-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	
	TIM_DMAConfig(TIM4, TIM_DMABase_CCR3, TIM_DMABurstLength_1Transfer);
	TIM_DMACmd(TIM4, TIM_DMA_Update,ENABLE);
	
	TIM_Cmd(TIM4, DISABLE);
}

void WS2812C_IRQHandler(void);

void WS2812C_Init(void)
{
	DMA1_SetIRQHandler(WS2812C_IRQHandler);
	DMA1_Init((uint32_t)(&WS2812C_Bit));
	Timer4_init();
}

void WS2812C_ClearBuf(void)
{
	uint8_t i;
	for(i=0;i<WS2812C_LED_QUANTITY;i++)
	{
		WS2812C_Buf[i]=0x000000;
	}
}

void WS2812C_SetBuf(uint32_t Color)
{
	uint8_t i;
	for(i=0;i<WS2812C_LED_QUANTITY;i++)
	{
		WS2812C_Buf[i]=Color;
	}
}

void WS2812C_UpdateBuf(void)
{
	uint8_t i,j;
	for(j=0;j<WS2812C_LED_QUANTITY;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812C_Buf[j]&(0x800000>>i)){WS2812C_Bit[j*24+i+1]=60;}
			else{WS2812C_Bit[j*24+i+1]=30;}
		}
	}
	DMA1_Start(24*WS2812C_LED_QUANTITY+1);
	TIM_Cmd(TIM4, ENABLE);
	while(WS2812C_Flag==0);
	WS2812C_Flag=0;
}

void WS2812C_IRQHandler(void)
{
	TIM_SetCompare3(TIM4, 0);
	TIM_Cmd(TIM4, DISABLE);
	WS2812C_Flag=1;
}
