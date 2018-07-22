#include "delay.h"
#include "UCOSIII_SUPPORT.h"					//UCOSIII ʹ��	  

static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��

//֧��UCOSIII
#define delay_osrunning		OSRunning			      //OS�Ƿ����б��,0,������;1,������
#define delay_ostickspersec	OSCfg_TickRate_Hz	//OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting 	OSIntNestingCtr		//�ж�Ƕ�׼���,���ж�Ƕ�״���
/**
  * @brief  ����SysTick����ʱ��ΪHCLK/8
  * @param  None
  * @retval None
	* @note   
	*    ��HCLKΪ72MHzʱ����ӳ�ʱ��Ϊ0xFFFFFF/9000000 (S) = 1864ms
  */
void Delay_Init(void)
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SystemCoreClock/8000000;				//Ϊϵͳʱ�ӵ�1/8  

	reload=SystemCoreClock/8000000;				//ÿ���ӵļ������� ��λΪM  
	reload*=1000000/delay_ostickspersec;		//����delay_ostickspersec�趨���ʱ��
												//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	fac_ms=1000/delay_ostickspersec;			//����OS������ʱ�����ٵ�λ	   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 						//ÿ1/delay_ostickspersec���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    
}

//us����ʱʱ,�ر��������(��ֹ���us���ӳ�)
void delay_osschedlock(void)
{
	OS_ERR err; 
	OSSchedLock(&err);							//UCOSIII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
}

//us����ʱʱ,�ָ��������
void delay_osschedunlock(void)
{	
	OS_ERR err; 
	OSSchedUnlock(&err);						//UCOSIII�ķ�ʽ,�ָ�����
}

//����OS�Դ�����ʱ������ʱ
//ticks:��ʱ�Ľ�����
void delay_ostimedly(u32 ticks)
{
	OS_ERR err; 
	OSTimeDly(ticks,OS_OPT_TIME_PERIODIC,&err);	//UCOSIII��ʱ��������ģʽ
}

/**
  * @brief  ms��ʱ����
	* @param  xms: ��ʱ�ĺ�����
  * @retval None
	* @note   
	*    ͨ������Ĺ�ʽ���ı�SysTick���ж�ʱ��:
  *                          
  *    ����ֵ(Reload Value) = SysTick����ʱ��Ƶ�� (Hz) x  ϣ���жϵ�ʱ���� (s)
  *  
  *    - ����ֵ��ΪSysTick_Config()�Ĳ������� 
	*    - ����ֵ�������� 0xFFFFFF������������ʱ1864ms
  */
void delay_ms(uint16_t xms)
{
	if(delay_osrunning&&delay_osintnesting==0)	//���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)	    
	{		 
		if(xms>=fac_ms)							//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			delay_ostimedly(xms/fac_ms);		//OS��ʱ
		}
		xms%=fac_ms;							//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(xms*1000));					//��ͨ��ʽ��ʱ  
}

/**
  * @brief  us��ʱ����
	* @param  xus: ��ʱ��΢����
  * @retval None
	* @note   
	*		ͨ������Ĺ�ʽ���ı�SysTick���ж�ʱ��:
  *                          
  *   ����ֵ(Reload Value) = SysTick����ʱ��Ƶ�� (Hz) x  ϣ���жϵ�ʱ���� (s)
  *  
  *   - ����ֵ��ΪSysTick_Config()�Ĳ�������
	*		- ����ֵ�������� 0xFFFFFF������������ʱ1864ms
  */
void delay_us(uint32_t xus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//LOAD��ֵ	    	 
	ticks=xus*fac_us; 							//��Ҫ�Ľ�����	  		 
	tcnt=0;
	delay_osschedlock();						//��ֹOS���ȣ���ֹ���us��ʱ
	told=SysTick->VAL;        					//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
	delay_osschedunlock();						//�ָ�OS����				
}

//systick�жϷ�����,ʹ��ucosʱ�õ�
void SysTick_Handler(void)
{	
	if(delay_osrunning==1)						//OS��ʼ����,��ִ�������ĵ��ȴ���
	{
		OSIntEnter();							//�����ж�
		OSTimeTick();       					//����ucos��ʱ�ӷ������               
		OSIntExit();       	 					//���������л����ж�
	}
}

/********************* (C) COPYRIGHT 2018 ROBINJR. **********END OF FILE**********/
