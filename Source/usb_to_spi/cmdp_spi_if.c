
#include "cmdp_spi.h"
#include "usbd_cdc_if.h"
#include "spi1.h"
#include "buffer.h"
#include "gpio.h"


static uint8_t g_speed = 0;
static uint8_t g_mode = 0;

static uint8_t g_port_state = 0;



extern buffer_handle_t to_console;
extern gpio_t gpios[SPI_GPIO_MAX];


static void __print_str(const char* str);


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
	}

	if (speed > 7)
	{
		__print_str("Incorrect speed value!");
	}

	g_speed = speed;
	return 0;
}

int32_t cmdp_spi_mode(uint8_t mode)
{
	if (g_port_state == 1)
	{
		__print_str("Can't set speed due to port opened!");
	}

	if (mode > 3)
	{
		__print_str("Incorrect mode value!");
	}

	g_mode = mode;
	return 0;
}

int32_t cmdp_spi_gpio(uint8_t gpio)
{
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
	__print_str("    O Open port\n\r");
	__print_str("    C Close port\n\r");

	return 0;
}


int32_t cmdp_version(void)
{
	__print_str("1.0.0\n\r");
	return 0;
}


int32_t cmdp_spi_transmit(uint8_t* data, uint32_t len)
{

	if (g_port_state == 0)
	{
		__print_str("Port not opened!\n\r");
		return -1;
	}

	uint8_t rxdata [128] = {0};

	if (HAL_SPI_TransmitReceive(&hspi1, data, rxdata, len, HAL_MAX_DELAY) != HAL_OK)
	{
		return -1;
	}

	//CDC_Transmit_FS(rxdata, len);
	buffer_append(&to_console, rxdata, len);

	return 0;
}


int32_t cmdp_spi_invalid(uint8_t cmd)
{
	__print_str("Unknown command ");
	char str[7] = {'\'', cmd, '\'', '!', '\n', '\r', '\0'};
	__print_str(str);

	return 0;
}



static void __print_str(const char* str)
{
	// Считаем длину.

	size_t counter = 0;

	while (*(str + counter) != '\0')
	{
		counter++;
	}


	buffer_append(&to_console, (uint8_t*)str, counter);
}


