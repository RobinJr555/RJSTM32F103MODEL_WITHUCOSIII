#include "TCS3200.h"

u16 amount;
u16 Rgena=205,Ggena=195,Bgena=255;//��ɫ����ɫ����ɫ����

void TCS3200_Init(void)
{
	TCS3200_GPIOinit();
	TCS3200EXIT_init();
	SetTCSMode(1,1);
}

void TCS3200_GPIOinit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(TCS_CLK, ENABLE);
  
    //���IO��ʼ��
    GPIO_InitStructure.GPIO_Pin =TCS_S0_PIN|TCS_S1_PIN|TCS_S2_PIN|TCS_S3_PIN|LED_CON_PIN;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(TCS_PORT, &GPIO_InitStructure);
		GPIO_ResetBits(TCS_PORT,LED_CON_PIN);//��LED��ʾ
     
	  //����IO��ʼ��
    GPIO_InitStructure.GPIO_Pin =   TCS_OUT_PIN;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TCS_PORT, &GPIO_InitStructure);  
	
}

void TCS3200EXIT_init(void)
{
	 	EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��

		//�����ж���
  	GPIO_EXTILineConfig(TCS_OUT_PORTSOURCE,TCS_OUT_PINSOURCE);

  	EXTI_InitStructure.EXTI_Line=TCS_EXTI_LINE;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
 
}
//�ⲿ�жϼ����¼� �½��ش���
 void EXTI9_5_IRQHandler(void)
{			
	 amount ++;
 	 EXTI_ClearITPendingBit(EXTI_Line5);    //���LINE5�ϵ��жϱ�־λ  
}

/*
	s0      s1 
   0       0    Power down
   0       1    2%
   1       0    20%
   1       1    100%
*/
void SetTCSMode(u8 s0,u8 s1)
{
	S0 = s0;
	S1 = s1;
}

void whitebalance(void)
{

	S2=0;S3=0;//��ɫͨ��
	amount=0;			 //��ʼ����
	delay_ms(ALLTIME);
	Rgena = amount;   //�����ɫ����      
	amount=0;
//----------------------------------
	S2=1;S3=1;//��ɫͨ��
	amount=0;
	delay_ms(ALLTIME);
	Ggena = amount;	 //�����ɫ����
	amount=0;
//----------------------------------
	S2=0;S3=1;//��ɫͨ��
	amount=0;
	delay_ms(ALLTIME);
	Bgena = amount;	  //�����ɫ����
	amount=0;
	S2=1;S3=0;//�ر�ͨ�� 
	
	TCSLED = 0;	
}  

void SetWhiteBalance(void)
{
	TCSLED = 1;
	delay_ms(1000);	
	whitebalance();
	delay_ms(500);	
	TCSLED = 0;	
}

u16 GetTcs3200_RED(void)
{
	u16 Ramount;
	S2=0;S3=0;
	amount=0;
	delay_ms(ALLTIME);
	Ramount=(u32) amount*255/Rgena;	 //ȡRֵ
	if(Ramount>255) Ramount = 255;
	return Ramount;
//	amount=0;
}

u16 GetTcs3200_GREEN(void)
{
	u16 Gamount;
	S2=1;S3=1;
	amount=0;
	delay_ms(ALLTIME);
	Gamount=(u32) amount*255/Ggena;	//ȡGֵ
	if(Gamount>255) Gamount = 255;
	return Gamount;
//	amount=0;
}

u16 GetTcs3200_BLUE(void)
{
	u16 Bamount;
	S2=0;S3=1;
	amount=0;
	delay_ms(ALLTIME);
	Bamount=(u32) amount*255/Bgena;//ȥBֵ
	if(Bamount>255) Bamount = 255;
	return Bamount;
//	amount=0;
} 

void GetTcs3200_VALUE(u16* Get_Value)
{
	delay_ms(10);
	*Get_Value  	 = GetTcs3200_RED();
	delay_ms(10);
	*(Get_Value+1) = GetTcs3200_GREEN();
	delay_ms(10);
	*(Get_Value+2) = GetTcs3200_BLUE();
}

