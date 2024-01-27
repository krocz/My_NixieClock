#ifndef __ESP8266I2C_H
#define __ESP8266I2C_H
#include "sys.h"

#define I2C2_MAX_RECV_LEN		100					//�����ջ����ֽ���

extern u8  I2C2_RX_BUF[I2C2_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern vu16 I2C2_RX_STA;   						//��������״̬
extern u8 I2C2_RX_FLAG;                           //���յ��������ݵı�־

void init_esp8266i2c(void);

#endif
