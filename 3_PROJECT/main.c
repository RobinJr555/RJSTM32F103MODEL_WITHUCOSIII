/*******************************************************************************
 * @name     : 
 * @author   : RobinJR.
 * @version  : V1.0.0
 * @firstedit: 
 * @lastedit : 
 * @brief    : 
 * ---------------------------------------------------------------------------- 
 * @copyright
  
 * ----------------------------------------------------------------------------
 * @description

 *
 *-----------------------------------------------------------------------------
 ******************************************************************************/
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

#include <stdio.h>
#include "string.h"
#include "ModbusProtocol.h"
#include "UCOSIII_SUPPORT.h"

/*-------------------------����������-------------------------------*/																			 
extern UCHAR    usRegCoilsBuf[REG_COILS_NREGS]; 
extern UCHAR    usRegDiscreteBuf[REG_DISCRETE_NREGS];
extern USHORT   usRegInputBuf[REG_INPUT_NREGS];
extern USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

/*-------------------------��ʼ����-------------------------------*/
//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

/*-------------------------LED����-------------------------------*/
//�������ȼ�
#define LED_TASK_PRIO		5
//�����ջ��С	
#define LED_STK_SIZE 		512
//������ƿ�
OS_TCB LedTaskTCB;
//�����ջ	
CPU_STK LED_TASK_STK[LED_STK_SIZE];
void led_task(void *p_arg);

/*-------------------------	MODBUS����-------------------------------*/
//�������ȼ�
#define FREEMODBUS_TASK_PRIO		4
//�����ջ��С	
#define FREEMODBUS_STK_SIZE 		512
//������ƿ�
OS_TCB FreeModbusTaskTCB;
//�����ջ	
CPU_STK FREEMODBUS_TASK_STK[FREEMODBUS_STK_SIZE];
void FreeModbus_task(void *p_arg);

/* ----------------------- Start implementation -----------------------------*/
/**
  * @brief  Init Peripherals.
  * @param  None
  * @retval None
  */
	void Peripherals_Init(void)
{
	LED_Init();
	Delay_Init();      //��ʱ��ϵͳOSʱ�ӳ�ʼ��

	eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE); //MODBUS��ʼ��
	eMBEnable(); 

}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	Peripherals_Init();//�����ʼ��

	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
								 
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	
/*-------------------------LED����-------------------------------*/
	OSTaskCreate((OS_TCB 	* )&LedTaskTCB,		
				 (CPU_CHAR	* )"led0 task", 		
                 (OS_TASK_PTR )led_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED_TASK_PRIO,     
                 (CPU_STK   * )&LED_TASK_STK[0],	
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);		
								 
/*-------------------------MODBUS����-------------------------------*/
	OSTaskCreate((OS_TCB 	* )&FreeModbusTaskTCB,		
				 (CPU_CHAR	* )"Modbus task", 		
                 (OS_TASK_PTR )FreeModbus_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )FREEMODBUS_TASK_PRIO,     
                 (CPU_STK   * )&FREEMODBUS_TASK_STK[0],	
                 (CPU_STK_SIZE)FREEMODBUS_STK_SIZE/10,	
                 (CPU_STK_SIZE)FREEMODBUS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
								 


								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
				
			 
}

//led0������
void led_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED_Toggle();
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

//MODBUS������
void FreeModbus_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		(void)eMBPoll();
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ100ms
	}
}