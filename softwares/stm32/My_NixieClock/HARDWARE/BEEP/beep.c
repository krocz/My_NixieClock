#include "beep.h"
#include "hv57708.h"
#include "usart.h"
u16 arr, ccr, sum;
u8* n;
float* b;
u8 play_song_flag;


u16 hz[30] = {
	0, 262, 294, 330, 349, 392, 440, 494, 0, 0, 
	0, 523, 587, 659, 698, 784, 880, 988, 0, 0,
	0, 1046, 1175, 1318, 1397, 1568, 1760, 1976, 0, 0
};

u8 note1[] = {
	0, 0, 15, 15,
	16, 15, 21, 
	17, 15, 15,
	16, 15, 22,
	21, 15, 15,
	25, 23, 21, 
	17, 16, 
	0, 0, 24, 24,
	23, 21, 22,
	21, 
	21,
	
	0, 0, 15, 15,
	16, 15, 21, 
	17, 15, 15,
	16, 15, 22,
	21, 15, 15,
	25, 23, 21, 
	17, 16, 
	0, 0, 24, 24,
	23, 21, 22,
	21, 
	21,
	255
};

float beat1[] = {
	1, 1, 0.5, 0.5,
	1, 1, 1,
	2, 0.5, 0.5,
	1, 1, 1,
	2, 0.5, 0.5,
	1, 1, 1,
	1, 2,
	1, 1, 0.5, 0.5,
	1, 1, 1,
	2,
	2,
	
	1, 1, 0.5, 0.5,
	1, 1, 1,
	2, 0.5, 0.5,
	1, 1, 1,
	2, 0.5, 0.5,
	1, 1, 1,
	1, 2,
	1, 1, 0.5, 0.5,
	1, 1, 1,
	3,
	3,
	0
};


uint8_t sovietNote[] = { 
	21, 0, 5,
	21, 5, 6, 7, 3,
	16, 15, 14, 15, 11,
	12, 13, 14, 15,
	16, 17, 11, 22, 15,
	
	23, 22, 21, 22, 15,
	21, 17, 16, 17, 13, 
	16, 15, 14, 15, 11, 
	21, 17, 16, 5,
	
	23, 22, 21, 17, 21,
	22, 15, 
	21, 17, 16, 15, 16,
	17, 13, 13, 0,
	21, 16, 17, 21, 16, 17,
	
	21, 16, 21, 14, 0, 
	24, 23, 22, 21, 22, 
	23, 21, 
	22, 21, 17, 16, 17,
	21, 16, 
	
	21, 17, 16, 15, 11,
	21, 17, 16, 15, 0, 15,
	
	// 第二节
	21, 5, 6, 7, 3,
	16, 15, 14, 15, 11,
	12, 13, 14, 15,
	16, 17, 11, 22, 15,
	
	23, 22, 21, 22, 15,
	21, 17, 16, 17, 13, 
	16, 15, 14, 15, 11, 
	21, 17, 16, 5,
	
	23, 22, 21, 17, 21,
	22, 15, 
	21, 17, 16, 15, 16,
	17, 13, 13, 0,
	21, 16, 17, 21, 16, 17,
	
	21, 16, 21, 14, 0, 
	24, 23, 22, 21, 22, 
	23, 21, 
	22, 21, 17, 16, 17,
	21, 16, 
	
	21, 17, 16, 15, 11,
	21, 17, 16, 15, 0, 15,
	
	// 第三节
	21, 5, 6, 7, 3,
	16, 15, 14, 15, 11,
	12, 13, 14, 15,
	16, 17, 11, 22, 15,
	
	23, 22, 21, 22, 15,
	21, 17, 16, 17, 13, 
	16, 15, 14, 15, 11, 
	21, 17, 16, 5,
	
	23, 22, 21, 17, 21,
	22, 15, 
	21, 17, 16, 15, 16,
	17, 13, 13, 0,
	21, 16, 17, 21, 16, 17,
	
	21, 16, 21, 14, 0, 
	24, 23, 22, 21, 22, 
	23, 21, 
	22, 21, 17, 16, 17,
	21, 16, 
	
	21, 17, 16, 15, 11,
	15, 16, 17,
	21, 
    255, 	
};

float sovietBeat[] = { 
	2, 0.5, 0.5,
	1, 0.75, 0.25, 1, 1,
	6, 0.75, 0.25, 1 , 1,
	1.75, 0.25, 1.75, 0.25,
	1, 0.75, 0.25, 1.5, 0.5,
	
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 2, 
	
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 0.5, 1, 1, 
	1, 0.5, 0.5, 1, 0.5, 0.5,
	
	1, 0.75, 0.25, 1.5, 0.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	
	1, 0.5, 0.5, 1, 1,
	1, 0.75, 0.25, 1, 0.5, 0.5,
	
	
	//第二节
	1, 0.75, 0.25, 1, 1,
	6, 0.75, 0.25, 1 , 1,
	1.75, 0.25, 1.75, 0.25,
	1, 0.75, 0.25, 1.5, 0.5,
	
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 2, 
	
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 0.5, 1, 1, 
	1, 0.5, 0.5, 1, 0.5, 0.5,
	
	1, 0.75, 0.25, 1.5, 0.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	
	1, 0.5, 0.5, 1, 1,
	1, 0.75, 0.25, 1, 0.5, 0.5,
	
	// 第三节
	1, 0.75, 0.25, 1, 1,
	6, 0.75, 0.25, 1 , 1,
	1.75, 0.25, 1.75, 0.25,
	1, 0.75, 0.25, 1.5, 0.5,
	
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 1, 1,
	1, 0.75, 0.25, 2, 
	
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 0.5, 1, 1, 
	1, 0.5, 0.5, 1, 0.5, 0.5,
	
	1, 0.75, 0.25, 1.5, 0.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	2, 0.5, 0.5, 0.5, 0.5,
	1.5, 2.5,
	
	1, 0.5, 0.5, 1, 1,
	2, 1, 1,
	4,
	0
};


void BEEP_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = BEEP_PWM_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);

	TIM_DeInit(TIM3);
	TIM_TimeBaseStructure.TIM_Period = 1912 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	//使能中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
}

void play_song(u8 number_of_song)
{
	if(number_of_song == 1)
	{ 	n = note1;   b = beat1;}
	else if(number_of_song == 2)
	{   n = sovietNote;  b = sovietBeat;}
	set_new_tim_info(hz[*n++], *b++);
	TIM_Cmd(TIM3, ENABLE);	
}

void set_new_tim_info(u16 hz, float beat_time)
{
	if(hz)
	{
		arr = TIM_HZ / hz;
		ccr = arr >> 1;
		sum = beat_time * BEAT_TIME * TIM_HZ / arr;
	} else    // hz==0，对应休止符，随便设一个arr，然后ccr为0就行
	{
		arr = 5000;
		ccr = 0;
		sum = beat_time * BEAT_TIME * TIM_HZ / arr;
	}
	TIM3->ARR = arr - 1;   
	TIM_SetCompare3(TIM3, ccr);
}

void TIM3_IRQHandler(void)
{
	static u16 cnt = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		cnt++;
		if(cnt >= sum)
		{
			cnt = 0;
			if(*n == 255)  //说明到了歌曲结尾
			{
				TIM_SetCompare3(TIM3, 0);
				TIM_Cmd(TIM3, DISABLE);
				play_song_flag = 2;
			} else
			{
				set_new_tim_info(hz[*n++], *b++);					
			}
			
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}




