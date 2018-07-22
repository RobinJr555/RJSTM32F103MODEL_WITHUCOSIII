#include "led.h"

/**
  * @brief  LED IO��ʼ��
  * @param  None
  * @retval None
	* @note   LED���ӵ�GPIOB.12���͵�ƽ�����ߵ�ƽ��
  */
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
       
  /* ʹ��GPIOBʱ�� */
  RCC_APB2PeriphClockCmd(LEDCLOCK, ENABLE);

  /* ����GPIOB.12Ϊ�������ģʽ */
  GPIO_InitStructure.GPIO_Pin = LEDPIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LEDPORT, &GPIO_InitStructure);
	
	GPIO_SetBits(LEDPORT, LEDPIN);//����GPIOB.12Ϊ�ߵ�ƽ���ر�LED
}

/**
  * @brief  ����LEDָʾ��
  * @param  None
  * @retval None
	* @note   LED���ӵ�GPIOB.12���͵�ƽ�����ߵ�ƽ��
  */
void LED_On(void)
{
	LED = 0;
}

/**
  * @brief  �ر�LEDָʾ��
  * @param  None
  * @retval None
	* @note   LED���ӵ�GPIOB.12���͵�ƽ�����ߵ�ƽ��
  */
void LED_Off(void)
{
	LED = 1;
}

/**
  * @brief  LEDָʾ�Ʒ�ת
  * @param  None
  * @retval None
	* @note   LED���ӵ�GPIOB.12���͵�ƽ�����ߵ�ƽ��
  */
void LED_Toggle(void)
{
	LED = !LED;
}