#ifndef __KEY_H
#define __KEY_H	 

#include "stm32f10x.h"

#define KEY0_CLOCK     RCC_APB2Periph_GPIOC
#define KEY0_PORT 		 GPIOC
#define KEY0_PIN 			 GPIO_Pin_1
#define KEY0_PINNUM  	 1

#define KEY1_CLOCK     RCC_APB2Periph_GPIOC
#define KEY1_PORT 		 GPIOC
#define KEY1_PIN 			 GPIO_Pin_13
#define KEY1_PINNUM  	 13

#define KEY0  GPIO_ReadInputDataBit(KEY0_PORT,KEY0_PIN)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)//��ȡ����1
 

#define KEY0_PRES	1		//KEY0  
#define KEY1_PRES	2		//KEY1 

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��					    
#endif
