#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "parsejson.h"
#include "malloc.h"
#include "usart.h"
#include "string.h"
#include "delay.h"

typedef struct
{
	char m_CityName[10];
	char m_Text[10];
	char m_Temp[5];
	char m_Humi[5];
	char m_LastUpdataTime[30];
	char m_WindDir[10];
	char m_WindGrade[8];
	char m_WindSpeed[8];
}Now_Weather;



extern u8 flag;

extern int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

//数字字符串转换成整形
//static u8 str2int(u8 *str)
//{
//	u8 len,res;
//	len = strlen((const char *)str);
//	switch(len)
//	{
//		case 1:
//			res = str[0]-0x30;
//			break;
//		case 2:
//			res = (str[0]-0x30)*10+(str[1]-0x30);
//			break;
//		default:
//			break;
//	}
//	return res;
//}


//解析当前天气
void parse_now_weather(void)
{
	u16 i = 0,j = 0,line = 0;
	int len = 0;
	char gbkstr[50] = {0};
	Now_Weather *weather_Structure = mymalloc(SRAMIN,sizeof(Now_Weather));
	printf("jieshou->1dayjson = %s\r\n",USART3_RX_BUF);
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 'n' && USART3_RX_BUF[i+1] == 'a' && USART3_RX_BUF[i+2] == 'm' && USART3_RX_BUF[i+3] == 'e')
		{
			while(USART3_RX_BUF[i+7] != '"')
				weather_Structure->m_CityName[j++] = USART3_RX_BUF[7+(i++)];
			weather_Structure->m_CityName[j] = '\0';
			break;
		}
	}
	SwitchToGbk((const u8*)weather_Structure->m_CityName,strlen(weather_Structure->m_CityName),(u8 *)gbkstr,&len);  //获取城市名称转换为gbk文件
	if(strstr(gbkstr,"衡阳") != NULL)
		LCD_ShowString(0,20,sizeof("hengyang")*8,16,16,"hengyang");
	else if(strstr(gbkstr,"长沙") != NULL)  
		LCD_ShowString(0,140,sizeof("changsha")*8,16,16,"changsha");
	
	j = 0;
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 't' && USART3_RX_BUF[i+1] == 'e' && USART3_RX_BUF[i+2] == 'x' && USART3_RX_BUF[i+3] == 't')
		{
			while(USART3_RX_BUF[i+7] != '"')
				weather_Structure->m_Text[j++] = USART3_RX_BUF[7+(i++)];
			weather_Structure->m_Text[j] = '\0';
			break;
		}
	}
	memset(gbkstr,0,sizeof(gbkstr));
	if(flag == 1) line = 160;//长沙
	else if(flag == 2) line = 40;//衡阳
	SwitchToGbk((const u8*)weather_Structure->m_Text,strlen(weather_Structure->m_Text),(u8 *)gbkstr,&len);
	if(strstr(gbkstr,"阴") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"overcast      ");
	else if(strstr(gbkstr,"多云") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"cloudy        ");
	else if(strstr(gbkstr,"晴") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"fine day      ");
	else if(strstr(gbkstr,"小雨") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"light rain    ");
	else if(strstr(gbkstr,"中雨") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"moderate rain ");
	else if(strstr(gbkstr,"大雨") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"heavy rain    ");
	else if(strstr(gbkstr,"雷阵雨") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"thunder shower");
	else if(strstr(gbkstr,"阵雨") != NULL)
		LCD_ShowString(88,line,sizeof("overcast      ")*8,16,16,"shower        ");

	
	j = 0;
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 't' && USART3_RX_BUF[i+1] == 'e' && USART3_RX_BUF[i+2] == 'm' && USART3_RX_BUF[i+3] == 'p')
		{
			while(USART3_RX_BUF[i+14] != '"')
				weather_Structure->m_Temp[j++] = USART3_RX_BUF[14+(i++)];
			weather_Structure->m_Temp[j] = '\0';
			break;
		}
	}
	
	if(flag == 1) line = 220;//长沙
	else if(flag == 2) line = 100;//衡阳
	LCD_ShowString(100,line,25,20,16,(u8*)weather_Structure->m_Temp);	
	printf("wendu = %s\r\n",weather_Structure->m_Temp);
	
	j = 0;
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 'l' && USART3_RX_BUF[i+1] == 'a' && USART3_RX_BUF[i+2] == 's' && USART3_RX_BUF[i+3] == 't')
		{
			while(USART3_RX_BUF[i+14] != '"')
				weather_Structure->m_LastUpdataTime[j++] = USART3_RX_BUF[14+(i++)];
			weather_Structure->m_LastUpdataTime[j] = '\0';
			break;
		}
	}
	if(flag == 1) line = 140;
	else if(flag == 2)line = 20;
	LCD_ShowString(210,line,200,20,16,(u8*)weather_Structure->m_LastUpdataTime);
	printf("1day_updata_time = %s\r\n",(u8*)weather_Structure->m_LastUpdataTime);
	
	myfree(SRAMIN,weather_Structure);
}



