#include "CAN.h"
#include "usart.h"

//������Ϊ��ӳ������
// PA11(PB8) - CAN-RX
// PA12(PB9) - CAN-TX

/*CAN��ʼ��  */
void MYCAN_Init(void)
{
	CAN_GPIO_Init();
	CAN_NVIC_Configuration();
	CAN_MODE_INIT();
}

//CAN����GPIO��ʼ��
void CAN_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  /* ���ù��ܺ�GPIOB�˿�ʱ��ʹ��*/	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);	                        											 

  /* CAN1 ģ��ʱ��ʹ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 

  /* Configure CAN pin: RX */	 // PB8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Configure CAN pin: TX */   // PB9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
	//#define GPIO_Remap_CAN    GPIO_Remap1_CAN1
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

}

/*CAN RX0 �ж����ȼ�����  */
 void CAN_NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* enabling interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//CAN�����ʼ��
void CAN_MODE_INIT(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  CanTxMsg TxMessage;  

  /* CAN register init */
  CAN_DeInit(CAN1);	//������CAN��ȫ���Ĵ�������Ϊȱʡֵ
  CAN_StructInit(&CAN_InitStructure);//��CAN_InitStruct�е�ÿһ��������ȱʡֵ����

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM=DISABLE;//û��ʹ��ʱ�䴥��ģʽ
  CAN_InitStructure.CAN_ABOM=DISABLE;//û��ʹ���Զ����߹���
  CAN_InitStructure.CAN_AWUM=DISABLE;//û��ʹ���Զ�����ģʽ
  CAN_InitStructure.CAN_NART=DISABLE;//û��ʹ�ܷ��Զ��ش�ģʽ
  CAN_InitStructure.CAN_RFLM=DISABLE;//û��ʹ�ܽ���FIFO����ģʽ
  CAN_InitStructure.CAN_TXFP=DISABLE;//û��ʹ�ܷ���FIFO���ȼ�
	
  CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;//CAN����Ϊ����ģʽ
  CAN_InitStructure.CAN_SJW=CAN_SJW_1tq; //����ͬ����Ծ���1��ʱ�䵥λ
  CAN_InitStructure.CAN_BS1=CAN_BS1_3tq; //ʱ���1Ϊ3��ʱ�䵥λ
  CAN_InitStructure.CAN_BS2=CAN_BS2_2tq; //ʱ���2Ϊ2��ʱ�䵥λ
  CAN_InitStructure.CAN_Prescaler=60;  //ʱ�䵥λ����Ϊ60	
  CAN_Init(CAN1,&CAN_InitStructure);
                                      //������Ϊ��72M/2/60(1+3+2)=0.1 ��100K

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber=0;//ָ��������Ϊ0
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;//ָ��������Ϊ��ʶ������λģʽ
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//������λ��Ϊ32λ
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;// ��������ʶ���ĸ�16λֵ
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;//	 ��������ʶ���ĵ�16λֵ
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//���������α�ʶ���ĸ�16λֵ
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;//	���������α�ʶ���ĵ�16λֵ
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;// �趨��ָ���������FIFOΪ0
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;// ʹ�ܹ�����
  CAN_FilterInit(&CAN_FilterInitStructure);//	������Ĳ�����ʼ��������

  /* CAN FIFO0 message pending interrupt enable */ 
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); //ʹ��FIFO0��Ϣ�Һ��ж�
 	
}

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 Can_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;			// ��׼��ʶ�� 
	TxMessage.ExtId=0x12;			// ������չ��ʾ�� 
	TxMessage.IDE=CAN_Id_Standard; 	// ��׼֡
	TxMessage.RTR=CAN_RTR_Data;		// ����֡
	TxMessage.DLC=len;				// Ҫ���͵����ݳ���
	for(i=0;i<len;i++)
		TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)return 1;
	return 0;	 
}

//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
  if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)
		return 0;		//û�н��յ�����,ֱ���˳� 
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
  for(i=0;i<8;i++)//���8���ֽ�
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

/* USB�жϺ�CAN�����жϷ������USB��CAN����I/O������ֻ�õ�CAN���жϡ� */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
	int i=0;
    CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);
	
		printf((unsigned char*)RxMessage.Data);
		printf("\r\n");
   
} 


