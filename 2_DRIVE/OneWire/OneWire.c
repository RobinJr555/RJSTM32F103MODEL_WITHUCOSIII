#include "OneWire.h"

void OW_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		//ʹ������ʱ��
		RCC_APB2PeriphClockCmd(OWCLOCK, ENABLE );	
		
		//��������Ҫ��Ӳ���������������裬�����������Ϊ��©���
		GPIO_InitStructure.GPIO_Pin = OWPIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//��©���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(OWPORT, &GPIO_InitStructure);
}

void OW_SDA_IN(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = OWPIN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     
    GPIO_Init(OWPORT,&GPIO_InitStructure);
}

void OW_SDA_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = OWPIN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
    GPIO_Init(OWPORT,&GPIO_InitStructure);
}


OW_StatusTypeDef OW_START(u8 ms)			//��ʼ�ź�
{
		OW_SDA_OUT();
		//��������
		OW_SDA = 0;
		delay_ms(ms);
	
		//�ȴ���Ӧ ������һ��ʱ�䣬������һ��ʱ�䣩
		OW_SDA = 1;
		delay_us(20);
	  OW_SDA_IN();
		
		//��Ӧ�ź�Ϊ0��ȴ����ߣ�Ϊ1���ȡʧ��
		if(!READ_OW_SDA) 
		{
				while(!READ_OW_SDA); //�����С���
				while(READ_OW_SDA);  //�����С���
				return OW_SUCCESS; //�ɹ�
		}
		
		return OW_ERROR; //ʧ��
}

//��ȡһ�ֽ�����
u8 OW_ReadByte(void)
{
		u8 i;
		u8 sbuf = 0;
		for(i = 0; i < 8; i++)
				{
						sbuf <<= 1; //MSB first
						delay_us(20);
						while(!READ_OW_SDA);// 50us �͵�ƽ�ź�
						delay_us(30);				// 26us �ߵ�ƽ�ź�
						if(READ_OW_SDA)			// bit = 1
						{
								sbuf |= 0x01;
								delay_us(30);
						}
						else // bit = 0;
						{
								sbuf &= 0xfe;
						
						}
				}
		return sbuf;

}

