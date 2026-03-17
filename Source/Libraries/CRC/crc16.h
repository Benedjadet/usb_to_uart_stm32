#ifndef __CRC16_H__
#define __CRC16_H__

#ifdef __cplusplus
extern "C" {
#endif


uint16_t crc16(uint8_t *Data, uint8_t Size);
uint16_t crc16_true(uint8_t* Data, uint8_t Size, uint16_t Poly, uint16_t Init, boolean_t RefIn, boolean_t RefOut, uint16_t XorOut);
uint16_t crc16_check(uint16_t Poly, uint16_t Init, boolean_t RefIn, boolean_t RefOut, uint16_t XorOut);


#ifdef __cplusplus
}
#endif

#endif /* __CRC16_H__ */