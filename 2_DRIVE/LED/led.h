/*******************************************************************************
 * @name    : The head of led
 * @author  : RobinJR.
 * @version : V1.0
 * @date    : 2018-6-26
 * @brief   : General led file
 * ---------------------------------------------------------------------------- 
 * @copyright
 *
 * ----------------------------------------------------------------------------
 * @description

 *
 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------

 ******************************************************************************/


#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"
#include "bitband.h"	//ʹ��λ��������һ��Ҫ������ͷ�ļ�

#define LEDCLOCK     RCC_APB2Periph_GPIOC
#define LEDPORT 		 GPIOC
#define LEDPIN 			 GPIO_Pin_13
#define LEDPINNUM  	 13


#define LED		GPIOout(LEDPORT, LEDPINNUM)	//����PB12λ������

void LED_Init(void);	//LED IO��ʼ��
void LED_On(void);		//LED��
void LED_Off(void);		//LED��0
void LED_Toggle(void);//LED״̬��ת

#endif

