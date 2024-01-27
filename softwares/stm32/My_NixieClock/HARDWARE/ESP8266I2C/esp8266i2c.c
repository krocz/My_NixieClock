#include "esp8266i2c.h"
#include "usart.h"

u8 I2C2_RX_FLAG = 0;  
u8 I2C2_RX_BUF[I2C2_MAX_RECV_LEN]; 
vu16 I2C2_RX_STA=0;       //����״̬���	 

void init_esp8266i2c(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(WiFiIIC_GPIO_CLK, ENABLE);    //��ʹ������IO PORTBʱ�� 
		
	GPIO_InitStructure.GPIO_Pin = WiFiIIC_SCL_PIN | WiFiIIC_SDA_PIN;	 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 		 //���ÿ�©��������Ӳ������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(WiFiIIC_GPIO_PORT, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO 
	
	
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 50000;  //50KHz
	// ռ�ձ����ã���ʱ��Ƶ������Ϊ100kHz���µı�׼���ģʽ��ռ�ձ���ЧĬ��,Ϊ1:1
	// �ڿ���ģʽ�£�����ѡ��SCL�͵�ƽ:�ߵ�ƽ=16:9��2:1��
	// ��Ϊ�ڿ���ģʽ�£���Ҫ���͵�ƽ�����ʱ���Ա���SDA����д��(��������������������Ҫʱ���ɸߵ�ƽ)
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x42;
	I2C_Init(I2C2, &I2C_InitStructure);
	I2C_Cmd(I2C2, ENABLE);
	
	
	 I2C_ITConfig(I2C2, I2C_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

void I2C2_EV_IRQHandler(void)
{
	u8 res;	      
	static u8 RxState = 0;   //״̬����־λ,��0xfe 0xff��Ϊ��ͷ��0xff 0xfe��Ϊ��β
	static u8 LastRxData = 0;
	printf("�ж���\r\n");
	if(I2C_GetITStatus(I2C2, I2C_IT_RXNE) != RESET)//���յ�����
	{	 
		printf("�����ж�\r\n");
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




