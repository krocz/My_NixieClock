#include "gxht30.h"
#include "delay.h"
#include "usart.h"

const uint16_t N = 0xffff;
 //GXHT IIC 延时函数
void GXHT_IIC_Delay(void)
{
	delay_us(2);
}

//初始化IIC
void GXHT_IIC_Init(void)
{					     
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(GXHT_IIC_CLK, ENABLE);//先使能外设IO PORTB时钟 
		
	GPIO_InitStructure.GPIO_Pin = GXHT_IIC_SCL_PIN | GXHT_IIC_SDA_PIN;	 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GXHT_IIC_PORT, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 

	GPIO_SetBits(GPIOB,GXHT_IIC_SCL_PIN | GXHT_IIC_SDA_PIN);	 // 输出高	
 
}
//产生IIC起始信号
void GXHT_IIC_Start(void)
{
	GXHT_SDA_OUT();     //sda线输出
	GXHT_IIC_SDA=1;	  	  
	GXHT_IIC_SCL=1;
	GXHT_IIC_Delay();
 	GXHT_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	GXHT_IIC_Delay();
	GXHT_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void GXHT_IIC_Stop(void)
{
	GXHT_SDA_OUT();//sda线输出
	GXHT_IIC_SCL=0;
	GXHT_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	GXHT_IIC_Delay();
	GXHT_IIC_SCL=1; 
	GXHT_IIC_SDA=1;//发送I2C总线结束信号
	GXHT_IIC_Delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 GXHT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	GXHT_SDA_IN();      //SDA设置为输入  
	GXHT_IIC_SDA=1;GXHT_IIC_Delay();	   
	GXHT_IIC_SCL=1;GXHT_IIC_Delay();	 
	while(GXHT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			GXHT_IIC_Stop();
			return 1;
		}
	}
	GXHT_IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void GXHT_IIC_Ack(void)
{
	GXHT_IIC_SCL=0;
	GXHT_SDA_OUT();
	GXHT_IIC_SDA=0;
	GXHT_IIC_Delay();
	GXHT_IIC_SCL=1;
	GXHT_IIC_Delay();
	GXHT_IIC_SCL=0;
}
//不产生ACK应答		    
void GXHT_IIC_NAck(void)
{
	GXHT_IIC_SCL=0;
	GXHT_SDA_OUT();
	GXHT_IIC_SDA=1;
	GXHT_IIC_Delay();
	GXHT_IIC_SCL=1;
	GXHT_IIC_Delay();
	GXHT_IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void GXHT_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	GXHT_SDA_OUT(); 	    
    GXHT_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        GXHT_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		    GXHT_IIC_SCL=1;
		    GXHT_IIC_Delay(); 
		    GXHT_IIC_SCL=0;	
		    GXHT_IIC_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 GXHT_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	GXHT_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        GXHT_IIC_SCL=0; 
        GXHT_IIC_Delay();
		GXHT_IIC_SCL=1;
        receive<<=1;
        if(GXHT_READ_SDA)receive++;   
		GXHT_IIC_Delay(); 
    }					 
    if (!ack)
        GXHT_IIC_NAck();//发送nACK
    else
        GXHT_IIC_Ack(); //发送ACK   
    return receive;
}

/* =======================GXHT30读取相关函数======================= */
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 GXHT_Write_Byte(u8 reg,u8 data) 				 
{ 
    GXHT_IIC_Start(); 
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|0);//发送器件地址+写命令	
	if(GXHT_IIC_Wait_Ack())	//等待应答
	{
		GXHT_IIC_Stop();		 
		return 1;		
	}
    GXHT_IIC_Send_Byte(reg);	//写寄存器地址
    GXHT_IIC_Wait_Ack();		//等待应答 
	GXHT_IIC_Send_Byte(data);//发送数据
	if(GXHT_IIC_Wait_Ack())	//等待ACK
	{
		GXHT_IIC_Stop();	 
		return 1;		 
	}		 
    GXHT_IIC_Stop();	 
	return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 GXHT_Read_Byte(u8 reg)
{
	u8 res;
    GXHT_IIC_Start(); 
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|0);//发送器件地址+写命令	
	GXHT_IIC_Wait_Ack();		//等待应答 
    GXHT_IIC_Send_Byte(reg);	//写寄存器地址
    GXHT_IIC_Wait_Ack();		//等待应答
    GXHT_IIC_Start();
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|1);//发送器件地址+读命令	
    GXHT_IIC_Wait_Ack();		//等待应答 
	res=GXHT_IIC_Read_Byte(0);//读取数据,发送nACK 
    GXHT_IIC_Stop();			//产生一个停止条件 
	return res;		
}

// 执行GXHT30的单次转换模式
// temper：读取的温度数据，humid：读取的湿度数据
// 返回值：0，正常；其他表示错误
u8 GXHT_Get_One_Value(float *temper, float *humid) 				 
{ 
	u8 buf[6], get_NACK = 1, i;
	u16 t,h;
    GXHT_IIC_Start(); 
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|0);//发送器件地址+写命令	
	if(GXHT_IIC_Wait_Ack())	//等待应答
	{
		GXHT_IIC_Stop();		 
		return 1;		
	}
	GXHT_IIC_Send_Byte(CMD_MSB);	//写命令的MSB
	GXHT_IIC_Wait_Ack();		//等待应答 
	GXHT_IIC_Send_Byte(CMD_LSB);	//写命令的LSB
	if(GXHT_IIC_Wait_Ack())	    //等待ACK
	{
		GXHT_IIC_Stop();	 
		return 1;		 
	}		
    GXHT_IIC_Stop();	
	
	//开始读取数据
	delay_ms(1);  //开始之前，按照说明需要至少等待1ms
	do{
		GXHT_IIC_Start(); 
		GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|1);//发送器件地址+读命令	
		if(GXHT_IIC_Wait_Ack())  //接收到NACK，说明数据还没有转换完成
		{
			GXHT_IIC_Stop();     
			delay_ms(2);        //等待2ms之后，再次询问数据是否转换完成
		}
		else
		{ get_NACK = 0; }
	}while(get_NACK);   
	
	for(i = 0; i < 6; i++)
	{
		if(i < 5) buf[i] = GXHT_IIC_Read_Byte(1);   //读数据,发送ACK  
		else buf[i] = GXHT_IIC_Read_Byte(0);   //读数据,发送NACK 
	}    
    GXHT_IIC_Stop();  //产生一个停止条件  
	
	t = ((u16)buf[0]<<8)|buf[1];  
	h = ((u16)buf[3]<<8)|buf[4];  
	
	*temper = -45.0 + 175.0 * (float)t / N;
	*humid = 100.0 * h / N;
	return 0;
}


//初始化GXHT30
//返回值:0,成功
//    其他,错误代码
u8 GXHT_Init(void)
{ 
	GXHT_IIC_Init();//初始化IIC总线
	delay_ms(50);
	
	return 0;
}



