#ifndef __ESP8266EXIT_H
#define __ESP8266EXIT_H
#include "sys.h"
#include "ds1302.h"


#define BAG_HEAD 0xfffe
#define BAG_HAIL 0xfeff

extern u8 ESP8266_RX_FLAG;
extern u32 epoch_time;
extern TIMEData beijing_time;



void init_esp8266exit(void);
void convert_beijing_time(long long epochTime);

#endif
