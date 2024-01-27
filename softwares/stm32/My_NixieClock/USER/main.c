#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer2.h"
#include "math.h"
#include "gxht30.h"
#include "hv57708.h"
#include "ws2812c.h"
#include "esp8266exit.h"
#include "ds1302.h"
#include "beep.h"
#include "key.h"
#include "Neon.h"

#include <stdlib.h>
#include <time.h>

#define RED 0x4200              //GRB
#define GREEN 0x420000
#define BLUE 0x42
#define D_GRAY 0x555555          // 暗灰色
#define D_BlUE 0x000033          // 深蓝色
#define D_PURPLE 0x330033        // 暗紫色
#define D_RED    0x003300        // 深红色
#define D_GREEN  0x330000        // 深绿色
#define D_ORANGE 0x114400        // 深橙色
#define D_YELLO  0x333300        // 深黄色
#define D_PINK   0x005533        // 暗粉色

u8 show_data[4];
u8 last_func = 0, func = 1, func2_switch_flag = 0, func2_switch_cnt = 0;
u16 temp, cnt_of_protection = 0, song_time_cnt = 0;
float temper, humid;


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	    //串口初始化 波特率为115200
	//usart3_init(115200);		//初始化串口3 
	delay_init();	            //延时函数初始化 
	Timer2_Init();              // 控制主循环每1s扫描一次，主要为了防止对传感器发起过快的读取，导致错乱
	ds1302_gpio_init();         //
	BEEP_GPIO_Init();        //占用定时器3    
	GXHT_Init();
	HV57708_Init(HV57708_MODE_INVERTION);
	WS2812C_Init();          //占用定时器4
	HV57708_Display_None(); 
	Key_Init();
	Neon_Bubble_Init();
	delay_ms(200);
	init_esp8266exit();
	ds1302_init();
	while(1)
	{
//		HV57708_Display(show_data, 0);
//		GXHT_Get_One_Value(&temper, &humid);
//		printf("temper: %f   humid: %f\r\n", temper, humid);	
//		WS2812C_SetBuf((u32)D_GRAY);
//		WS2812C_UpdateBuf();		
//		delay_s(5);
		
		
		
//		ds1302_read_realTime();    //读取此时时刻
//		show_data[0] = TimeData.hour / 10;  show_data[1] = TimeData.hour % 10;
//		show_data[2] = TimeData.minute / 10; show_data[3] = TimeData.minute % 10;
//		HV57708_Display(show_data, 0);
//		printf("时间:%d-%d-%d %d:%d:%d %d \r\n",TimeData.year,TimeData.month,TimeData.day,TimeData.hour,TimeData.minute,TimeData.second,TimeData.week);
//		delay_s(5);
		
		
		func = Key_GetNum();
		if(Timer2_1s_Flag || last_func != func)
		{
			ds1302_read_realTime();   //读取当前时刻
			switch (func)
			{
				case 1:                //显示时:分
					show_data[0] = TimeData.hour / 10;  show_data[1] = TimeData.hour % 10;
					show_data[2] = TimeData.minute / 10; show_data[3] = TimeData.minute % 10;
					
					WS2812C_SetBuf((u32)D_PINK);   // 设置氛围灯
					WS2812C_UpdateBuf();
					HV57708_Display(show_data, 0);
					NEON_BUBBLE_REVERSE();   // 翻转氖泡的开关状态
					break;
				case 2:               //显示年：月：日
					if(func2_switch_flag)  // 显示月：日
					{
						show_data[0] = TimeData.month / 10;  show_data[1] = TimeData.month % 10;
						show_data[2] = TimeData.day / 10; show_data[3] = TimeData.day % 10;
						NEON_BUBBLE_OPEN();
					} else                 // 显示年
					{
						show_data[0] = TimeData.year / 1000;  show_data[1] = TimeData.year % 1000 / 100;
						show_data[2] = TimeData.year % 100 / 10; show_data[3] = TimeData.year % 10;
						NEON_BUBBLE_CLOSE();
					}
					WS2812C_SetBuf((u32)D_PURPLE);
					WS2812C_UpdateBuf();
					HV57708_Display(show_data, 0);
					func2_switch_cnt++;
					if(func2_switch_cnt == 2){
						func2_switch_cnt = 0;
						func2_switch_flag = ~func2_switch_flag;
					}
					break;
				case 3:            //显示温度（前3位显示整数部分，后1位显示小数部分）
					NEON_BUBBLE_CLOSE();    // 关闭氖泡
					GXHT_Get_One_Value(&temper, &humid);
					if(temper < 0)     //小于0，氛围灯显示冷色调
					{
						WS2812C_SetBuf((u32)D_BlUE);
						WS2812C_UpdateBuf();
					}
					else               //大于0，氛围灯显示暖色调
					{
						WS2812C_SetBuf((u32)D_ORANGE);
						WS2812C_UpdateBuf();
					}
					temp = (u16)(fabs(temper) * 100);
					show_data[0] = temp / 1000;  show_data[1] = temp % 1000 / 100;
					show_data[2] = temp % 100 / 10; show_data[3] = temp % 10;	
					HV57708_Display(show_data, 1);
					break;
				case 4:
					NEON_BUBBLE_CLOSE();    // 关闭氖泡
					GXHT_Get_One_Value(&temper, &humid);
					WS2812C_SetBuf((u32)D_GREEN);
					WS2812C_UpdateBuf();					
				    temp = (u16)(fabs(humid) * 100);
					show_data[0] = temp / 1000;  show_data[1] = temp % 1000 / 100;
					show_data[2] = temp % 100 / 10; show_data[3] = temp % 10;	
					HV57708_Display(show_data, 1);
					break;
				case 5:
					if(play_song_flag == 0)    //歌曲开始
					{
						NEON_BUBBLE_OPEN();    
						WS2812C_SetBuf((u32)D_RED);
						WS2812C_UpdateBuf();
						song_time_cnt = 0;
						show_data[0] = 0; show_data[1] = 0; show_data[2] = 0; show_data[3] = 0;
						HV57708_Display(show_data, 0);
						play_song_flag = 1;
						play_song(2);
					} 
					else if(play_song_flag == 1)  //歌曲演奏中
					{
						song_time_cnt++;
						if(song_time_cnt < 60) 
							{show_data[0] = 0; show_data[1] = 0; show_data[2] = song_time_cnt / 10; show_data[3] = song_time_cnt % 10; }
						else 
						{  temp = song_time_cnt % 60;
						   show_data[2] = temp / 10; show_data[3] = temp % 10;
						   show_data[1] = song_time_cnt / 60;
						}
						HV57708_Display(show_data, 0);
					} else if(play_song_flag == 2){        //歌曲结束
						play_song_flag = 0;                //重新开始演奏
					} else {                               // 其他状态，生日歌用到，在此状态下什么都不做
						;
					}
					break;
			}
			Timer2_1s_Flag = 0;
			cnt_of_protection++;
			if(cnt_of_protection == 300)   //每5分钟进行一次阴极保护
			{
				HV57708_Protection();
				cnt_of_protection = 0;
			}
		}		
		if(last_func == 5 && func != 5 && play_song_flag != 3)
		{
			TIM_SetCompare3(TIM3, 0);
			TIM_Cmd(TIM3, DISABLE);
			play_song_flag = 0;
		}
		last_func = func;
		if(func != 2) 
		{
			func2_switch_flag = 0;  //处于其他功能时重置年：月日的切换标志
			func2_switch_cnt = 0;
		}
		
		//播放歌曲
		if(TimeData.month == 8 && TimeData.day == 9 && TimeData.hour == 0 && TimeData.minute == 0 && (TimeData.second == 0))
		{
			play_song_flag = 3;
			TIM_SetCompare3(TIM3, 0);
			TIM_Cmd(TIM3, DISABLE);
			play_song(1);
		}
		
		//更新时间		
		if(ESP8266_RX_FLAG)
		{
			convert_beijing_time((long long)epoch_time);
			ds1302_update_beijing_time(&beijing_time);
			ESP8266_RX_FLAG = 0;
			epoch_time = 0;
		}	
		
	} 	
}
 


