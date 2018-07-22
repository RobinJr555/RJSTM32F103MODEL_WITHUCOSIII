/*******************************************************************************
 * @name    : The file of USART drive
 * @author  : RobinJR.
 * @version : V1.0
 * @date    : 2018-6-22
 * @brief   : file for stm32 USART1,2,3
 * ---------------------------------------------------------------------------- 
 * @copyright

 * ----------------------------------------------------------------------------
 * @description
 * 
 * ע��ʹ�ñ����������ļ�ʱ����Ҫ��stm32f10x_it.c��д��Ӧ�Ĵ��ڽ����жϺ���
 * void USART1_IRQHandler(void)��
 *
 * COM1:
 * USART1_TX -> PA9
 * USART1_RX -> PA10 
 * 
 * COM2:
 * USART2_TX -> PA2
 * USART2_RX -> PA3
 * 
 * COM3:
 * USART3_TX -> PB10
 * USART3_RX -> PB11
 *
 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------

 * ----------------------------------------------------------------------------

 ******************************************************************************/

#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"

#define COMn							3							//����STM32֧�ֵĴ�����	

#define MODBUS_SUPPORT 		1   					//�Ƿ�����MODBUS����
#ifdef  MODBUS_SUPPORT
	#define MODBUS_USART USART1
#endif

//����print���ĸ���������������ʹ��printf����ע�͵���һ��
#define PRINTF_SUPPORT		UCORTEX_COM1	
#ifdef PRINTF_SUPPORT
#include <stdio.h>
#endif

/**
 * @brief ����COM PORT1�����ӵ�USART1
 */ 
#define UCORTEX_COM1								USART1
#define UCORTEX_COM1_CLK						RCC_APB2Periph_USART1
#define UCORTEX_COM1_TX_PIN					GPIO_Pin_9
#define UCORTEX_COM1_TX_GPIO_PORT		GPIOA
#define UCORTEX_COM1_TX_GPIO_CLK		RCC_APB2Periph_GPIOA
#define UCORTEX_COM1_RX_PIN					GPIO_Pin_10
#define UCORTEX_COM1_RX_GPIO_PORT		GPIOA
#define UCORTEX_COM1_RX_GPIO_CLK		RCC_APB2Periph_GPIOA
#define UCORTEX_COM1_IRQn						USART1_IRQn

/**
 * @brief ����COM PORT2�����ӵ�USART2
 */ 
#define UCORTEX_COM2								USART2
#define UCORTEX_COM2_CLK						RCC_APB1Periph_USART2
#define UCORTEX_COM2_TX_PIN					GPIO_Pin_2
#define UCORTEX_COM2_TX_GPIO_PORT		GPIOA
#define UCORTEX_COM2_TX_GPIO_CLK		RCC_APB2Periph_GPIOA
#define UCORTEX_COM2_RX_PIN					GPIO_Pin_3
#define UCORTEX_COM2_RX_GPIO_PORT		GPIOA
#define UCORTEX_COM2_RX_GPIO_CLK		RCC_APB2Periph_GPIOA
#define UCORTEX_COM2_IRQn						USART2_IRQn

/**
 * @brief ����COM PORT3�����ӵ�USART3
 */ 
#define UCORTEX_COM3								USART3
#define UCORTEX_COM3_CLK						RCC_APB1Periph_USART3
#define UCORTEX_COM3_TX_PIN					GPIO_Pin_10
#define UCORTEX_COM3_TX_GPIO_PORT		GPIOB
#define UCORTEX_COM3_TX_GPIO_CLK		RCC_APB2Periph_GPIOB
#define UCORTEX_COM3_RX_PIN					GPIO_Pin_11
#define UCORTEX_COM3_RX_GPIO_PORT		GPIOB
#define UCORTEX_COM3_RX_GPIO_CLK		RCC_APB2Periph_GPIOB
#define UCORTEX_COM3_IRQn						USART3_IRQn

//���崮�ڶ˿�
typedef enum 
{
  COM1 = 0,
  COM2 = 1,
	COM3 = 2
} COM_TypeDef;

void COM_Init(COM_TypeDef COM, uint32_t BaudRate); //���ڳ�ʼ��

#endif

/********************* (C) COPYRIGHT 2018 ROBINJR. **********END OF FILE**********/
