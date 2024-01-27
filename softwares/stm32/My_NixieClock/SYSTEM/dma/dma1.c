#include "dma1.h"
#include "stm32f10x.h"  

void (*DMA1_Handler)(void);

void DMA1_Init(uint32_t MemoryBaseAddr)
{
	DMA_InitTypeDef DMA_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel7);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&TIM4->CCR3);  //外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = MemoryBaseAddr;        // 存储器地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;            // 以外设为目标(从存储器到外设)
	DMA_InitStructure.DMA_BufferSize = 0;                         // 传输数目
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;           // 存储器自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 半字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                 // 单次传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;         // 通道优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                  
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);    // 中断产生的事件为全部数据传输完成
	
	DMA_Cmd(DMA1_Channel7, ENABLE);
}

void DMA1_SetIRQHandler(void (*IRQHandler)(void))
{
	DMA1_Handler=IRQHandler;
}

void DMA1_Start(uint16_t DataNumber)
{
	DMA_Cmd(DMA1_Channel7,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel7,DataNumber);
	DMA_Cmd(DMA1_Channel7,ENABLE);
}

void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC7))
	{
		DMA1_Handler();   
		DMA_ClearFlag(DMA1_FLAG_TC7);
	}
}

