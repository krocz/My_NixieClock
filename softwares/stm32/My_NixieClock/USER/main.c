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
#define D_GRAY 0x555555          // ����ɫ
#define D_BlUE 0x000033          // ����ɫ
#define D_PURPLE 0x330033        // ����ɫ
#define D_RED    0x003300        // ���ɫ
#define D_GREEN  0x330000        // ����ɫ
#define D_ORANGE 0x114400        // ���ɫ
#define D_YELLO  0x333300        // ���ɫ
#define D_PINK   0x005533        // ����ɫ

u8 show_data[4];
u8 last_func = 0, func = 1, func2_switch_flag = 0, func2_switch_cnt = 0;
u16 temp, cnt_of_protection = 0, song_time_cnt = 0;
float temper, humid;


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	    //���ڳ�ʼ�� ������Ϊ115200
	//usart3_init(115200);		//��ʼ������3 
	delay_init();	            //��ʱ������ʼ�� 
	Timer2_Init();              // ������ѭ��ÿ1sɨ��һ�Σ���ҪΪ�˷�ֹ�Դ������������Ķ�ȡ�����´���
	ds1302_gpio_init();         //
	BEEP_GPIO_Init();        //ռ�ö�ʱ��3    
	GXHT_Init();
	HV57708_Init(HV57708_MODE_INVERTION);
	WS2812C_Init();          //ռ�ö�ʱ��4
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
		
		
		
//		ds1302_read_realTime();    //��ȡ��ʱʱ��
//		show_data[0] = TimeData.hour / 10;  show_data[1] = TimeData.hour % 10;
//		show_data[2] = TimeData.minute / 10; show_data[3] = TimeData.minute % 10;
//		HV57708_Display(show_data, 0);
//		printf("ʱ��:%d-%d-%d %d:%d:%d %d \r\n",TimeData.year,TimeData.month,TimeData.day,TimeData.hour,TimeData.minute,TimeData.second,TimeData.week);
//		delay_s(5);
		
		
		func = Key_GetNum();
		if(Timer2_1s_Flag || last_func != func)
		{
			ds1302_read_realTime();   //��ȡ��ǰʱ��
			switch (func)
			{
				case 1:                //��ʾʱ:��
					show_data[0] = TimeData.hour / 10;  show_data[1] = TimeData.hour % 10;
					show_data[2] = TimeData.minute / 10; show_data[3] = TimeData.minute % 10;
					
					WS2812C_SetBuf((u32)D_PINK);   // ���÷�Χ��
					WS2812C_UpdateBuf();
					HV57708_Display(show_data, 0);
					NEON_BUBBLE_REVERSE();   // ��ת���ݵĿ���״̬
					break;
				case 2:               //��ʾ�꣺�£���
					if(func2_switch_flag)  // ��ʾ�£���
					{
						show_data[0] = TimeData.month / 10;  show_data[1] = TimeData.month % 10;
						show_data[2] = TimeData.day / 10; show_data[3] = TimeData.day % 10;
						NEON_BUBBLE_OPEN();
					} else                 // ��ʾ��
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
				case 3:            //��ʾ�¶ȣ�ǰ3λ��ʾ�������֣���1λ��ʾС�����֣�
					NEON_BUBBLE_CLOSE();    // �ر�����
					GXHT_Get_One_Value(&temper, &humid);
					if(temper < 0)     //С��0����Χ����ʾ��ɫ��
					{
						WS2812C_SetBuf((u32)D_BlUE);
						WS2812C_UpdateBuf();
					}
					else               //����0����Χ����ʾůɫ��
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
					NEON_BUBBLE_CLOSE();    // �ر�����
					GXHT_Get_One_Value(&temper, &humid);
					WS2812C_SetBuf((u32)D_GREEN);
					WS2812C_UpdateBuf();					
				    temp = (u16)(fabs(humid) * 100);
					show_data[0] = temp / 1000;  show_data[1] = temp % 1000 / 100;
					show_data[2] = temp % 100 / 10; show_data[3] = temp % 10;	
					HV57708_Display(show_data, 1);
					break;
				case 5:
					if(play_song_flag == 0)    //������ʼ
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
					else if(play_song_flag == 1)  //����������
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
					} else if(play_song_flag == 2){        //��������
						play_song_flag = 0;                //���¿�ʼ����
					} else {                               // ����״̬�����ո��õ����ڴ�״̬��ʲô������
						;
					}
					break;
			}
			Timer2_1s_Flag = 0;
			cnt_of_protection++;
			if(cnt_of_protection == 300)   //ÿ5���ӽ���һ����������
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
			func2_switch_flag = 0;  //������������ʱ�����꣺���յ��л���־
			func2_switch_cnt = 0;
		}
		
		//���Ÿ���
		if(TimeData.month == 8 && TimeData.day == 9 && TimeData.hour == 0 && TimeData.minute == 0 && (TimeData.second == 0))
		{
			play_song_flag = 3;
			TIM_SetCompare3(TIM3, 0);
			TIM_Cmd(TIM3, DISABLE);
			play_song(1);
		}
		
		//����ʱ��		
		if(ESP8266_RX_FLAG)
		{
			convert_beijing_time((long long)epoch_time);
			ds1302_update_beijing_time(&beijing_time);
			ESP8266_RX_FLAG = 0;
			epoch_time = 0;
		}	
		
	} 	
}
 


