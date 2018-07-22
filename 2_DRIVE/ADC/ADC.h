#ifndef _ADC_H_
#define _ADC_H_
//�˴�ΪADC��ʼ���ļ�
#include "stm32f10x.h"




void ADC_Initialize(void);
void ADC_GPIO_Init(void);
void ADC_Channel_Initialize(void);

uint16_t Get_Adc(ADC_TypeDef* ADCx, uint8_t ch);
uint16_t Get_Adc_Average(ADC_TypeDef* ADCx, uint8_t ch);
float Get_Voltage(u16);//���õ���ADCֵת��Ϊʵ�ʵ�ѹ 3.3V�ο���ѹֵ

#endif