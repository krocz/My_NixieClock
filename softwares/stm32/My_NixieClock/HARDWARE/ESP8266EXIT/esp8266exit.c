#include "esp8266exit.h"
#include "usart.h"
#include <time.h>

u32 epoch_time;
u8 ESP8266_RX_FLAG = 0;
TIMEData beijing_time;

void init_esp8266exit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(WiFi_GPIO_CLK, ENABLE);    //先使能外设IO PORTB时钟 
		
	GPIO_InitStructure.GPIO_Pin = WiFi_SCL_PIN | WiFi_SDA_PIN;	 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 // 输入捕获功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(WiFi_GPIO_PORT, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
	
	
	GPIO_EXTILineConfig(WiFi_PORT_SOURCE, WiFi_PIN_SOURCE);
	EXTI_InitStructure.EXTI_Line = WiFi_EXIT_LINE;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	  
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = WiFi_NVIC_SOURCE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}


void convert_beijing_time(long long epochTime)
{
	struct tm *p = localtime((time_t *)&epochTime);
	//printf("%lld\r\n", epochTime);
	//printf("day : %d\r\n", p->tm_mday);
	beijing_time.day = p->tm_mday;
	beijing_time.hour = p->tm_hour;
	beijing_time.minute = p->tm_min;
	beijing_time.month = 1 + p->tm_mon;
	beijing_time.second = p->tm_sec + 1;
	beijing_time.week = (p->tm_wday)? p->tm_wday : 7;
	beijing_time.year = 1900 + p->tm_year;
}


void EXTI15_10_IRQHandler(void)
{
	uint8_t temp;
	static u8 RxState = 0;
	static u8 cnt = 0;
	static u16 bag_head = 0, bag_hail = 0;
	if (EXTI_GetITStatus(WiFi_EXIT_LINE) == SET)
	{

		temp = GPIO_ReadInputDataBit(WiFi_GPIO_PORT, WiFi_SDA_PIN);
		if(RxState == 0)
		{
			bag_head = (bag_head << 1) | temp;
			cnt++;
			if(cnt == 16)
			{
				if(bag_head == BAG_HEAD)
				{  RxState = 1;}
				cnt = 0;
				bag_head = 0;
			}
		} 
		else if(RxState == 1)
		{
			epoch_time = (epoch_time << 1) | temp;
			cnt++;
			if(cnt == 32)
			{
				RxState = 2;
				cnt = 0;
			}
		}
		else if(RxState == 2)
		{
			bag_hail = (bag_hail << 1) | temp;
			cnt++;
			if(cnt == 16)
			{
				if(bag_hail == BAG_HAIL)
				{
					ESP8266_RX_FLAG = 1;
				} else {
					epoch_time = 0;
				}
				cnt = 0;
				bag_hail = 0;
				RxState = 0;
			}
		}
		EXTI_ClearITPendingBit(WiFi_EXIT_LINE);
	}	
}



