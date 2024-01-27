#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include "usart.h"
#include "stdlib.h"
#include "string.h"
#include "malloc.h"
#include <time.h>


extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.

//用户配置区
//时间端口号
#define TIME_PORTNUM			"80"
//时间服务器IP
#define TIME_SERVERIP			"cgi.im.qq.com"  
TIMEData beijing_time;
u8 flag = 0;

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid = "418_2";			//路由器SSID号
const u8* wifista_encryption = "WPA_WAP2_PSK";	//wpa/wpa2 aes加密方式
const u8* wifista_password = "418418418"; 	//连接密码
//5种加密方式
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN", "WEP", "WPA_PSK", "WPA2_PSK", "WPA_WAP2_PSK"};




//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	USART3_RX_BUF[USART3_RX_STA]=0;//添加结束符
	//printf("%s",USART3_RX_BUF);	//发送到串口
	if(mode)USART3_RX_STA=0;
	 
}
//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
	char *strx=0;
	USART3_RX_BUF[USART3_RX_STA]=0;//添加结束符
	strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	return (u8*)strx;
}

u8 Find(u8 *ack)
{
	if(ack != NULL)
	{
		if(strstr((char *)USART3_RX_BUF,(char *)ack) != NULL)
		{
//			CLR_BUF();
			return 0;
		}
	}
	//CLR_BUF();
	return 1;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	char cmd1[50] = {0};
	USART3_RX_STA=0;
	sprintf(cmd1,"%s\r\n",cmd);
	u3_printf(cmd1);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			
			if(!Find(ack))
			{
				//printf("%s  ack:%s\r\n",cmd,ack);
				return 0;
			}
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data, u8 *ack, u16 waittime)
{
	u8 res=0; 
	char cmd1[50] = {0};
	USART3_RX_STA=0;
	
	sprintf(cmd1,"%s",data);
	u3_printf(cmd1);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(!Find(ack))
			{
				//printf("%s  ack:%s\r\n",data,ack);
				return 0;
			}
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(500);					//等待500ms
	return atk_8266_send_cmd("AT","OK",20);//退出透传判断.
}

//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_8266_consta_check(void)
{
	u8 *p = mymalloc(SRAMIN,sizeof(u8));
	u8 res;
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//得到连接状态	
	myfree(SRAMIN,p);
	return res;
}

//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p = mymalloc(SRAMIN,sizeof(u8));
	u8 *p1 = mymalloc(SRAMIN,sizeof(u8));
	if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
	{
		ipbuf[0]=0;
		return;
	}		
	p=atk_8266_check_cmd("\"");
	p1=(u8*)strstr((const char*)(p+1),"\"");
	sprintf((char*)ipbuf,"%s",p+1);	
	myfree(SRAMIN,p);
	myfree(SRAMIN,p1);
}




