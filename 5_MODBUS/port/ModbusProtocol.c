#include "ModbusProtocol.h"
/*
��׼ʹ�÷���
int main(void)
{
	
	eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE); 
	eMBEnable(); 
  while (1)
  {

		(void)eMBPoll();
		
	}

}


*/

//��Э���0λ�������ӵ�1λ��ʼ��д
/* ----------------------- Static variables ---------------------------------*/
USHORT   usRegCoilsStart = REG_COILS_START;
																							 //ȡ���ݷ���      <----    <----
UCHAR    usRegCoilsBuf[REG_COILS_NREGS] 			= {0x11,0x34}; //0001 0001 0011 0100

USHORT   usRegDiscreteStart = REG_DISCRETE_START;
																							 //ȡ���ݷ���      <----    <----
UCHAR    usRegDiscreteBuf[REG_DISCRETE_NREGS] = {0x12,0x00}; //0001 0010 0000 0000

USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS]				= {0xddfd,0xedad,0x000d,0x0356,0x23da,0xffff,0x333b,0x4444};

USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS] 	= {0x147b,0x3f8e,0x147e,0x400e,0x1eb8,0x4055,0x147b,0x408e};



/****************************************************************************
* ��	  �ƣ�eMBRegInputCB 
* ��    �ܣ���ȡ����Ĵ�������Ӧ�������� 04 eMBFuncReadInputRegister
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNRegs: Ҫ��ȡ�ļĴ�������
* ���ڲ�����
* ע	  �⣺��λ�������� ֡��ʽ��: SlaveAddr(1 Byte)+FuncCode(1 Byte)
*								+StartAddrHiByte(1 Byte)+StartAddrLoByte(1 Byte)
*								+LenAddrHiByte(1 Byte)+LenAddrLoByte(1 Byte)+
*								+CRCAddrHiByte(1 Byte)+CRCAddrLoByte(1 Byte)
*							3 ��
****************************************************************************/
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart - 1 );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/****************************************************************************
* ��	  �ƣ�eMBRegHoldingCB 
* ��    �ܣ���Ӧ�������У�06 д���ּĴ��� eMBFuncWriteHoldingRegister 
*													16 д������ּĴ��� eMBFuncWriteMultipleHoldingRegister
*													03 �����ּĴ��� eMBFuncReadHoldingRegister
*													23 ��д������ּĴ��� eMBFuncReadWriteMultipleHoldingRegister
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNRegs: Ҫ��д�ļĴ�������
*						eMode: ������
* ���ڲ�����
* ע	  �⣺4 ��
****************************************************************************/
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;


	if((usAddress >= REG_HOLDING_START)&&\
		((usAddress+usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{ //�˴������ȥ1
		iRegIndex = (int)(usAddress - usRegHoldingStart - 1);
		switch(eMode)
		{                                       
			case MB_REG_READ://�� MB_REG_READ = 0
        while(usNRegs > 0)
				{
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] >> 8);     //�ȶ���λ        
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] & 0xFF);   //�ٶ���λ
          iRegIndex++;
          usNRegs--;					
				}                            
        break;
			case MB_REG_WRITE://д MB_REG_WRITE = 0
				while(usNRegs > 0)
				{         
					usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
          iRegIndex++;
          usNRegs--;
        }				
			}
	}
	else//����
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}

/****************************************************************************
* ��	  �ƣ�eMBRegCoilsCB 
* ��    �ܣ���Ӧ�������У�01 ����Ȧ eMBFuncReadCoils
*													05 д��Ȧ eMBFuncWriteCoil
*													15 д�����Ȧ eMBFuncWriteMultipleCoils
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: ��Ȧ��ַ
*						usNCoils: Ҫ��д����Ȧ����
*						eMode: ������
* ���ڲ�����
* ע	  �⣺��̵��� 
*						0 ��
****************************************************************************/
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	//����״̬
	eMBErrorCode    eStatus = MB_ENOERR;
	//�Ĵ�������
	int16_t iNCoils = (int16_t)usNCoils;
	//�Ĵ���ƫ����
	int             iRegIndex;

	//���Ĵ����Ƿ���ָ����Χ��
	if((usAddress >= REG_COILS_START)&&\
		((usAddress+usNCoils) <= (REG_COILS_START + REG_COILS_NREGS)))
	{
		//Ϊ��Ӧ��Ƭ����ַ��0λ��ʼ���˴������ȥ1
		iRegIndex = (int)(usAddress - usRegCoilsStart - 1);
		switch(eMode)
		{                                       
			case MB_REG_READ://�� MB_REG_READ = 0
        while(iNCoils > 0)
				{	
						*pucRegBuffer++ = xMBUtilGetBits( usRegCoilsBuf, iRegIndex,( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
						iNCoils -= 8;
						iRegIndex += 8;
				}                            
        break;
			case MB_REG_WRITE://д MB_REG_WRITE = 1
				while(iNCoils > 0)
				{         
						xMBUtilSetBits( usRegCoilsBuf, iRegIndex,( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) ,*pucRegBuffer++);
						iNCoils -= 8;
        }				
			}
	}
	else//����
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}
/****************************************************************************
* ��	  �ƣ�eMBRegDiscreteCB 
* ��    �ܣ���ȡ��ɢ�Ĵ�������Ӧ�������У�02 ����ɢ�Ĵ��� eMBFuncReadDiscreteInputs
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNDiscrete: Ҫ��ȡ�ļĴ�������
* ���ڲ�����
* ע	  �⣺1 ��
****************************************************************************/
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;
	int16_t iNDisctete = (int16_t)usNDiscrete;


	if((usAddress >= REG_COILS_START)&&\
		((usAddress+usNDiscrete) <= (REG_DISCRETE_START + REG_DISCRETE_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegDiscreteStart - 1);

    while(iNDisctete > 0)
	 {	
			*pucRegBuffer++ = xMBUtilGetBits( usRegDiscreteBuf, iRegIndex,( uint8_t )( iNDisctete > 8 ? 8 : iNDisctete ) );
			iNDisctete -= 8;
			iRegIndex += 8;
	 }                                
	}
	else//����
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}

