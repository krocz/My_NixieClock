/**
 * HV57708����
 * оƬ��飺
 *     32MHz, 64ͨ������������벢��оƬ
 *     ���� 4 �� ���е� 16 λ��λ�Ĵ���
 * ���ߣ�    Blanboom
 * �����£�2014-07-17
 * http://blanboom.org
 *****************************************************************************
 * ʹ�ñ�����ʱ����Ҫ�ڴ˴��� PinDef.h �ж��� HV57708 �ĸ������ţ����������
 *     HV57708_CLK - CLK
 *     HV57708_LE  - LE
 *     HV57708_POL - POL
 *     HV57708_DI1 - Din1/Dout4(A)
 *     HV57708_DI2 - Din2/Dout3(A)
 *     HV57708_DI3 - Din3/Dout2(A)
 *     HV57708_DI4 - Din4/Dout1(A)
 *****************************************************************************
 *     HG57708 �� BL �Ӹߵ�ƽ��DIR �ӵ͵�ƽʱ��ʱ���ƴ棬�Ӹߵ�ƽʱ˳ʱ���ƴ�
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
 * ��飺HV57708 ��ʼ��
 * ���ܣ�HV57708 ��ʼ�������������ģʽ
 * ���룺
 *       HV57708_Mode:
 *           HV57708_MODE_NORMAL    - ��׼���ģʽ
 *           HV57708_MODE_INVERTION - �������ģʽ
 * �������
 */
void HV57708_Init(u8 HV57708_Mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(HV57708_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HV57708_CLK_PIN|HV57708_LE_PIN|HV57708_POL_PIN|
			HV57708_DI1_PIN|HV57708_DI2_PIN|HV57708_DI3_PIN|HV57708_DI4_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;														//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;														//�ٶ�50MHZ
	GPIO_Init(HV57708_GPIO_PORT, &GPIO_InitStructure);

	HV57708_POL = 0;  //�͵�ƽ�������ڷ���ģʽ��д1���0

	HV57708_DI1 = 0;
	HV57708_DI2 = 0;
	HV57708_DI3 = 0;
	HV57708_DI4 = 0;

	HV57708_CLK = 0;
	HV57708_LE = 0;   //�������棬��ʱ����������α任����������ֵ����
}

/**
 * void HV57708_SendData(u32 datapart1, u32 datapart2)
 * ��飺�� HV57708 ��������
 * ���ܣ��� HV57708 ���� 64 λ������
 * ���룺
 *       datapart1 - ��һ�������ݣ�32λ
 *       datapart2 - �ڶ��������ݣ�32λ
 * �������
 */
void HV57708_SendData(u32 datapart2, u32 datapart1)
{
	u8 i;
	u32 tmp;
	tmp = datapart1;
	/* 
	datapart2 ~ datapart1
	�ӵ�λ����λ��˳��д�룬
	����������У�HVOUT(64)��Ӧ�����ݾͻ���λ��DI4�Ĵ����ĵ�16λ��
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
 * ��飺�� HV57708 �Ĵ����е��������������
 * ���ܣ��� HV57708 �Ĵ����е��������������
 * ���룺��
 * �������
 */
void HV57708_OutputData(void)
{
	HV57708_LE = 1;  //����д��������
	Delay(50);
	HV57708_LE = 0;  //��������
	Delay(20);
}

/**
 * void HV57708_Display(u8 *data)
 * ��飺�ڻԹ������ʾ����
 * ���ܣ��ڻԹ������ʾ����
 * ���룺data�������0��ʼ����ӦPCB���ϴ���ʼ�ĻԹ��
		 data��Ԫ��0~9��Ӧ��Ӧ����ʾ���֣�Ԫ��10��Ӧ��С���㣬11��Ӧ��С���㣬12��Ӧ����ʾ
		 show_decimal_point: Ϊ1����ʾ��2���ƹܵ��ұߵ�С���㣨��Ϊ����һ����ͬʱ��ʾ���ֺ�С���㣩
 * �������
 */
void HV57708_Display(u8 *data, u8 show_decimal_point)
{	

	u8 hvout_x, i, temp;  	
	u32 LSB = 0, MSB = 0;
	temp = data[2];  data[2] = data[3];  data[3] = temp;
	printf("data : %d  %d  %d  %d\r\n", data[0],data[1],data[2],data[3]);
	for(i = 0; i < 4; i++)
	{	
		//�����޸�data[i]��ֵ��ʹ֮��PCB��Ƶ��������ж�Ӧ
		if (data[i] == 12) continue;
		if(data[i] >= 1 && data[i] <= 9) temp = data[i];
		else if (data[i] == 0) temp = 10;
		else if (data[i] == 10) temp = 0;
		else temp = 11;
		hvout_x = 63 - (i * 12 + temp); //ȷ���Թ����ʾ���ֶ�ӦHV57708��HVOUT(x), 0~63
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
  * ��飺 �����������Թ��ѭ����ʾ0-9����С���㣬��С����
  * ���ܣ� �Թ��ѭ����ʾ0-9
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
	/* --------------------------����TIM3 --------------------------*/
	//����TIM��ʱ��ģ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	// ����TIM����ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//Ϊ��ʱ�������ڲ�ʱ��Դ
	TIM_InternalClockConfig(TIM3);
	

	//�ò���������ʱ��Դ���˲�ģ��Ĳ���Ƶ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//����100us����1���ж�
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	//ʹ���ж�
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

//�����ûԹ����crossfade��ʽ�л����֣�����Ч���ܲ�
//void TIM3_IRQHandler(void)
//{
//	static u8 cnt;
//	static int light_rank = 99;
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
//	{
//		cnt++;
//		if(cnt == light_rank)  now_data_show_flag = 1;
//		
//		if(cnt == 100)  //10ms�ѵ�
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
