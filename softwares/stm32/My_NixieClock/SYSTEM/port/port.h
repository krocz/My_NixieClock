#ifndef __PORT_H
#define __PORT_H	
#include "stm32f10x.h"

//������
#define BEEP_GPIO_PORT                      GPIOB
#define BEEP_GPIO_CLK                       RCC_APB2Periph_GPIOB
#define BEEP_PWM_PIN                        GPIO_Pin_0


//����
#define NEON_GPIO_PORT                      GPIOB
#define NEON_GPIO_CLK                       RCC_APB2Periph_GPIOB
#define NEON_GPIO_PIN                       GPIO_Pin_1


//��ʪ�ȴ�����
#define GXHT_IIC_PORT                       GPIOB
#define GXHT_IIC_CLK                        RCC_APB2Periph_GPIOB
#define GXHT_IIC_SCL_PIN                    GPIO_Pin_6
#define GXHT_IIC_SDA_PIN                    GPIO_Pin_7
//IO��������(PB7)
#define GXHT_SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define GXHT_SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}
//IO��������	 
#define GXHT_IIC_SCL    PBout(6) 		//SCL
#define GXHT_IIC_SDA    PBout(7) 		//SDA	 
#define GXHT_READ_SDA   PBin(7) 		//����SDA 


//��Χ��
#define WS2812C_GPIO_PORT                   GPIOB
#define WS2812C_GPIO_CLK                    RCC_APB2Periph_GPIOB
#define WS2812C_PWM_PIN                     GPIO_Pin_8


//����3 - WiFiģ���STM32ͨ��
#define USART3_GPIO_PORT                    GPIOB
#define USART3_GPIO_CLK                     RCC_APB2Periph_GPIOB
#define USART3_TX_PIN                       GPIO_Pin_10
#define USART3_RX_PIN                       GPIO_Pin_11

//Ӳ��IIC - WiFiģ���STM32ͨ��
#define WiFi_GPIO_PORT                     GPIOB
#define WiFi_PORT_SOURCE                   GPIO_PortSourceGPIOB
#define WiFi_GPIO_CLK                      RCC_APB2Periph_GPIOB
#define WiFi_SCL_PIN                       GPIO_Pin_10
#define WiFi_PIN_SOURCE                    GPIO_PinSource10
#define WiFi_EXIT_LINE                     EXTI_Line10
#define WiFi_SDA_PIN                       GPIO_Pin_11
#define WiFi_NVIC_SOURCE                   EXTI15_10_IRQn



//ʱ��оƬ
#define DS1302_GPIO_PORT                    GPIOB
#define DS1302_GPIO_CLK                     RCC_APB2Periph_GPIOB

#define DS1302_CE_PIN                       GPIO_Pin_14
#define DS1302_SCL_PIN                      GPIO_Pin_12
#define DS1302_SDA_PIN                      GPIO_Pin_13
//IO�������� PB13
#define DS1302_SDA_IN()  {GPIOB->CRH&=0XFF0FFFFF;GPIOB->CRH|=(u32)8<<20;}  
#define DS1302_SDA_OUT() {GPIOB->CRH&=0XFF0FFFFF;GPIOB->CRH|=(u32)3<<20;} 
//IO��������	 
#define DS1302_CE           PBout(14)       //CE
#define DS1302_SCL          PBout(12) 		//SCLK
#define DS1302_WRITE_SDA    PBout(13) 		//SDA	 
#define DS1302_READ_SDA     PBin(13) 		//����SDA 
//===========================================================================
//�Թ������оƬ
#define HV57708_GPIO_PORT                   GPIOA
#define HV57708_GPIO_CLK                    RCC_APB2Periph_GPIOA

#define HV57708_CLK_PIN                     GPIO_Pin_2
#define HV57708_LE_PIN                      GPIO_Pin_3
#define HV57708_POL_PIN                     GPIO_Pin_1
#define HV57708_DI1_PIN                     GPIO_Pin_7
#define HV57708_DI2_PIN                     GPIO_Pin_6
#define HV57708_DI3_PIN                     GPIO_Pin_5
#define HV57708_DI4_PIN                     GPIO_Pin_4


#define HV57708_CLK	   PAout(2)	
#define HV57708_LE     PAout(3)
#define HV57708_POL    PAout(1)
#define HV57708_DI1    PAout(7)
#define HV57708_DI2    PAout(6)
#define HV57708_DI3    PAout(5)
#define HV57708_DI4    PAout(4)


//����1- ������
#define USART1_GPIO_PORT                    GPIOA 
#define USART1_GPIO_CLK                     RCC_APB2Periph_GPIOA
#define USART1_TX_PIN                       GPIO_Pin_9
#define USART1_RX_PIN                       GPIO_Pin_10


//����
#define KEY_GPIO_PORT                       GPIOA
#define KEY_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define KEY_GPIO_PIN_UP                     GPIO_Pin_11 
#define KEY_GPIO_PIN_DOWN                   GPIO_Pin_12


#endif
