/*******************************************************************************
 * @name    : The head of One Wire protocol
 * @author  : RobinJR.
 * @version : V1.0
 * @date    : 2018-6-26
 * @brief   : General One Wire protocol
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

#ifndef __ONEWIRE_H
#define __ONEWIRE_H
#include "stm32f10x.h"
#include "bitband.h"
#include "delay.h"

#define OWCLOCK     RCC_APB2Periph_GPIOA
#define OWPORT 			GPIOA
#define OWPIN 			GPIO_Pin_1
#define OWPINNUM  	1

#define OW_SDA 			    GPIOout(OWPORT,OWPINNUM) //���õ��������ݽӿ�
#define READ_OW_SDA 		GPIOin(OWPORT,OWPINNUM)	 //��ȡ��������

typedef enum
{
	OW_SUCCESS = 0,
	OW_TIMEOUT,
	OW_ERROR,
}OW_StatusTypeDef;


void OW_Init(void);				//��ʼ��ONEWIRE��IO��		
void OW_SDA_IN(void);     //��������Ϊ����
void OW_SDA_OUT(void);		//��������Ϊ���

OW_StatusTypeDef OW_START(u8 us);			//��ʼ�ź�
u8 OW_ReadByte(void);	





#endif