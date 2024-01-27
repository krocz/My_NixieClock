#ifndef __GXHT30_H
#define __GXHT30_H
#include "sys.h"

//GXHT30设备地址(ADDR引脚接地)
#define GXHT_ADDR				0X44

//单次转换相关命令
#define CMD_MSB 0x24   // clock stretching关闭，如果芯片没有完成转换会发送NACK
#define CMD_LSB 0x0B   // 中重复率，重复率越高精度越高，耗时越长
#define WAIT_TIME 6.5  // 中重复率的最大转换时间(ms)


//IIC所有操作函数
void GXHT_IIC_Delay(void);				//GXHT IIC延时函数
void GXHT_IIC_Init(void);                //初始化IIC的IO口				 
void GXHT_IIC_Start(void);				//发送IIC开始信号
void GXHT_IIC_Stop(void);	  			//发送IIC停止信号
void GXHT_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 GXHT_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 GXHT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void GXHT_IIC_Ack(void);					//IIC发送ACK信号
void GXHT_IIC_NAck(void);				//IIC不发送ACK信号

void IGXHT_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 GXHT_IIC_Read_One_Byte(u8 daddr,u8 addr);	  

/* =========GXHT30芯片相关========== */
u8 GXHT_Write_Byte(u8 reg,u8 data);
u8 GXHT_Read_Byte(u8 reg);
u8 GXHT_Init(void);
u8 GXHT_Get_One_Value(float *temper, float *humid);	

#endif
















