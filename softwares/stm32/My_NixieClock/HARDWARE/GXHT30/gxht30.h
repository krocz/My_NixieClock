#ifndef __GXHT30_H
#define __GXHT30_H
#include "sys.h"

//GXHT30�豸��ַ(ADDR���Žӵ�)
#define GXHT_ADDR				0X44

//����ת���������
#define CMD_MSB 0x24   // clock stretching�رգ����оƬû�����ת���ᷢ��NACK
#define CMD_LSB 0x0B   // ���ظ��ʣ��ظ���Խ�߾���Խ�ߣ���ʱԽ��
#define WAIT_TIME 6.5  // ���ظ��ʵ����ת��ʱ��(ms)


//IIC���в�������
void GXHT_IIC_Delay(void);				//GXHT IIC��ʱ����
void GXHT_IIC_Init(void);                //��ʼ��IIC��IO��				 
void GXHT_IIC_Start(void);				//����IIC��ʼ�ź�
void GXHT_IIC_Stop(void);	  			//����IICֹͣ�ź�
void GXHT_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 GXHT_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 GXHT_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void GXHT_IIC_Ack(void);					//IIC����ACK�ź�
void GXHT_IIC_NAck(void);				//IIC������ACK�ź�

void IGXHT_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 GXHT_IIC_Read_One_Byte(u8 daddr,u8 addr);	  

/* =========GXHT30оƬ���========== */
u8 GXHT_Write_Byte(u8 reg,u8 data);
u8 GXHT_Read_Byte(u8 reg);
u8 GXHT_Init(void);
u8 GXHT_Get_One_Value(float *temper, float *humid);	

#endif
















