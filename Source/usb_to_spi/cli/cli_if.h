#ifndef CLI_IF_H
#define CLI_IF_H

#ifdef __cplusplus
extern "C"
{
#endif

#define CLI_IF_OK (0UL)
#define CLI_IF_ERR_SPI_CLOSED (1UL)
#define CLI_IF_ERR_SPI_OPENED (2UL)
#define CLI_IF_ERR_SPI_HARDWARE (3UL)
#define CLI_IF_ERR_VALUE (4UL)

#define CLI_IF_GPIO_MAX (0x1FUL)

#define CLI_IF_SPI_SPEED_MAX (7UL)
#define CLI_IF_SPI_MODE_MAX (3UL)

	uint32_t CLI_IF_SPI_Open(void);
	uint32_t CLI_IF_SPI_Close(void);
	uint32_t CLI_IF_SPI_Speed(uint8_t speed);
	uint32_t CLI_IF_SPI_Mode(uint8_t mode);
	uint32_t CLI_IF_SPI_Transmit(uint8_t *txdata, size_t len);
	uint32_t CLI_IF_SPI_Tranceive(uint8_t *txdata, uint8_t *rxdata, size_t len);

	uint32_t CLI_IF_GPIO_Read(uint8_t *gpio);
	uint32_t CLI_IF_GPIO_Write(uint8_t gpio);
	uint32_t CLI_IF_GPIO_ModeSet(uint8_t mode);
	uint32_t CLI_IF_GPIO_ModeGet(uint8_t *mode);

#ifdef __cplusplus
}
#endif

#endif /* CLI_IF_H */
