#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>      
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <time.h>
#include <cstdio>

// Rxd   GPIO3   SDA     PB11
// Txd   GPIO1   SCL     PB10
#define EXIT_SCL 1
#define EXIT_SDA 3


void exit_send_epochTime(unsigned long epochTime)
{
  uint8_t temp;
  for(int i = 31; i >= 0; i--)
  {
      temp = (epochTime >> i) & 0x01;
      exit_send_bit(temp);
  }  
}


void exit_send_word(uint16_t send_word)
{
  uint8_t temp;
  for(int i = 15; i >= 0; i--)
  {
      temp = (send_word >> i) & 0x01;
      exit_send_bit(temp);
  }
}

void exit_send_bit(uint8_t send_bit)
{
  digitalWrite(EXIT_SDA, send_bit);
  delay(1);
  digitalWrite(EXIT_SCL, LOW);
  delay(1);
  digitalWrite(EXIT_SCL, HIGH);
}

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com",60*60*8, 60*1000);
unsigned long epochTime;
bool is_first = false;

void setup()
{
  WiFiManager wifiManager;
  //wifiManager.resetSettings();    // 清除ESP8266所存储的WiFi连接信息
  
  delay(10);
  pinMode(EXIT_SCL, FUNCTION_0);
  pinMode(EXIT_SDA, FUNCTION_0);
  pinMode(EXIT_SCL, OUTPUT);
  pinMode(EXIT_SDA, OUTPUT);
  digitalWrite(EXIT_SCL, HIGH);
  digitalWrite(EXIT_SDA, HIGH);

  // 自动连接WiFi。以下语句的参数是连接ESP8266时的WiFi名称
  wifiManager.autoConnect("AutoConnectAP");  
  timeClient.begin();
  delay(1000);

}

void loop() 
{
  while(!timeClient.isTimeSet())
  {
    timeClient.update();
    delay(1000); 
  }

  epochTime = timeClient.getEpochTime();

  exit_send_word(0xfffe);
  exit_send_epochTime(epochTime);
  exit_send_word(0xfeff);

  if(!is_first)
  {
    for(int i = 0; i < 3; i++)
      delay(1000);
    is_first = true;
  } else
  {
    for(int i = 0; i < 600; i++)  // 10分钟
      delay(1000);
  }
}
















  // digitalWrite(I2C_SDA, HIGH);
  // digitalWrite(I2C_SCL, HIGH);
  // delay(1000);
  // digitalWrite(I2C_SDA, LOW);
  // digitalWrite(I2C_SCL, LOW);
  // delay(1000);  



  // i2c_start();
  // i2c_send_byte(0x42);
  // i2c_wait_ack();
  // i2c_send_byte(0xfe);
  // i2c_wait_ack();
  // i2c_send_byte(0xff);
  // i2c_wait_ack();
  // for(int i = 0; strTime[i]; i++)
  // {
  //   i2c_send_byte((uint8_t)strTime[i]);
  //   i2c_wait_ack();
  // }
  // i2c_send_byte(0xff);
  // i2c_wait_ack();
  // i2c_send_byte(0xfe);
  // i2c_wait_ack();
  // i2c_stop();


// #define I2C_SDA  3
// #define I2C_SCL  1

// void i2c_delay(void)
// {
// 	delayMicroseconds(3);
// }
// //产生IIC起始信号
// void i2c_start(void)
// {
//    pinMode(I2C_SDA, OUTPUT);
//    digitalWrite(I2C_SDA, HIGH);
//    digitalWrite(I2C_SCL, HIGH);
//    i2c_delay();
//    digitalWrite(I2C_SDA, LOW);
//    i2c_delay();
//    digitalWrite(I2C_SCL, LOW);
// }
// //产生IIC停止信号
// void i2c_stop(void)
// {
//    pinMode(I2C_SDA, OUTPUT);
//    digitalWrite(I2C_SCL, LOW);
//    digitalWrite(I2C_SDA, LOW);
//    i2c_delay();
//    digitalWrite(I2C_SCL, HIGH);
//    digitalWrite(I2C_SDA, HIGH);
//    i2c_delay();
// }
// //等待应答信号到来
// //返回值：1，接收应答失败
// //        0，接收应答成功
// uint8_t i2c_wait_ack(void)
// {
//   uint8_t ucErrTime = 0;
//   pinMode(I2C_SDA, INPUT_PULLUP); 
// 	digitalWrite(I2C_SDA, HIGH);   i2c_delay();	   
// 	digitalWrite(I2C_SCL, HIGH);   i2c_delay();	 
// 	while(digitalRead(I2C_SDA))
// 	{
// 		ucErrTime++;
// 		if(ucErrTime > 250)
// 		{
// 			i2c_stop();
// 			return 1;
// 		}
// 	}
//   digitalWrite(I2C_SCL, LOW);   
// 	return 0;   
// }

// //产生ACK应答
// void i2c_ack(void)
// {
//   digitalWrite(I2C_SCL, LOW);   
//   pinMode(I2C_SDA, OUTPUT);
// 	digitalWrite(I2C_SDA, LOW); 
// 	i2c_delay();
// 	digitalWrite(I2C_SCL, HIGH); 
// 	i2c_delay();
// 	digitalWrite(I2C_SCL, LOW); 
// }
// //不产生ACK应答		    
// void i2c_nack(void)
// {
// 	digitalWrite(I2C_SCL, LOW);
// 	pinMode(I2C_SDA, OUTPUT);
// 	digitalWrite(I2C_SDA, HIGH);
// 	i2c_delay();
// 	digitalWrite(I2C_SCL, HIGH); 
// 	i2c_delay();
// 	digitalWrite(I2C_SCL, LOW); 
// }	
// //IIC发送一个字节
// //返回从机有无应答
// //1，有应答
// //0，无应答			  
// void i2c_send_byte(uint8_t txd)
// {                        
//   uint8_t t;   
// 	pinMode(I2C_SDA, OUTPUT); 	    
//   digitalWrite(I2C_SCL, LOW); //拉低时钟开始数据传输
//   for(t = 0; t < 8; t++)
//   {   
//       if((txd & 0x80) >> 7)    digitalWrite(I2C_SDA, HIGH);  
//       else                     digitalWrite(I2C_SDA, LOW);    
//       txd <<= 1; 	  
//       digitalWrite(I2C_SCL, HIGH); 
//       i2c_delay(); 
//       digitalWrite(I2C_SCL, LOW);	
//       i2c_delay();
//   }	 
// } 	    
// //读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
// uint8_t i2c_read_byte(unsigned char ack)
// {
// 	unsigned char i, receive=0;
// 	pinMode(I2C_SDA, INPUT_PULLUP); //SDA设置为输入
//   for(i = 0;i < 8;i++)
// 	{
//     digitalWrite(I2C_SCL, LOW);	
//     i2c_delay();
// 		digitalWrite(I2C_SCL, HIGH); 
//     receive <<= 1;
//     if(digitalRead(I2C_SDA)) receive++;   
// 		i2c_delay(); 
//   }					 
//   if (!ack)
//       i2c_nack();//发送nACK
//   else
//       i2c_ack(); //发送ACK   
//   return receive;
// }




