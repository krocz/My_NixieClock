/**
 * HV57708驱动
 * 芯片简介：
 *     32MHz, 64通道推挽输出串入并出芯片
 *     内有 4 个 并行的 16 位移位寄存器
 * 作者：    Blanboom
 * 最后更新：2014-07-17
 * http://blanboom.org
 *****************************************************************************
 * 使用本程序时，需要在此处或 PinDef.h 中定义 HV57708 的各个引脚，具体包括：
 *     HV57708_CLK - CLK
 *     HV57708_LE  - LE
 *     HV57708_POL - POL
 *     HV57708_DI1 - Din1/Dout4(A)
 *     HV57708_DI2 - Din2/Dout3(A)
 *     HV57708_DI3 - Din3/Dout2(A)
 *     HV57708_DI4 - Din4/Dout1(A)
 *****************************************************************************
 *     HG57708 的 BL 接高电平，DIR 接低电平时逆时针移存，接高电平时顺时针移存
 */
 
#include "hv57708.h"
#include "delay.h"
#include "usart.h"

u8 last_data_show_flag, now_data_show_flag, show_over_flag;


void Delay(u32 time)
{
	u32 i,j;
	for(i = 0;i < time;i++)
		for(j = 0;j < 250;j++);
}
/**
 * void HV57708_Init(bool HV57708_Mode)
 * 简介：HV57708 初始化
 * 功能：HV57708 初始化，并设置输出模式
 * 输入：
 *       HV57708_Mode:
 *           HV57708_MODE_NORMAL    - 标准输出模式
 *           HV57708_MODE_INVERTION - 反相输出模式
 * 输出：无
 */
