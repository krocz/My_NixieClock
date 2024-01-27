#include "timer2.h"
#include "usart.h"
//#define ANGLE_THRESHOLD 15
//u8 work_status = 0;
u8 Timer2_1s_Flag;
void Timer2_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* --------------------------配置TIM2 --------------------------*/
	//配置TIM的时基模块
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	// 开启TIM外设使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//为定时器设置内部时钟源
	TIM_InternalClockConfig(TIM2);
	

	//该参数是配置时钟源的滤波模块的采样频率
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//配置10ms产生1次中断
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	//使能中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);	
}

//static float f_max(float a , float b)
//{
//	return a > b? a : b;
//}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)	
	{
		Timer2_1s_Flag = 1;
//		printf("TIM2 switch\r\n");
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}

}

//void TIM2_IRQHandler(void)
//{
//	static u8 step, zero_change_cnt, is_first = 1, cnt;
//	static float last, sum, right_span, left_span;
//	static int8_t change_tend;
//	float pitch, roll, yaw, d_value;

//	
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//	{
//		step++;
//		if(step == 5)   //每50ms读取一次数据
//		{
//			mpu_dmp_get_data(&pitch, &roll, &yaw);
//			//printf("pitch: %f   roll: %f   yaw: %f\r\n", pitch, roll, yaw);
//			if(fabs(yaw) > 0.001 && fabs(yaw) < 180)   //剔除脏数据
//			{
//				//printf("change_tend:                                                           %d\r\n", change_tend);
//				if(fabs(pitch) < 30 && fabs(roll) > 150)    //判断是否处于水平状态
//				{
//					if(is_first) { last = yaw; is_first = 0; }
//					else
//					{
//						d_value = yaw - last;
//						last = yaw;
//						if(d_value > 180) d_value -= 360.0;
//						else if(d_value < -180) d_value += 360.0;
//						if(fabs(d_value) > 1.0)  // 会不会转的太慢了，导致10ms的间隔，连1度都转不了
//						{
//							sum += d_value;
//							switch(change_tend)
//							{
//								case 0:     //静止
//									if(sum > ANGLE_THRESHOLD) 
//									{ change_tend = 1;  right_span = ANGLE_THRESHOLD;}
//									else if(sum < -ANGLE_THRESHOLD) 
//									{ change_tend = -1; left_span = ANGLE_THRESHOLD; }
//									break;
//								case 1:    //右转
//									right_span = f_max(right_span, sum);
//									//printf("right span:         %f\r\n", right_span);
//									if(right_span > sum)     // 说明右转停止，进入左转状态
//									{
//										cnt++;
//										if(cnt == 2)
//										{
//											sum -= right_span; left_span = fabs(sum);
//											printf("sum:                %f\r\n", sum);
//											change_tend = -1;
//											cnt = 0;
//										}								
//									}
//									break;
//								case -1:   //左转    
//									left_span = f_max(left_span, fabs(sum));
//									//printf("left span:         %f\r\n", left_span);
//									if(left_span > fabs(sum)) // 说明左转停止，进入右转状态
//									{
//										cnt++;
//										if(cnt == 2)
//										{
//											sum = left_span - fabs(sum); right_span = fabs(sum);
//											change_tend = 1;
//											cnt = 0;
//										}
//									}
//									break;
//							}
//							zero_change_cnt = 0;
//						} 
//						else
//						{
//							zero_change_cnt++;
//							if (zero_change_cnt == 5)  // 如果连续5次都不变，则说明静止
//							{
//								//printf("right_span:                                 %f     %f\r\n", right_span, left_span);
//								if(fabs(right_span - left_span) < 10.0)
//								{
//									if(change_tend == 1) 
//										work_status = (work_status -1 + 3) % 3;
//									else if(change_tend == -1)
//										work_status = (work_status +1) % 3;
//								}
//								sum = 0;
//								right_span = 0; left_span = 0;
//								change_tend = 0;
//								zero_change_cnt = 0;
//							}
//						}
//					}				
//				}
//			}
//			step = 0;
//		}
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}
//}

