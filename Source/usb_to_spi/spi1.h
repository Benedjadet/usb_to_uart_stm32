#ifndef __SPI1_H__
#define __SPI1_H__


#ifdef __cplusplus
extern "C" {
#endif

extern SPI_HandleTypeDef hspi1;
extern void Error_Handler();
void spi_init(uint8_t speed, uint8_t mode);
void spi_deinit(void);


#ifdef __cplusplus
}
#endif





#endif /* __SPI1_H__ */