//配置ESP8266位sta模式，并连接到路由器
u8 atk_8266_wifista_config(void)
{
	u8 p[40] = {0};							//申请32字节内存，用于存wifista_ssid，wifista_password
//	Show_Str_Mid(0,30,"ATK-ESP8266 WIFI模块测试",16,240); 
	while(atk_8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
	{
		atk_8266_quit_trans();//退出透传
		atk_8266_send_cmd("AT+CIPMODE=0", "OK",200);  //关闭透传模式	
//		Show_Str(40,55,200,16,"未检测到模块!!!",16,0);
		delay_ms(800);
//		LCD_Fill(40,55,200,55+16,WHITE);
//		Show_Str(40,55,200,16,"尝试连接模块...",16,0); 
	} 
	while(atk_8266_send_cmd("ATE0","OK",20));//关闭 回显命令
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	//printf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
	return 0;
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
	beijing_time.second = p->tm_sec;
	beijing_time.week = (p->tm_wday)? p->tm_wday : 7;
	beijing_time.year = 1900 + p->tm_year;
}

//获取北京时间
u8 get_beijing_time(void)
{
	u8 *p;
	u8 res;
	
	u8 *resp;
	u8 *p_end;
	u8 month_str[4], week_str[4];
//	u8 ipbuf[16]; 	//IP缓存
	p=mymalloc(SRAMIN,40);							//申请40字节内存
	resp=mymalloc(SRAMIN,10);
	p_end=mymalloc(SRAMIN,40);
	
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",TIME_SERVERIP,TIME_PORTNUM);    //配置目标TCP服务器
	res = atk_8266_send_cmd(p,"OK",200);//连接到目标TCP服务器
	if(res==1)
	{
		myfree(SRAMIN,p);
		return 1;
	}
	delay_ms(300);
	atk_8266_send_cmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	

	//printf("设备 %s\r\n",p);
	
	CLR_BUF();
	atk_8266_send_cmd("AT+CIPSEND","OK",100);         //开始透传
	//printf("start trans...\r\n");

	u3_printf("GET http://cgi.im.qq.com\n\n");
	delay_ms(20);
	CLR_BUF();	
	delay_ms(1000);

	if(USART3_RX_STA != 0)
    {
		strncpy((char *)resp,"Date",5);
        USART3_RX_BUF[USART3_RX_STA] = 0;
		//printf("get_tim_srt：%s\r\n",USART3_RX_BUF);
        if(strstr((char*)USART3_RX_BUF,(char*)resp)) 
        {       
			strncpy((char *)resp,"GMT",4);
            p_end = (u8*)strstr((char*)USART3_RX_BUF,(char*)resp);
			p = p_end - 9; 
			//printf("get_net_str %s\r\n",p);
			beijing_time.hour = ((*p - 0x30)*10 + (*(p+1) - 0x30) + 8) % 24;  //GMT0-->GMT8

			beijing_time.minute = ((*(p+3) - 0x30)*10 + (*(p+4) - 0x30)) % 60;

			beijing_time.second = ((*(p+6) - 0x30)*10 + (*(p+7) - 0x30)) % 60;

			beijing_time.year = ((*(p-5) - 0x30)*1000 + (*(p-4) - 0x30)*100+ (*(p-3) - 0x30)*10+ (*(p-2) - 0x30)); 
			
			beijing_time.day = ((*(p-12) - 0x30)*10 + (*(p-11) - 0x30)); 
			
			strncpy((char *)month_str, (char *)(p-9), 3);   month_str[3] = '\0';
			strncpy((char *)week_str, (char *)(p-17), 3);   week_str[3] = '\0';
			
			if        (!strncmp((char*)month_str, (char*)"Jan", 3)) beijing_time.month=1; 
			else if   (!strncmp((char*)month_str, (char*)"Feb", 3)) beijing_time.month=2; 
			else if   (!strncmp((char*)month_str, (char*)"Mar", 3)) beijing_time.month=3; 
			else if   (!strncmp((char*)month_str, (char*)"Apr", 3)) beijing_time.month=4; 
			else if   (!strncmp((char*)month_str, (char*)"May", 3)) beijing_time.month=5; 
			else if   (!strncmp((char*)month_str, (char*)"Jun", 3)) beijing_time.month=6; 
			else if   (!strncmp((char*)month_str, (char*)"Jul", 3)) beijing_time.month=7; 
			else if   (!strncmp((char*)month_str, (char*)"Aug", 3)) beijing_time.month=8; 
			else if   (!strncmp((char*)month_str, (char*)"Sep", 3)) beijing_time.month=9; 
			else if   (!strncmp((char*)month_str, (char*)"Oct", 3)) beijing_time.month=10; 
			else if   (!strncmp((char*)month_str, (char*)"Nov", 3)) beijing_time.month=11; 
			else if   (!strncmp((char*)month_str, (char*)"Dec", 3)) beijing_time.month=12;
			
			if        (!strncmp((char*)week_str, (char*)"Mon", 3))  beijing_time.week=1;
			else if   (!strncmp((char*)week_str, (char*)"Tue", 3))  beijing_time.week=2;
			else if   (!strncmp((char*)week_str, (char*)"Wed", 3))  beijing_time.week=3;
			else if   (!strncmp((char*)week_str, (char*)"Thu", 3))  beijing_time.week=4;
			else if   (!strncmp((char*)week_str, (char*)"Fri", 3))  beijing_time.week=5;
			else if   (!strncmp((char*)week_str, (char*)"Sat", 3))  beijing_time.week=6;
			else if   (!strncmp((char*)week_str, (char*)"Sun", 3))  beijing_time.week=7;
			
 
			CLR_BUF();
												 
			printf("get time----year:%d  month:%d  date:%d  hour:%d  min:%d  sec:%d  week:%d\r\n", 
				beijing_time.year, beijing_time.month, beijing_time.day, beijing_time.hour, beijing_time.minute, beijing_time.second, beijing_time.week);														
	    }
		CLR_BUF();														
    }               
	atk_8266_quit_trans();//退出透传
	atk_8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
	myfree(SRAMIN,p);
	myfree(SRAMIN,resp);
	myfree(SRAMIN,p_end);
	return 0;
}


