#ifndef __ds1302_H 
#define __ds1302_H  
#include "sys.h" 
 
typedef struct 
{
	u16 year;
	u8  month;
	u8  day;
	u8  hour;
	u8  minute;
	u8  second;
	u8  week;
}TIMEData;//创建TIMEData结构体方便存储时间日期数据



extern TIMEData TimeData;//全局变量
void ds1302_gpio_init(void);//ds1302端口初始化
void ds1302_write_onebyte(u8 data);//向ds1302发送一字节数据
void ds1302_wirte_rig(u8 address,u8 data);//向指定寄存器写一字节数据
u8 ds1302_read_rig(u8 address);//从指定寄存器读一字节数据
void ds1302_init(void);//ds1302初始化函数
void ds1302_read_time(void);//从ds1302读取实时时间（BCD码）
void ds1302_read_realTime(void);//将BCD码转化为十进制数据
void ds1302_update_beijing_time(TIMEData* beijing_time);
#endif
