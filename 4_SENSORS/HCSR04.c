#include "HCSR04.h"

 
void Hcsr04Init(void)
{  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
  
    //���IO��ʼ��
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
     
	  //����IO��ʼ��
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
}
 
/********************************************************************************************************************************************
@f_name: void Hcsr04_Ranging(void)
@brief:	 ��������࣬��ֵ5�Σ�ʵ�� �������̷��ֲ�����̬�ıȽ��ȶ�����̬�Ĳ��ȶ���3�׷�Χ�ڱȽ��ȶ�
@param:	 float *p:��໺����� ��λΪcm
@return: None
*********************************************************************************************************************************************/
void Hcsr04_Ranging(float *p)
{
	u8 i;
	u32 j;
	float Hcsr04_Temp;	
	for(i=0;i<5;i++)
	{
		TRIG_Send=1;
		delay_us(10);
		TRIG_Send=0;
		while(!ECHO_Reci); //һֱΪ0 //������ʱ��
		while(ECHO_Reci)   //���� ���巵���С���
		{
			delay_us(10);
			j++;
			if(j >1000000)
			{
				break;
			}
		}
		Hcsr04_Temp+=340/2*j*10;//  ģ�����ɲ��3m 
		j=0;
		delay_ms(60);//��ֹ�����źŶԻ����źŵ�Ӱ��
	}
	*p=Hcsr04_Temp/50000; 		
}
