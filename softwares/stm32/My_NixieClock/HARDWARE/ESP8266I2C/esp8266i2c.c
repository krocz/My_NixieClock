#include "esp8266i2c.h"
#include "usart.h"

u8 I2C2_RX_FLAG = 0;  
u8 I2C2_RX_BUF[I2C2_MAX_RECV_LEN]; 
vu16 I2C2_RX_STA=0;       //接收状态标记	 

void init_esp8266i2c(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(WiFiIIC_GPIO_CLK, ENABLE);    //先使能外设IO PORTB时钟 
		
	GPIO_InitStructure.GPIO_Pin = WiFiIIC_SCL_PIN | WiFiIIC_SDA_PIN;	 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 		 //复用开漏，引脚由硬件控制
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(WiFiIIC_GPIO_PORT, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
	
	
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 50000;  //50KHz
	// 占空比配置，在时钟频率配置为100kHz以下的标准输出模式，占空比无效默认,为1:1
	// 在快速模式下，可以选择SCL低电平:高电平=16:9和2:1。
	// 因为在快速模式下，需要给低电平更多的时间以便于SDA数据写入(弱上拉，所以上升沿需要时间变成高电平)
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x42;
	I2C_Init(I2C2, &I2C_InitStructure);
	I2C_Cmd(I2C2, ENABLE);
	
	
	 I2C_ITConfig(I2C2, I2C_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

void I2C2_EV_IRQHandler(void)
{
	u8 res;	      
	static u8 RxState = 0;   //状态机标志位,以0xfe 0xff作为包头，0xff 0xfe作为包尾
	static u8 LastRxData = 0;
	printf("中断外\r\n");
	if(I2C_GetITStatus(I2C2, I2C_IT_RXNE) != RESET)//接收到数据
	{	 
		printf("进入中断\r\n");
		res = I2C_ReceiveData(I2C2);	
		printf("res : %d\r\n", res);
		if(RxState == 0)
		{
			if(LastRxData == 0xfe && res == 0xff)
			{
				RxState = 1;
				I2C2_RX_STA = 0;
			}
		}
		else if(RxState == 1)
		{
			if (res != 0xff)
				I2C2_RX_BUF[I2C2_RX_STA++] = res;
			else
				RxState = 2;
		}
		else if(RxState == 2)
		{
			if (LastRxData == 0xff && res == 0xfe)
			{
				I2C2_RX_BUF[I2C2_RX_STA++] = '\0';
				RxState = 0;
				I2C2_RX_FLAG = 1;
			}
		}
		LastRxData = res;
		I2C_ClearITPendingBit(I2C2, I2C_IT_RXNE);
	}  				 				

}




