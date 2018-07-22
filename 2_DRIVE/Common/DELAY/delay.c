#include "delay.h"
#include "UCOSIII_SUPPORT.h"					//UCOSIII 使用	  

static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

//支持UCOSIII
#define delay_osrunning		OSRunning			      //OS是否运行标记,0,不运行;1,在运行
#define delay_ostickspersec	OSCfg_TickRate_Hz	//OS时钟节拍,即每秒调度次数
#define delay_osintnesting 	OSIntNestingCtr		//中断嵌套级别,即中断嵌套次数
/**
  * @brief  设置SysTick计数时钟为HCLK/8
  * @param  None
  * @retval None
	* @note   
	*    当HCLK为72MHz时，最长延迟时间为0xFFFFFF/9000000 (S) = 1864ms
  */
void Delay_Init(void)
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  

	reload=SystemCoreClock/8000000;				//每秒钟的计数次数 单位为M  
	reload*=1000000/delay_ostickspersec;		//根据delay_ostickspersec设定溢出时间
												//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/delay_ostickspersec;			//代表OS可以延时的最少单位	   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 						//每1/delay_ostickspersec秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    
}

//us级延时时,关闭任务调度(防止打断us级延迟)
void delay_osschedlock(void)
{
	OS_ERR err; 
	OSSchedLock(&err);							//UCOSIII的方式,禁止调度，防止打断us延时
}

//us级延时时,恢复任务调度
void delay_osschedunlock(void)
{	
	OS_ERR err; 
	OSSchedUnlock(&err);						//UCOSIII的方式,恢复调度
}

//调用OS自带的延时函数延时
//ticks:延时的节拍数
void delay_ostimedly(u32 ticks)
{
	OS_ERR err; 
	OSTimeDly(ticks,OS_OPT_TIME_PERIODIC,&err);	//UCOSIII延时采用周期模式
}

/**
  * @brief  ms延时函数
	* @param  xms: 延时的毫秒数
  * @retval None
	* @note   
	*    通过下面的公式来改变SysTick的中断时间:
  *                          
  *    重载值(Reload Value) = SysTick计数时钟频率 (Hz) x  希望中断的时间间隔 (s)
  *  
  *    - 重载值作为SysTick_Config()的参数传递 
	*    - 重载值不允许超过 0xFFFFFF，因此最长可以延时1864ms
  */
void delay_ms(uint16_t xms)
{
	if(delay_osrunning&&delay_osintnesting==0)	//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
	{		 
		if(xms>=fac_ms)							//延时的时间大于OS的最少时间周期 
		{ 
   			delay_ostimedly(xms/fac_ms);		//OS延时
		}
		xms%=fac_ms;							//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(xms*1000));					//普通方式延时  
}

/**
  * @brief  us延时函数
	* @param  xus: 延时的微秒数
  * @retval None
	* @note   
	*		通过下面的公式来改变SysTick的中断时间:
  *                          
  *   重载值(Reload Value) = SysTick计数时钟频率 (Hz) x  希望中断的时间间隔 (s)
  *  
  *   - 重载值作为SysTick_Config()的参数传递
	*		- 重载值不允许超过 0xFFFFFF，因此最长可以延时1864ms
  */
void delay_us(uint32_t xus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//LOAD的值	    	 
	ticks=xus*fac_us; 							//需要的节拍数	  		 
	tcnt=0;
	delay_osschedlock();						//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        					//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//时间超过/等于要延迟的时间,则退出.
		}  
	};
	delay_osschedunlock();						//恢复OS调度				
}

//systick中断服务函数,使用ucos时用到
void SysTick_Handler(void)
{	
	if(delay_osrunning==1)						//OS开始跑了,才执行正常的调度处理
	{
		OSIntEnter();							//进入中断
		OSTimeTick();       					//调用ucos的时钟服务程序               
		OSIntExit();       	 					//触发任务切换软中断
	}
}

/********************* (C) COPYRIGHT 2018 ROBINJR. **********END OF FILE**********/