//解析3天天气
void parse_3days_weather(void)
{
	u16 i = 0,j = 0,line = 0;
	int len = 0;
	char gbkstr[50] = {0};
	Now_Weather *weather_Structure = mymalloc(SRAMIN,sizeof(Now_Weather));
	printf("jieshou->1dayjson = %s\r\n",USART3_RX_BUF);
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 'w' && USART3_RX_BUF[i+1] == 'i' && USART3_RX_BUF[i+2] == 'n' && USART3_RX_BUF[i+3] == 'd')
		{
			while(USART3_RX_BUF[i+17] != '"')
				weather_Structure->m_WindDir[j++] = USART3_RX_BUF[17+(i++)];
			weather_Structure->m_WindDir[j] = '\0';
			break;
		}
	}
	SwitchToGbk((const u8*)weather_Structure->m_WindDir,strlen(weather_Structure->m_WindDir),(u8 *)gbkstr,&len);  //获取城市名称转换为gbk文件
	if(flag == 1) line = 200;//长沙
	else if(flag == 2) line = 80;//衡阳
	LCD_Fill(220,45,320,61,BLACK);
	if(strstr(gbkstr,"北") != NULL)           
	{
		if(strstr(gbkstr,"西") != NULL)       LCD_ShowString(0,line,sizeof("southerly        ")*8,16,16,"Northwest wind   ");
		else if(strstr(gbkstr,"东") != NULL)  LCD_ShowString(0,line,sizeof("southerly        ")*8,16,16,"Northeastern wind");									  
		else LCD_ShowString(0,line,sizeof("southerly        ")*8,16,16,"northerly        ");
		   
	}
	else if(strstr(gbkstr,"南") != NULL)  
	{
		if(strstr(gbkstr,"西") != NULL)      LCD_ShowString(0,line,sizeof("southerly        ")*8,16,16,"Southwest wind   ");
		else if(strstr(gbkstr,"东") != NULL) LCD_ShowString(0,line,sizeof("southerly        ")*8,16,16,"Southeast wind   ");
		else LCD_ShowString(0,line,sizeof("southerly        ")*8,16,16,"southerly        ");
	}

	j = 0;
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 's' && USART3_RX_BUF[i+1] == 'p' && USART3_RX_BUF[i+2] == 'e' && USART3_RX_BUF[i+3] == 'e')
		{
			while(USART3_RX_BUF[i+8] != '"')
				weather_Structure->m_WindSpeed[j++] = USART3_RX_BUF[8+(i++)];
			weather_Structure->m_WindSpeed[j] = '\0';
			break;
		}
	}
	LCD_ShowString(200,line,sizeof(weather_Structure->m_WindSpeed)*8,16,16,(u8 *)weather_Structure->m_WindSpeed);   //显示风速km/h
	
	j = 0;
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 's' && USART3_RX_BUF[i+1] == 'c' && USART3_RX_BUF[i+2] == 'a' && USART3_RX_BUF[i+3] == 'l')
		{
			while(USART3_RX_BUF[i+8] != '"')
				weather_Structure->m_WindGrade[j++] = USART3_RX_BUF[8+(i++)];
			weather_Structure->m_WindGrade[j] = '\0';
			break;
		}
	}
	LCD_ShowString(160,line,sizeof(weather_Structure->m_WindGrade)*8,16,16,(u8 *)weather_Structure->m_WindGrade);
	
	
	j = 0;
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 'h' && USART3_RX_BUF[i+1] == 'u' && USART3_RX_BUF[i+2] == 'm' && USART3_RX_BUF[i+3] == 'i')
		{
			while(USART3_RX_BUF[i+11] != '"')
				weather_Structure->m_Humi[j++] = USART3_RX_BUF[11+(i++)];
			weather_Structure->m_Humi[j] = '\0';
			break;
		}
	}
	if(flag == 1) line = 220;//长沙
	else if(flag == 2) line = 100;//衡阳
	LCD_ShowString(160,line,sizeof(weather_Structure->m_Humi)*8,16,16,(u8 *)weather_Structure->m_Humi);
	
	
	j = 0;
	for(i = 0; i < strlen((char *)USART3_RX_BUF); i++)
	{
		if(USART3_RX_BUF[i] == 'l' && USART3_RX_BUF[i+1] == 'a' && USART3_RX_BUF[i+2] == 's' && USART3_RX_BUF[i+3] == 't')
		{
			while(USART3_RX_BUF[i+14] != '"')
				weather_Structure->m_LastUpdataTime[j++] = USART3_RX_BUF[14+(i++)];
			weather_Structure->m_LastUpdataTime[j] = '\0';
			break;
		}
	}
	
	LCD_ShowString(0,300,200,20,12,(u8*)weather_Structure->m_LastUpdataTime);
	printf("1day_updata_time = %s\r\n",(u8*)weather_Structure->m_LastUpdataTime);
	
	myfree(SRAMIN,weather_Structure);
}



