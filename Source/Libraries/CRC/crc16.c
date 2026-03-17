
#include "crc16.h"

/**
 * @brief Функция вычисления CRC16.
 * @param Data Указатель на самив данных.
 * @param Size Размер масива данных в байтах.
 * @return Вычисленное значение CRC.
 */
uint16_t crc16(uint8_t* Data, uint8_t Size)
{
	// uint16_t flag;
	// uint16_t crc = 0xFFFF;
	// for (uint8_t i = 0; i < Size; i++)
	// {
	// 	crc ^= Data[i];
	// 	for (uint8_t j = 0; j < 8; ++j)
	// 	{
	// 		flag = crc & 0x0001;
	// 		crc >>= 1;
	// 		if (flag)
	// 		{
	// 			crc ^= 0xA001;
	// 		}
	// 	}
	// }

	// uint16_t temp2 = crc >> 8;
	// crc = (crc << 8) | temp2;
	return crc16_true(Data, Size, 0x8005, 0xFFFF, true, true, 0x0000);
}

/**
 * @brief Функция вычисления CRC16.
 * @param Data Указатель на масив данных.
 * @param Size Размер масива данных в байтах.
 * @param Poly Полином.
 * @param Init Начальное значение.
 * @param RefIn Изменить порядок бит перед началом обработки.
 * @param RefOut Изменить порядок бит перед передачей на XorOut этап.
 * @param XorOut Значение с которым нужно XOR'ить результирующее значение перед возвращением
 * @return Вычисленное значение CRC.
 */
uint16_t crc16_true(uint8_t* Data, uint8_t Size, uint16_t Poly, uint16_t Init, boolean_t RefIn, boolean_t RefOut, uint16_t XorOut)
{
	uint16_t CrcTrigger;		// Индикатор что пора применять полином.
	uint16_t CrcValue = Init;	//

    for (uint8_t ByteNum = 0; ByteNum < Size; ByteNum++)
	{
	    uint8_t ByteValue = 0;
	    if(!RefIn)
        {
    	    ByteValue = Data[ByteNum];
        }
        else
        {
            for(int8_t i = 0; i < 8; i++)
    	    {
    	        ByteValue <<= 1;
    	        if(Data[ByteNum] & (1<<i))
    	        {
    	            ByteValue |= 0x01;
    	        }
    	    }
        }

		CrcValue ^= ((uint16_t)ByteValue << 8);

		for (uint8_t BitNum = 0; BitNum < 8; BitNum++)
		{
			CrcTrigger = CrcValue & 0x8000;
			CrcValue <<= 1;
			if (CrcTrigger)
			{
				CrcValue ^= Poly;
			}
		}
	}

	//uint16_t CrcValueMsb = CrcValue >> 8;
    //CrcValue = (CrcValueMsb << 8) | CrcValue;

	if(!RefOut)
	{
    	return CrcValue^XorOut;
	}
	else
	{
	    uint16_t temp = 0;
	    for(uint8_t i = 0; i < 16; i++)
	    {
	        temp <<= 1;
	        if(CrcValue & (1 << i))
	        {
	            temp |= 0x0001;
	        }
	    }
	    return temp^XorOut;
	}
}


uint16_t crc16_check(uint16_t Poly, uint16_t Init, boolean_t RefIn, boolean_t RefOut, uint16_t XorOut)
{
    const char* CheckStr = "123456789";
    return crc16_true((uint8_t*)CheckStr, 9, Poly, Init, RefIn, RefOut, XorOut);
}