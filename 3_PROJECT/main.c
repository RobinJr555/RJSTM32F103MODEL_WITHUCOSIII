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

/*-------------------------公共变量区-------------------------------*/																			 
extern UCHAR    usRegCoilsBuf[REG_COILS_NREGS]; 
extern UCHAR    usRegDiscreteBuf[REG_DISCRETE_NREGS];
extern USHORT   usRegInputBuf[REG_INPUT_NREGS];
extern USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

/*-------------------------起始任务-------------------------------*/
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

/*-------------------------LED任务-------------------------------*/
//任务优先级
#define LED_TASK_PRIO		5
//任务堆栈大小	
#define LED_STK_SIZE 		512
//任务控制块
OS_TCB LedTaskTCB;
//任务堆栈	
CPU_STK LED_TASK_STK[LED_STK_SIZE];
void led_task(void *p_arg);

/*-------------------------	MODBUS任务-------------------------------*/
//任务优先级
#define FREEMODBUS_TASK_PRIO		4
//任务堆栈大小	
#define FREEMODBUS_STK_SIZE 		512
//任务控制块
OS_TCB FreeModbusTaskTCB;
//任务堆栈	
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
	Delay_Init();      //延时及系统OS时钟初始化

	eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE); //MODBUS初始化
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
	
	Peripherals_Init();//外设初始化

	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
								 
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
	
/*-------------------------LED任务-------------------------------*/
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
								 
/*-------------------------MODBUS任务-------------------------------*/
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
								 


								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
				
			 
}

//led0任务函数
void led_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED_Toggle();
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

//MODBUS任务函数
void FreeModbus_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		(void)eMBPoll();
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms
	}
}