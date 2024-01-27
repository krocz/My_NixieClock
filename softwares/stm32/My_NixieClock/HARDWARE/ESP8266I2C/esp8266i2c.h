#ifndef __ESP8266I2C_H
#define __ESP8266I2C_H
#include "sys.h"

#define I2C2_MAX_RECV_LEN		100					//最大接收缓存字节数

extern u8  I2C2_RX_BUF[I2C2_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern vu16 I2C2_RX_STA;   						//接收数据状态
extern u8 I2C2_RX_FLAG;                           //接收到完整数据的标志

void init_esp8266i2c(void);

#endif
