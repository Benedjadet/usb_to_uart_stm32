
#include "uart1.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

void MX_DMA_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

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

		// --- DMA TX ---
		hdma_usart1_tx.Instance = DMA1_Channel4;
		hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_tx.Init.Mode = DMA_NORMAL;
		hdma_usart1_tx.Init.Priority = DMA_PRIORITY_HIGH;

		if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(huart, hdmatx, hdma_usart1_tx);

		// --- DMA RX ---
		hdma_usart1_rx.Instance = DMA1_Channel5;
		hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
		hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;

		if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
		{
			Error_Handler();
		}

		// 🔗 Связываем DMA с UART
		__HAL_LINKDMA(huart, hdmarx, hdma_usart1_rx);

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

void MX_DMA_Init(void)
{
	// 1. Включаем тактирование DMA1
	__HAL_RCC_DMA1_CLK_ENABLE();

	// 2. Настраиваем NVIC для DMA (опционально)
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

	HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
void uart_init(uint32_t baudrate, uint8_t parity, uint8_t stopbits)
{

	MX_DMA_Init();
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

	if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);

		// сюда обработку DMA
		UART_IdleCallback(&huart1);
	}

	HAL_UART_IRQHandler(&huart1);
}

void DMA1_Channel4_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void DMA1_Channel5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_usart1_rx);
}
