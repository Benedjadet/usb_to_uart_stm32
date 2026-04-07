#ifndef _SLCAN_H
#define _SLCAN_H


// Maximum rx buffer len
#define SLCAN_MTU 138 + 1 + 16 // canfd 64 frame plus \r plus some padding
#define SLCAN_STD_ID_LEN 3
#define SLCAN_EXT_ID_LEN 8


// Prototypes
//int32_t cmdp_generate(uint8_t *buf, FDCAN_RxHeaderTypeDef *frame_header, uint8_t* frame_data);
int32_t cmdp_spi_parse(uint8_t *buf, uint8_t len);


extern int32_t cmdp_spi_open(void);
extern int32_t cmdp_spi_close(void);
extern int32_t cmdp_spi_speed(uint8_t speed);
extern int32_t cmdp_spi_mode(uint8_t mode);
extern int32_t cmdp_spi_gpio(uint8_t* data, uint32_t len);
extern int32_t cmdp_spi_transmit(uint8_t* data, uint32_t len);
extern int32_t cmdp_spi_receive(uint8_t* data, uint32_t len);
extern int32_t cmdp_version(void);
extern int32_t cmdp_spi_invalid(uint8_t cmd);
extern int32_t cmdp_spi_help(void);

void ascii_to_hex_int(uint8_t* buf, uint32_t len);


#endif // _SLCAN_H
