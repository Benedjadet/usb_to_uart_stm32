#include "spi1.h"

SPI_HandleTypeDef hspi1;

void spi_init(uint8_t speed, uint8_t mode)
{
	// SPI1 parameter configuration.
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;

	// Mode.
	if (mode == 0)
	{
		hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	}
	else if (mode == 1)
	{
		hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	}
	else if (mode == 2)
	{
		hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
		hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	}
	else if (mode == 3)
	{
		hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
		hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	}

	hspi1.Init.NSS = SPI_NSS_SOFT;

	// Speed.
	if (speed == 0)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	}
	else if (speed == 1)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	}
	else if (speed == 2)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	}
	else if (speed == 3)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	}
	else if (speed == 4)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	}
	else if (speed == 5)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	}
	else if (speed == 6)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	}
	else if (speed == 7)
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	}

	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}

void spi_deinit(){

	HAL_SPI_DeInit(&hspi1);
}



void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	if (hspi->Instance == SPI1)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		__HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		// SPI1 SCK.
		GPIO_InitStruct.Pin = SPI_SCK_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SPI_SCK_PORT, &GPIO_InitStruct);

		// SPI1 MOSI.
		GPIO_InitStruct.Pin = SPI_MOSI_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);

		// SPI1 MISO.
		GPIO_InitStruct.Pin = SPI_MISO_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);
	}
}



void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI1)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		// SPI1 SCK.
		GPIO_InitStruct.Pin = SPI_SCK_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SPI_SCK_PORT, &GPIO_InitStruct);

		// SPI1 MOSI.
		GPIO_InitStruct.Pin = SPI_MOSI_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);

		// SPI1 MISO.
		GPIO_InitStruct.Pin = SPI_MISO_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);
	}
}