#ifndef GPIO_CONF_H
#define GPIO_CONF_H



#define DEBUG_LED_PORT (GPIOC)
#define DEBUG_LED_PIN (GPIO_PIN_13)


#define SPI_SCK_PORT (GPIOA)
#define SPI_SCK_PIN (GPIO_PIN_5)

#define SPI_MOSI_PORT (GPIOA)
#define SPI_MOSI_PIN (GPIO_PIN_7)

#define SPI_MISO_PORT (GPIOA)
#define SPI_MISO_PIN (GPIO_PIN_6)


#define SPI_CS1_PORT (GPIOB)
#define SPI_CS1_PIN (GPIO_PIN_0)

#define SPI_CS2_PORT (GPIOB)
#define SPI_CS2_PIN (GPIO_PIN_1)

#define SPI_CS3_PORT (GPIOB)
#define SPI_CS3_PIN (GPIO_PIN_2)

#define SPI_CS4_PORT (GPIOB)
#define SPI_CS4_PIN (GPIO_PIN_3)

#define SPI_CS5_PORT (GPIOB)
#define SPI_CS5_PIN (GPIO_PIN_4)



// UART TX buffer size.
#define SPI_TX_BUF_SIZE (256)

// UART RX buffer size.
#define SPI_RX_BUF_SIZE (256)


#define SPI_GPIO_MAX (5)

#endif /* GPIO_CONF_H */