void HV57708_Init(u8 HV57708_Mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(HV57708_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HV57708_CLK_PIN|HV57708_LE_PIN|HV57708_POL_PIN|
			HV57708_DI1_PIN|HV57708_DI2_PIN|HV57708_DI3_PIN|HV57708_DI4_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;														//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;														//速度50MHZ
	GPIO_Init(HV57708_GPIO_PORT, &GPIO_InitStructure);

	HV57708_POL = 0;  //低电平，工作于反向模式，写1输出0

	HV57708_DI1 = 0;
	HV57708_DI2 = 0;
	HV57708_DI3 = 0;
	HV57708_DI4 = 0;

	HV57708_CLK = 0;
	HV57708_LE = 0;   //允许锁存，此时无论输入如何变换，锁存器的值不变
}

/**
 * void HV57708_SendData(u32 datapart1, u32 datapart2)
 * 简介：向 HV57708 发送数据
 * 功能：向 HV57708 发送 64 位的数据
 * 输入：
 *       datapart1 - 第一部分数据，32位
 *       datapart2 - 第二部分数据，32位
 * 输出：无
 */
void HV57708_SendData(u32 datapart2, u32 datapart1)
{
	u8 i;
	u32 tmp;
	tmp = datapart1;
	/* 
	datapart2 ~ datapart1
	从低位到高位的顺序写入，
	在这个过程中，HVOUT(64)对应的数据就会移位到DI4寄存器的第16位上
	*/
	for(i = 0; i < 8; i++)
    {
		HV57708_CLK = 0;

		HV57708_DI4 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		HV57708_DI3 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		HV57708_DI2 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		HV57708_DI1 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		Delay(20);
		HV57708_CLK = 1;
		Delay(20);
		HV57708_CLK = 0;
	}
	tmp = datapart2;
	for(i=0; i < 8; i++)
	{
		HV57708_CLK = 0;
		HV57708_DI4 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		HV57708_DI3 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		HV57708_DI2 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		HV57708_DI1 = (tmp & 0x00000001);
		tmp = tmp >> 1;

		Delay(20);
		HV57708_CLK = 1;
		Delay(20);
		HV57708_CLK = 0;
		Delay(20);		
	}
}

/**
 * void HV57708_OutputData(void)
 * 简介：将 HV57708 寄存器中的数据输出至引脚
 * 功能：将 HV57708 寄存器中的数据输出至引脚
 * 输入：无
 * 输出：无
 */
void HV57708_OutputData(void)
{
	HV57708_LE = 1;  //数据写入锁存器
	Delay(50);
	HV57708_LE = 0;  //数据锁存
	Delay(20);
}

/**
 * void HV57708_Display(u8 *data)
 * 简介：在辉光管上显示数据
 * 功能：在辉光管上显示数据
 * 输入：data，数组从0开始，对应PCB板上从左开始的辉光管
		 data的元素0~9对应相应的显示数字，元素10对应左小数点，11对应右小数点，12对应不显示
		 show_decimal_point: 为1则显示第2个灯管的右边的小数点（因为可能一个管同时显示数字和小数点）
 * 输出：无
 */
void HV57708_Display(u8 *data, u8 show_decimal_point)
{	

	u8 hvout_x, i, temp;  	
	u32 LSB = 0, MSB = 0;
	temp = data[2];  data[2] = data[3];  data[3] = temp;
	printf("data : %d  %d  %d  %d\r\n", data[0],data[1],data[2],data[3]);
	for(i = 0; i < 4; i++)
	{	
		//首先修改data[i]的值，使之与PCB设计的引脚排列对应
		if (data[i] == 12) continue;
		if(data[i] >= 1 && data[i] <= 9) temp = data[i];
		else if (data[i] == 0) temp = 10;
		else if (data[i] == 10) temp = 0;
		else temp = 11;
		hvout_x = 63 - (i * 12 + temp); //确定辉光管显示数字对应HV57708的HVOUT(x), 0~63
	    if(hvout_x < 32)
			LSB |= 1 << hvout_x;
		else
			MSB |= 1 << (hvout_x - 32);
	}

	//2_RP  40
	if (show_decimal_point)  
	{
		MSB |= 1 << (40 - 32);
	}
	HV57708_SendData(MSB, LSB);
	HV57708_OutputData();
}

void HV57708_Display_None(void)
{	 	
	u32 LSB = 0, MSB = 0;
	
	HV57708_SendData(MSB, LSB);
	HV57708_OutputData();
}

/*
  * void HV57708_Protection(void)
  * 简介： 阴极保护，辉光管循环显示0-9，左小数点，右小数点
  * 功能： 辉光管循环显示0-9
  */
void HV57708_Protection(void)
{
	u8 i=0;
	u8 j=0;
	u8 data_scan[4];
    for (i = 0; i < 12; i++)
    {
        for(j = 0; j < 4; j++)
			data_scan[j] = i;
        HV57708_Display(data_scan, 0);
		delay_ms(200); 
    }
}

void Show_With_Crossfade(u8 *last_data, u8 *now_data)
{
	TIM_Cmd(TIM3, ENABLE);
	while(!show_over_flag)
	{
		if(last_data_show_flag)
		{
			HV57708_Display(last_data, 0);
			last_data_show_flag = 0;
		}
		if(now_data_show_flag)
		{
			HV57708_Display(now_data, 0);
			now_data_show_flag = 0;			
		}	
	}
	show_over_flag = 0;
	TIM_Cmd(TIM3, DISABLE);
	TIM_SetCounter(TIM3, (u16)0);
}


void Timer3_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* --------------------------配置TIM3 --------------------------*/
	//配置TIM的时基模块
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	// 开启TIM外设使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//为定时器设置内部时钟源
	TIM_InternalClockConfig(TIM3);
	

	//该参数是配置时钟源的滤波模块的采样频率
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//配置100us产生1次中断
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	//使能中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

//控制让辉光管以crossfade方式切换数字，但是效果很差
//void TIM3_IRQHandler(void)
//{
//	static u8 cnt;
//	static int light_rank = 99;
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
//	{
//		cnt++;
//		if(cnt == light_rank)  now_data_show_flag = 1;
//		
//		if(cnt == 100)  //10ms已到
//		{	
//			if(light_rank)  last_data_show_flag = 1;
//			light_rank--;
//			if(light_rank < 0) 
//			{
//				show_over_flag = 1;
//				light_rank = 99;
//			}
//			cnt = 0;
//		}
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//	}
//}
