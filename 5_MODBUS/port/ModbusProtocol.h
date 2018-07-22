#ifndef __MODBUSPROTOCOL_H
#define __MODBUSPROTOCOL_H
#include "stm32f10x.h"
//���FREEMODBUS ����Ĵ�������Դ��
//�õ��˴���1 �� ��ʱ��4�ĸ����ж�
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
//�˴��ļĴ���������Ҫ���õı�ʵ�ʴ�1
#define REG_COILS_START 				0X0000
#define REG_COILS_NREGS 				17

#define REG_DISCRETE_START 			0X0000
#define REG_DISCRETE_NREGS 			17

#define REG_INPUT_START 				0x0000
#define REG_INPUT_NREGS 				9

#define REG_HOLDING_START 			0x0000
#define REG_HOLDING_NREGS 			9


#endif