
#include "uart1.h"


UART_HandleTypeDef huart1;

/**
 * @brief UART MSP Initialization
 * This function configures the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (huart->Instance == USART1)
	{
		/* USER CODE BEGIN USART1_MspInit 0 */

		/* USER CODE END USART1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
}

/**
 * @brief UART MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

		/* USART1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART1_IRQn);
	}
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
void uart_init(uint32_t baudrate, uint8_t parity, uint8_t stopbits)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = baudrate;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;

	// Stop-bits.
	if ((stopbits == 0) || (stopbits == 1))
	{
		huart1.Init.StopBits = UART_STOPBITS_1;
	}
	else if (stopbits == 2)
	{
		huart1.Init.StopBits = UART_STOPBITS_2;
	}
	else
	{
		huart1.Init.StopBits = UART_STOPBITS_1;
	}

	// Parity.
	if (parity == 0)
	{
		huart1.Init.Parity = UART_PARITY_NONE;
	}
	else if (parity == 1)
	{
		huart1.Init.Parity = UART_PARITY_ODD;
	}
	else if (parity == 2)
	{
		huart1.Init.Parity = UART_PARITY_EVEN;
	}
	else
	{
		huart1.Init.Parity = UART_PARITY_NONE;
	}

	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}


void uart_reset_ore(UART_HandleTypeDef *huart)
{
	uint32_t sr_flags = huart->Instance->SR; // Считали SR.

	// При подаче питания CPU ADC запускается быстрее и начинает слать в еще несконфигурированный UART CPU CAN,
	// Что приводит к возникновению ошибки USART_SR_ORE.
	// По неизвестной мне причине HAL не обрабатывет это исключение, к тому же он сбрасывает флаг RXNEIE
	// что приводит к тому что не генерируется HAL_UART_ErrorCallback().
	// Нужно сбросить аварии чтобы не зависло прерывание.
	if ((sr_flags & (USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE)) != RESET)
	{
		uint32_t temp_dr = huart->Instance->DR; // Считали DR чтобы сбросить аварии.
		UNUSED(temp_dr);
	}
}




/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}
