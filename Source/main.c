
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "buffer.h"

UART_HandleTypeDef huart1;

uint8_t uart_tx_data[2048] = {0};
buffer_handle_t uart_tx_buf = {0};

uint8_t uart_rx_data[2048] = {0};
uint8_t uart_rx_byte = 0;
buffer_handle_t uart_rx_buf = {0};

static void rcc_init(void);
static void gpio_init(void);
static void uart_init(USBD_CDC_LineCodingTypeDef linecoding);
static void uart_reset_ore(UART_HandleTypeDef *huart);

USBD_CDC_LineCodingTypeDef uart_linecoding = {
	.bitrate = 9600,
	.format = 0,	 // 1 stop bit
	.paritytype = 0, // none
	.datatype = 8};

uint8_t CDC_SetLineCoding_CB(USBD_CDC_LineCodingTypeDef linecoding)
{
	uart_linecoding.bitrate = linecoding.bitrate;
	uart_linecoding.format = linecoding.format;
	uart_linecoding.paritytype = linecoding.paritytype;
	uart_linecoding.datatype = 8;
	uart_init(uart_linecoding);
	return 0;
}

USBD_CDC_LineCodingTypeDef CDC_GetLineCoding_CB(void)
{
	return uart_linecoding;
}

void CDC_Receive_FC_CB(uint8_t *Buf, uint16_t Len)
{
	buffer_append(&uart_tx_buf, Buf, Len);
}

/**
 * @brief USART Receive Complite Callback.
 * @param huart USART Handler.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		buffer_append(&uart_rx_buf, &uart_rx_byte, 1);

		// ПЕРЕЗАПУСК ПРИЁМА (обязательно!)
		HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
	}
}

/**
 * @brief USART Error Callback.
 * @param huart USART Handler.
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		uart_reset_ore(huart);
	}
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	HAL_Init();

	rcc_init();
	gpio_init();
	uart_init(uart_linecoding);

	buffer_init(&uart_tx_buf, uart_tx_data, sizeof(uart_tx_data));
	buffer_init(&uart_rx_buf, uart_rx_data, sizeof(uart_rx_data));

	usb_device_init();

	uint32_t ticks = 0;

	size_t tx_len = 0;
	uint8_t tx_buf[256] = {0};

	size_t rx_len = 0;
	uint8_t rx_buf[256] = {0};

	HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);

	while (1)
	{
		tx_len = buffer_len(&uart_tx_buf);

		if (tx_len != 0)
		{
			buffer_get(&uart_tx_buf, tx_buf, &tx_len);
			HAL_UART_Transmit_IT(&huart1, tx_buf, tx_len);
		}

		rx_len = buffer_len(&uart_rx_buf);

		if (rx_len != 0)
		{
			buffer_get(&uart_rx_buf, rx_buf, &rx_len);
			CDC_Transmit_FS(rx_buf, (uint16_t)rx_len);
		}

		ticks++;
		if (ticks >= 1000)
		{
			ticks = 0;
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}

		HAL_Delay(1);
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void rcc_init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/**
	 * Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 * HSE_VALUE=160000000
	 * PLLCLK = HSE_VALUE/2 = 8000000
	 * SYSCLK = PLLCLK*PLLMUL = 8000000*9 = 72000000
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/**
	 * Initializes the CPU, AHB and APB buses clocks
	 * AHB_CLK = SYSCLK/1 = 72000000
	 * APB1_CLK = AHB_CLK/2 = 36000000
	 * APB2_CLK = AHB_CLK/1 = 72000000
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}

	/**
	 * Initializes the USB buses clocks
	 * USB_CLK = PLLCLK/1.5 = 72000000/1.5 = 48000000
	 */
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void uart_init(USBD_CDC_LineCodingTypeDef linecoding)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = linecoding.bitrate;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;

	// Stop-bits.
	if ((linecoding.format == 0) || (linecoding.format == 1))
	{
		huart1.Init.StopBits = UART_STOPBITS_1;
	}
	else if (linecoding.format == 2)
	{
		huart1.Init.StopBits = UART_STOPBITS_2;
	}
	else
	{
		huart1.Init.StopBits = UART_STOPBITS_1;
	}

	// Parity.
	if (linecoding.paritytype == 0)
	{
		huart1.Init.Parity = UART_PARITY_NONE;
	}
	else if (linecoding.paritytype == 1)
	{
		huart1.Init.Parity = UART_PARITY_ODD;
	}
	else if (linecoding.paritytype == 2)
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
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void gpio_init(void)
{

	// GPIO Ports Clock Enable.
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
