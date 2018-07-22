#include "DHTxx.h"

void DHTxx_Init(void)
{
	OW_Init();
}

/**
  * @brief  ��ȡDHTxx���� 40λ����
  * @param  
*     @arg sensortype :DHT21 DHT11 (ָ����ͬ��������ʱʱ�䣩
  *     @arg data ����40λ���ݵ�ָ��
  * @retval �Ƿ��ȡ�ɹ�
  * @note None
  */
DHTxx_StatusTypeDef ReadDHTxx(u8 sensortype,u8* data)
{
	u8 sum;
	if(OW_START(sensortype) == OW_SUCCESS)
	{
		*data  		= OW_ReadByte();
		*(data+1) = OW_ReadByte();
		*(data+2) = OW_ReadByte();
		*(data+3) = OW_ReadByte();
		*(data+4) = OW_ReadByte();
	
		sum = *data + *(data+1) + *(data+2) + *(data+3);
	}
	
	if(sum == *(data+4))
		return DHTxx_SUCCESS;
	else 
		return DHTxx_ERROR;
	
}
//��ȡʪ��
float Get_DHT21_Humidity(u8 Hdata,u8 Ldata)
{
	return 1.0*((Hdata << 8) | Ldata)/10;

}
//��ȡ�¶�
float Get_DHT21_Temperature(u8 Hdata,u8 Ldata)
{
	return 1.0*((Hdata << 8) | Ldata)/10;
}
//��ȡʪ��

float Get_DHT11_Humidity(u8 Hdata,u8 Ldata)
{
	return Hdata;

}
//��ȡ�¶�
float Get_DHT11_Temperature(u8 Hdata,u8 Ldata)
{
	return Hdata;
}
