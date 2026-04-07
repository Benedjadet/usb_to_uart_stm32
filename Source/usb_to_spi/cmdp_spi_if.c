
#include "cmdp_spi.h"
#include "usbd_cdc_if.h"
#include "spi1.h"
#include "buffer.h"
#include "gpio.h"
#include "stdio.h"

static uint8_t g_speed = 0;
static uint8_t g_mode = 0;

static uint8_t g_port_state = 0;

extern buffer_handle_t to_console;
extern gpio_t gpios[SPI_GPIO_MAX];

static void __print_str(const char *str);

int32_t cmdp_spi_open(void)
{
	spi_init(g_speed, g_mode);
	g_port_state = 1;
	return 0;
}

int32_t cmdp_spi_close(void)
{
	spi_deinit();
	g_port_state = 0;
	return 0;
}

int32_t cmdp_spi_speed(uint8_t speed)
{
	if (g_port_state == 1)
	{
		__print_str("Can't set speed due to port opened!");
		return -1;
	}

	if ((speed < '0') || (speed > '7') )
	{
		__print_str("Incorrect speed value!");
		return -1;
	}

	g_speed = speed - '0';
	return 0;
}

int32_t cmdp_spi_mode(uint8_t mode)
{
	if (g_port_state == 1)
	{
		__print_str("Can't set speed due to port opened!");
		return -1;
	}

	if ((mode < '0') || (mode > '3'))
	{
		__print_str("Incorrect mode value!");
		return -1;
	}

	g_mode = mode - '0';
	return 0;
}

int32_t cmdp_spi_gpio(uint8_t *data, uint32_t len)
{

	if (len != 2)
	{
		__print_str("Incorrect format!");
		return -1;
	}


	ascii_to_hex_int(data, len);


	if ((data[0] >= 16) || (data[1] >= 16))
	{
		__print_str("Incorrect value!");
		return -1;
	}

	uint8_t gpio = (data[0] << 4) | (data[1]);

	for (uint32_t i = 0; i < SPI_GPIO_MAX; i++)
	{
		GPIO_PinState state = GPIO_PIN_RESET;
		if (gpio & (1 << i))
		{
			state = GPIO_PIN_SET;
		}
		HAL_GPIO_WritePin(gpios[i].port, gpios[i].pin, state);
	}

	return 0;
}

int32_t cmdp_spi_help(void)
{
	__print_str("Helper\n\r");
	__print_str("\tO\tOpen channel.\n\r");
	__print_str("\tC\tClose channel.\n\r");
	__print_str("\tS0\tSet Bitrate 281.25 kHz.\n\r");
	__print_str("\tS1\tSet Bitrate 562.5 kHz.\n\r");
	__print_str("\tS2\tSet Bitrate 1.125 MHz.\n\r");
	__print_str("\tS3\tSet Bitrate 2.25 MHz.\n\r");
	__print_str("\tS4\tSet Bitrate 4.5 MHz.\n\r");
	__print_str("\tS5\tSet Bitrate 9.0 MHz.\n\r");
	__print_str("\tS6\tSet Bitrate 18.0 MHz.\n\r");
	__print_str("\tS7\tSet Bitrate 36.0 MHz.\n\r");
	__print_str("\tM0\tSet Mode CPOL=0 CPHA=0.\n\r");
	__print_str("\tM1\tSet Mode CPOL=0 CPHA=1.\n\r");
	__print_str("\tM2\tSet Mode CPOL=1 CPHA=0.\n\r");
	__print_str("\tM3\tSet Mode CPOL=1 CPHA=1.\n\r");
	__print_str("\tg<xx>\tSet GPIO x=0..f.\n\r");
	__print_str("\tV|v\tVersion.\n\r");
	__print_str("\tT|t <data>\tTransmit data in hex format. Example T 01 02 03 04.\n\r");
	__print_str("\tH|h\tThis page.\n\r");

	return 0;
}

int32_t cmdp_version(void)
{
	__print_str("1.0.0\n\r");
	return 0;
}

int32_t cmdp_spi_transmit(uint8_t *data, uint32_t len)
{

	if (g_port_state == 0)
	{
		__print_str("Port not opened!\n\r");
		return -1;
	}

	uint8_t txdata[128] = {0};

	for (uint32_t i = 0; i < len; i++)
	{

	}

	uint8_t rxdata[128] = {0};

	if (HAL_SPI_TransmitReceive(&hspi1, data, rxdata, len, HAL_MAX_DELAY) != HAL_OK)
	{
		__print_str("Internal error!\n\r");
		return -1;
	}

	// CDC_Transmit_FS(rxdata, len);
	buffer_append(&to_console, rxdata, len);

	return 0;
}



int32_t cmdp_spi_receive(uint8_t* data, uint32_t len)
{
	printf("Hello world!\n\r");
	return 0;
}


int32_t cmdp_spi_invalid(uint8_t cmd)
{
	//__print_str("Unknown command ");
	//char str[7] = {'\'', cmd, '\'', '!', '\n', '\r', '\0'};
	//__print_str(str);

	printf("Unknown command \'%c\'!\n\r", (char)cmd);


	return 0;
}

static void __print_str(const char *str)
{
	// Считаем длину.

	size_t counter = 0;

	while (*(str + counter) != '\0')
	{
		counter++;
	}

	buffer_append(&to_console, (uint8_t *)str, counter);
}
