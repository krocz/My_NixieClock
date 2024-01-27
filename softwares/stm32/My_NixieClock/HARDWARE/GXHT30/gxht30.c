#include "gxht30.h"
#include "delay.h"
#include "usart.h"

const uint16_t N = 0xffff;
 //GXHT IIC ��ʱ����
void GXHT_IIC_Delay(void)
{
	delay_us(2);
}

//��ʼ��IIC
void GXHT_IIC_Init(void)
{					     
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(GXHT_IIC_CLK, ENABLE);//��ʹ������IO PORTBʱ�� 
		
	GPIO_InitStructure.GPIO_Pin = GXHT_IIC_SCL_PIN | GXHT_IIC_SDA_PIN;	 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GXHT_IIC_PORT, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO 

	GPIO_SetBits(GPIOB,GXHT_IIC_SCL_PIN | GXHT_IIC_SDA_PIN);	 // �����	
 
}
//����IIC��ʼ�ź�
void GXHT_IIC_Start(void)
{
	GXHT_SDA_OUT();     //sda�����
	GXHT_IIC_SDA=1;	  	  
	GXHT_IIC_SCL=1;
	GXHT_IIC_Delay();
 	GXHT_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	GXHT_IIC_Delay();
	GXHT_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void GXHT_IIC_Stop(void)
{
	GXHT_SDA_OUT();//sda�����
	GXHT_IIC_SCL=0;
	GXHT_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	GXHT_IIC_Delay();
	GXHT_IIC_SCL=1; 
	GXHT_IIC_SDA=1;//����I2C���߽����ź�
	GXHT_IIC_Delay();							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 GXHT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	GXHT_SDA_IN();      //SDA����Ϊ����  
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
	GXHT_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void GXHT_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	GXHT_SDA_OUT(); 	    
    GXHT_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 GXHT_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	GXHT_SDA_IN();//SDA����Ϊ����
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
        GXHT_IIC_NAck();//����nACK
    else
        GXHT_IIC_Ack(); //����ACK   
    return receive;
}

/* =======================GXHT30��ȡ��غ���======================= */
//IICдһ���ֽ� 
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
u8 GXHT_Write_Byte(u8 reg,u8 data) 				 
{ 
    GXHT_IIC_Start(); 
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|0);//����������ַ+д����	
	if(GXHT_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		GXHT_IIC_Stop();		 
		return 1;		
	}
    GXHT_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    GXHT_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	GXHT_IIC_Send_Byte(data);//��������
	if(GXHT_IIC_Wait_Ack())	//�ȴ�ACK
	{
		GXHT_IIC_Stop();	 
		return 1;		 
	}		 
    GXHT_IIC_Stop();	 
	return 0;
}

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 GXHT_Read_Byte(u8 reg)
{
	u8 res;
    GXHT_IIC_Start(); 
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|0);//����������ַ+д����	
	GXHT_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
    GXHT_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    GXHT_IIC_Wait_Ack();		//�ȴ�Ӧ��
    GXHT_IIC_Start();
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|1);//����������ַ+������	
    GXHT_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	res=GXHT_IIC_Read_Byte(0);//��ȡ����,����nACK 
    GXHT_IIC_Stop();			//����һ��ֹͣ���� 
	return res;		
}

// ִ��GXHT30�ĵ���ת��ģʽ
// temper����ȡ���¶����ݣ�humid����ȡ��ʪ������
// ����ֵ��0��������������ʾ����
u8 GXHT_Get_One_Value(float *temper, float *humid) 				 
{ 
	u8 buf[6], get_NACK = 1, i;
	u16 t,h;
    GXHT_IIC_Start(); 
	GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|0);//����������ַ+д����	
	if(GXHT_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		GXHT_IIC_Stop();		 
		return 1;		
	}
	GXHT_IIC_Send_Byte(CMD_MSB);	//д�����MSB
	GXHT_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	GXHT_IIC_Send_Byte(CMD_LSB);	//д�����LSB
	if(GXHT_IIC_Wait_Ack())	    //�ȴ�ACK
	{
		GXHT_IIC_Stop();	 
		return 1;		 
	}		
    GXHT_IIC_Stop();	
	
	//��ʼ��ȡ����
	delay_ms(1);  //��ʼ֮ǰ������˵����Ҫ���ٵȴ�1ms
	do{
		GXHT_IIC_Start(); 
		GXHT_IIC_Send_Byte((GXHT_ADDR<<1)|1);//����������ַ+������	
		if(GXHT_IIC_Wait_Ack())  //���յ�NACK��˵�����ݻ�û��ת�����
		{
			GXHT_IIC_Stop();     
			delay_ms(2);        //�ȴ�2ms֮���ٴ�ѯ�������Ƿ�ת�����
		}
		else
		{ get_NACK = 0; }
	}while(get_NACK);   
	
	for(i = 0; i < 6; i++)
	{
		if(i < 5) buf[i] = GXHT_IIC_Read_Byte(1);   //������,����ACK  
		else buf[i] = GXHT_IIC_Read_Byte(0);   //������,����NACK 
	}    
    GXHT_IIC_Stop();  //����һ��ֹͣ����  
	
	t = ((u16)buf[0]<<8)|buf[1];  
	h = ((u16)buf[3]<<8)|buf[4];  
	
	*temper = -45.0 + 175.0 * (float)t / N;
	*humid = 100.0 * h / N;
	return 0;
}


//��ʼ��GXHT30
//����ֵ:0,�ɹ�
//    ����,�������
u8 GXHT_Init(void)
{ 
	GXHT_IIC_Init();//��ʼ��IIC����
	delay_ms(50);
	
	return 0;
}



