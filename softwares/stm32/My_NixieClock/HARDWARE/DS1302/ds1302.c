#include "ds1302.h"
#include "sys.h"
#include "delay.h"
 
TIMEData TimeData;
u8 read_time[7];

/**
PB12 <-->  CE
PB13 <-->  SCLK
PB14 <-->  SDA
*/
void ds1302_gpio_init()//CE,SCLK端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS1302_CE_PIN | DS1302_SCL_PIN | DS1302_SDA_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(DS1302_GPIO_PORT, &GPIO_InitStructure);
	
	DS1302_CE = 0; DS1302_SCL = 0;
}
 
void ds1302_write_onebyte(u8 data)//向DS1302发送一字节数据
{
	u8 count = 0;
	DS1302_SDA_OUT();
	DS1302_SCL = 0;
	for(count = 0; count < 8; count++)
	{	
		DS1302_SCL = 0;
		if(data & 0x01)
			{DS1302_WRITE_SDA = 1;}
		else
			{DS1302_WRITE_SDA = 0;}         //先准备好数据再发送
		DS1302_SCL = 1;               //拉高时钟线，发送数据
		data >>= 1;
	}
}
 
void ds1302_wirte_rig(u8 address,u8 data)//向指定寄存器地址发送数据
{
	u8 temp1 = address;
	u8 temp2 = data;
	DS1302_CE = 0;  DS1302_SCL = 0;
	delay_us(1);
	DS1302_CE = 1; 
	delay_us(2);
	ds1302_write_onebyte(temp1);
	ds1302_write_onebyte(temp2);
	DS1302_CE = 0;  DS1302_SCL = 0;
	delay_us(2);
}
 
u8 ds1302_read_rig(u8 address)//从指定地址读取一字节数据
{
	u8 temp3 = address;
	u8 count = 0;
	u8 return_data = 0x00;
	DS1302_CE = 0;  DS1302_SCL = 0;
    delay_us(3);
	DS1302_CE = 1;
	delay_us(3);
	ds1302_write_onebyte(temp3);
	
	
	DS1302_SDA_IN();   //配置I/O口为输入
	delay_us(2);
	for(count = 0; count < 8; count++)
	{
		delay_us(2);//使电平持续一段时间
		return_data >>= 1;
		DS1302_SCL = 1;
		delay_us(4);//使高电平持续一段时间
		DS1302_SCL = 0; 
		delay_us(14);//延时14us后再去读取电压，更加准确
		if(DS1302_READ_SDA)
			{return_data |= 0x80;}
	}
	delay_us(2);
	DS1302_CE = 0;
	return return_data;
}
 
void ds1302_init()
{
	ds1302_wirte_rig(0x8e, 0x00);//关闭写保护
//	ds1302_wirte_rig(0x80, 0x37);//seconds37秒
//	ds1302_wirte_rig(0x82, 0x58);//minutes58分
//	ds1302_wirte_rig(0x84, 0x23);//hours23时
//	ds1302_wirte_rig(0x86, 0x8);//date30日
//	ds1302_wirte_rig(0x88, 0x08);//months9月
//	ds1302_wirte_rig(0x8a, 0x07);//days星期日
//	ds1302_wirte_rig(0x8c, 0x20);//year2020年
	ds1302_wirte_rig(0x8e, 0x80);//打开写保护
}
 
void ds1302_read_time()
{
	read_time[0] = ds1302_read_rig(0x81);//读秒
	read_time[1] = ds1302_read_rig(0x83);//读分
	read_time[2] = ds1302_read_rig(0x85);//读时
	read_time[3] = ds1302_read_rig(0x87);//读日
	read_time[4] = ds1302_read_rig(0x89);//读月
	read_time[5] = ds1302_read_rig(0x8B);//读星期
	read_time[6] = ds1302_read_rig(0x8D);//读年
}
 
void ds1302_read_realTime()
{
	ds1302_read_time();  //BCD码转换为10进制
	TimeData.second = (read_time[0]>>4)*10 + (read_time[0]&0x0f);
	TimeData.minute = ((read_time[1]>>4)&(0x07))*10 + (read_time[1]&0x0f);
	TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
	TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
	TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
	TimeData.week=read_time[5];
	TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;
}

void ds1302_update_beijing_time(TIMEData* t)
{
	ds1302_wirte_rig(0x8e, 0x00);//关闭写保护
	ds1302_wirte_rig(0x80, (t->second)/10*16 + (t->second)%10);//seconds37秒
	ds1302_wirte_rig(0x82, (t->minute)/10*16 + (t->minute)%10);//minutes58分
	ds1302_wirte_rig(0x84, (t->hour)/10*16 + (t->hour)%10);//hours23时
	ds1302_wirte_rig(0x86, (t->day)/10*16 + (t->day)%10);//date30日
	ds1302_wirte_rig(0x88, (t->month)/10*16 + (t->month)%10);//months9月
	ds1302_wirte_rig(0x8a, t->week);//days星期日
	ds1302_wirte_rig(0x8c, (t->year-2000)/10*16 + (t->year-2000)%10);//year2020年
	ds1302_wirte_rig(0x8e, 0x80);//关闭写保护
}
