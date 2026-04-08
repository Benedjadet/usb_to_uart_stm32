#include "cli_if.h"
#include "spi1.h"
#include "gpio.h"

static uint8_t g_speed = 0;
static uint8_t g_mode = 0;

static uint8_t g_port_opened = false;

//extern buffer_handle_t to_console;
extern gpio_t gpios[SPI_GPIO_MAX];

uint32_t CLI_IF_SPI_Open(void)
{
	if (g_port_opened)
	{
		return CLI_IF_ERR_SPI_OPENED;
	}

	spi_init(g_speed, g_mode);
	g_port_opened = true;
	return CLI_IF_OK;
}

uint32_t CLI_IF_SPI_Close(void)
{
	if (!g_port_opened)
	{
		return CLI_IF_ERR_SPI_CLOSED;
	}

	spi_deinit();
	g_port_opened = false;
	return CLI_IF_OK;
}

uint32_t CLI_IF_SPI_Speed(uint8_t speed)
{
	if (g_port_opened)
	{
		return CLI_IF_ERR_SPI_OPENED;
	}

	if (speed > CLI_IF_SPI_SPEED_MAX)
	{
		return CLI_IF_ERR_VALUE;
	}

	g_speed = speed;
	return CLI_IF_OK;
}

uint32_t CLI_IF_SPI_Mode(uint8_t mode)
{
	if (g_port_opened)
	{
		return CLI_IF_ERR_SPI_OPENED;
	}

	if (mode > CLI_IF_SPI_MODE_MAX)
	{
		return CLI_IF_ERR_VALUE;
	}

	g_mode = mode;
	return CLI_IF_OK;
}

uint32_t CLI_IF_SPI_Transmit(uint8_t *txdata, size_t len)
{
	if (!g_port_opened)
	{
		return CLI_IF_ERR_SPI_CLOSED;
	}

	if (HAL_SPI_Transmit(&hspi1, txdata, len, 100) != HAL_OK)
	{
		return CLI_IF_ERR_SPI_HARDWARE;
	}


	return CLI_IF_OK;
}

uint32_t CLI_IF_SPI_Tranceive(uint8_t *txdata, uint8_t *rxdata, size_t len)
{
	if (!g_port_opened)
	{
		return CLI_IF_ERR_SPI_CLOSED;
	}

	if (HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, len, 100) != HAL_OK)
	{
		return CLI_IF_ERR_SPI_HARDWARE;
	}
	return CLI_IF_OK;
}

uint32_t CLI_IF_GPIO_Read(uint8_t *gpio)
{
	uint8_t new_gpio = 0;

	for (uint8_t i = 0; i < SPI_GPIO_MAX; i++)
	{
		GPIO_PinState state = HAL_GPIO_ReadPin(gpios[i].port, gpios[i].pin);
		if (state == GPIO_PIN_SET)
		{
			new_gpio |= (1 << i);
		}
	}

	*gpio = new_gpio;

	return CLI_IF_OK;
}

uint32_t CLI_IF_GPIO_Write(uint8_t gpio)
{
	if (gpio > CLI_IF_GPIO_MAX)
	{
		return CLI_IF_ERR_VALUE;
	}

	for (uint32_t i = 0; i < SPI_GPIO_MAX; i++)
	{
		GPIO_PinState state = GPIO_PIN_RESET;
		if (gpio & (1 << i))
		{
			state = GPIO_PIN_SET;
		}
		HAL_GPIO_WritePin(gpios[i].port, gpios[i].pin, state);
	}

	return CLI_IF_OK;
}
