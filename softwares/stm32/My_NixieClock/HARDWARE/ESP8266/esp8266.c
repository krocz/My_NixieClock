#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include "usart.h"
#include "stdlib.h"
#include "string.h"
#include "malloc.h"
#include <time.h>


extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

//�û�������
//ʱ��˿ں�
#define TIME_PORTNUM			"80"
//ʱ�������IP
#define TIME_SERVERIP			"cgi.im.qq.com"  
TIMEData beijing_time;
u8 flag = 0;

//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
const u8* wifista_ssid = "418_2";			//·����SSID��
const u8* wifista_encryption = "WPA_WAP2_PSK";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifista_password = "418418418"; 	//��������
//5�ּ��ܷ�ʽ
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN", "WEP", "WPA_PSK", "WPA2_PSK", "WPA_WAP2_PSK"};




//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	USART3_RX_BUF[USART3_RX_STA]=0;//��ӽ�����
	//printf("%s",USART3_RX_BUF);	//���͵�����
	if(mode)USART3_RX_STA=0;
	 
}
//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* atk_8266_check_cmd(u8 *str)
{
	char *strx=0;
	USART3_RX_BUF[USART3_RX_STA]=0;//��ӽ�����
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
//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	char cmd1[50] = {0};
	USART3_RX_STA=0;
	sprintf(cmd1,"%s\r\n",cmd);
	u3_printf(cmd1);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
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
//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
u8 atk_8266_send_data(u8 *data, u8 *ack, u16 waittime)
{
	u8 res=0; 
	char cmd1[50] = {0};
	USART3_RX_STA=0;
	
	sprintf(cmd1,"%s",data);
	u3_printf(cmd1);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
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
//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(500);					//�ȴ�500ms
	return atk_8266_send_cmd("AT","OK",20);//�˳�͸���ж�.
}

//��ȡATK-ESP8266ģ�������״̬
//����ֵ:0,δ����;1,���ӳɹ�.
u8 atk_8266_consta_check(void)
{
	u8 *p = mymalloc(SRAMIN,sizeof(u8));
	u8 res;
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//�õ�����״̬	
	myfree(SRAMIN,p);
	return res;
}

//��ȡClient ip��ַ
//ipbuf:ip��ַ���������
void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p = mymalloc(SRAMIN,sizeof(u8));
	u8 *p1 = mymalloc(SRAMIN,sizeof(u8));
	if(atk_8266_send_cmd("AT+CIFSR","OK",50))//��ȡWAN IP��ַʧ��
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




//����ESP8266λstaģʽ�������ӵ�·����
u8 atk_8266_wifista_config(void)
{
	u8 p[40] = {0};							//����32�ֽ��ڴ棬���ڴ�wifista_ssid��wifista_password
//	Show_Str_Mid(0,30,"ATK-ESP8266 WIFIģ�����",16,240); 
	while(atk_8266_send_cmd("AT","OK",20))//���WIFIģ���Ƿ�����
	{
		atk_8266_quit_trans();//�˳�͸��
		atk_8266_send_cmd("AT+CIPMODE=0", "OK",200);  //�ر�͸��ģʽ	
//		Show_Str(40,55,200,16,"δ��⵽ģ��!!!",16,0);
		delay_ms(800);
//		LCD_Fill(40,55,200,55+16,WHITE);
//		Show_Str(40,55,200,16,"��������ģ��...",16,0); 
	} 
	while(atk_8266_send_cmd("ATE0","OK",20));//�ر� ��������
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	//printf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
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

//��ȡ����ʱ��
u8 get_beijing_time(void)
{
	u8 *p;
	u8 res;
	
	u8 *resp;
	u8 *p_end;
	u8 month_str[4], week_str[4];
//	u8 ipbuf[16]; 	//IP����
	p=mymalloc(SRAMIN,40);							//����40�ֽ��ڴ�
	resp=mymalloc(SRAMIN,10);
	p_end=mymalloc(SRAMIN,40);
	
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",TIME_SERVERIP,TIME_PORTNUM);    //����Ŀ��TCP������
	res = atk_8266_send_cmd(p,"OK",200);//���ӵ�Ŀ��TCP������
	if(res==1)
	{
		myfree(SRAMIN,p);
		return 1;
	}
	delay_ms(300);
	atk_8266_send_cmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��	

	//printf("�豸 %s\r\n",p);
	
	CLR_BUF();
	atk_8266_send_cmd("AT+CIPSEND","OK",100);         //��ʼ͸��
	//printf("start trans...\r\n");

	u3_printf("GET http://cgi.im.qq.com\n\n");
	delay_ms(20);
	CLR_BUF();	
	delay_ms(1000);

	if(USART3_RX_STA != 0)
    {
		strncpy((char *)resp,"Date",5);
        USART3_RX_BUF[USART3_RX_STA] = 0;
		//printf("get_tim_srt��%s\r\n",USART3_RX_BUF);
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
	atk_8266_quit_trans();//�˳�͸��
	atk_8266_send_cmd("AT+CIPCLOSE","OK",50);         //�ر�����
	myfree(SRAMIN,p);
	myfree(SRAMIN,resp);
	myfree(SRAMIN,p_end);
	return 0;
}


