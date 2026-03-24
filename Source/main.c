
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "buffer.h"

#include "uart1.h"
#include "rcc.h"
#include "gpio.h"

// UART TX buffer size.
#define UART_TX_BUF_SIZE (32767)

// UART RX buffer size.
#define UART_RX_BUF_SIZE (32767)

// UART TX buffer.
uint8_t uart_tx_data[UART_TX_BUF_SIZE] = {0};

// Ring TX buffer handler.
buffer_handle_t uart_tx_buf = {0};

// UART TX busy flag.
boolean_t uart_tx_busy = false;

// UART RX buffer.
uint8_t uart_rx_data[UART_RX_BUF_SIZE] = {0};

// Ring RX buffer handler.
buffer_handle_t uart_rx_buf = {0};

// UART port configuration.
USBD_CDC_LineCodingTypeDef uart_linecoding = {
	.bitrate = 9600,
	.format = 0,	 // 1 stop bit
	.paritytype = 0, // none
	.datatype = 8};

void HAL_MspInit(void);

uint8_t CDC_SetLineCoding_CB(USBD_CDC_LineCodingTypeDef linecoding)
{
	uart_linecoding.bitrate = linecoding.bitrate;
	uart_linecoding.paritytype = linecoding.paritytype;
	uart_linecoding.format = linecoding.format;
	uart_init(linecoding.bitrate, linecoding.paritytype, linecoding.format);
	return 0;
}

USBD_CDC_LineCodingTypeDef CDC_GetLineCoding_CB(void)
{
	return uart_linecoding;
}

void CDC_Receive_FC_CB(uint8_t *Buf, uint16_t Len)
{
	uint8_t res = buffer_append(&uart_tx_buf, Buf, Len);

	if (res == BUFFER_NOINITED || res == BUFFER_NULL_HANDLE)
	{
		// Неверное использование буффера.
		Error_Handler();
	}

	if (res == BUFFER_OVERRIDE) {
		// Размер посылки превышает размер буфера приема. Непонятно что делат.
	}
}

void UART_IdleCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
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
		HAL_UART_Receive_DMA(&huart1, uart_rx_data, UART_RX_BUF_SIZE);
	}
}
/**
 * @brief  USART Transmit Complete Callback.
 * @param huart USART Handler.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		uart_tx_busy = false;
	}
}

/**
 * @brief DMA Buffer Start Position.
 * @param handle Ring Buffer Handler.
 * @return Start Position Index.
 */
uint32_t dma_istart_cb(void *handle)
{
	return (uint32_t)(UART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx));
}

void buffer_test(void)
{
	uint8_t buffer[32] = {0};

	buffer_handle_t hbuf = {0};
	buffer_init(&hbuf, buffer, sizeof(buffer), NULL, NULL);

	// Проверка перехода через край.
	// -------------------------------------------------------------------------

	// Записываем весь буффер.
	if (buffer_append(&hbuf, (uint8_t *)"11111111111111111111111111111111", 24) != BUFFER_OK)
	{
		// Ошибка при записи буффера.
		Error_Handler();
	}

	// Считываем половину.
	uint8_t data_buf1[128] = {0};
	size_t data_size = 16;
	if (buffer_get(&hbuf, data_buf1, &data_size) != BUFFER_OK)
	{
		// Ошибка при чтении буфера.
		Error_Handler();
	}

	// Записываем четверть.
	if (buffer_append(&hbuf, (uint8_t *)"2222222222", 10) != BUFFER_OK)
	{
		// Ошибка при записи через границу.
		Error_Handler();
	}

	if (hbuf.crossed_border == 0)
	{
		// Не установлен флаг перехода через границу
		Error_Handler();
	}

	// Считываем все.
	data_size = 32;
	uint8_t data_buf2[32] = {0};
	if (buffer_get(&hbuf, data_buf2, &data_size)!= BUFFER_OK)
	{
		// Ошибка чтения всего буфера.
		Error_Handler();
	}

	if (hbuf.crossed_border == 1)
	{
		// Не сброшен флаг прехода через границу.
		Error_Handler();
	}

	if (data_size != 18)
	{
		// Неверно посчитано содержимое буфера.
		Error_Handler();
	}

	// Очищаем буфер.
	if (buffer_flush(&hbuf) != BUFFER_OK)
	{
		// Ошибка очистки буфера.
		Error_Handler();
	}

	// Проверяем переполнение буфера.
	if (buffer_append(&hbuf, (uint8_t *)"11111111111111111111111111111111", 32) != BUFFER_OK)
	{
		// Ошибка записи буфера.
		Error_Handler();
	}

	uint8_t res = buffer_append(&hbuf, (uint8_t *)"11111111111111111111111111111111", 32);

	if (res != BUFFER_OVERRIDE)
	{
		// Функция не вернула флаг переполнения.
		Error_Handler();
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
	uart_init(uart_linecoding.bitrate, uart_linecoding.paritytype, uart_linecoding.format);

	buffer_init(&uart_tx_buf, uart_tx_data, sizeof(uart_tx_data), NULL, NULL);
	buffer_init(&uart_rx_buf, uart_rx_data, sizeof(uart_rx_data), NULL, dma_istart_cb);

	buffer_test();

	usb_device_init();

	uint32_t ticks = 0;

	size_t tx_len = 0;
	uint8_t tx_buf[256] = {0};

	size_t rx_len = 0;
	uint8_t rx_buf[256] = {0};

	// Запускаем прием данных через DMA.
	HAL_UART_Receive_DMA(&huart1, uart_rx_data, UART_RX_BUF_SIZE);

	// Включаем прерывание по бездействию линии.
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

	boolean_t usb_busy_flag = false;

	while (1)
	{
		// Отправка данных в UART.
		if (!uart_tx_busy)
		{
			if (buffer_len(&uart_tx_buf, &tx_len) != BUFFER_OK)
			{
				Error_Handler();
			}

			if (tx_len > 256)
			{
				tx_len = 256;
			}
			if (tx_len != 0)
			{
				if (buffer_get(&uart_tx_buf, tx_buf, &tx_len) != BUFFER_OK)
				{
					Error_Handler();
				}

				HAL_UART_Transmit_DMA(&huart1, tx_buf, tx_len);
				uart_tx_busy = true;
			}
		}

		if (!usb_busy_flag)
		{
			if (buffer_len(&uart_rx_buf, &rx_len) != BUFFER_OK)
			{
				Error_Handler();
			}

			if (rx_len > CDC_DATA_FS_MAX_PACKET_SIZE)
			{
				rx_len = CDC_DATA_FS_MAX_PACKET_SIZE;
			}

			if (rx_len != 0)
			{
				if (buffer_get(&uart_rx_buf, rx_buf, &rx_len) != BUFFER_OK)
				{
					Error_Handler();
				}

				if (CDC_Transmit_FS(rx_buf, (uint16_t)rx_len) != USBD_OK)
				{
					usb_busy_flag = true;
				}
			}
		}
		else
		{
			if (CDC_Transmit_FS(rx_buf, (uint16_t)rx_len) == USBD_OK)
			{
				usb_busy_flag = false;
			}
		}

		// Светодиодная индикация.
		ticks++;
		if (ticks >= 1000)
		{
			ticks = 0;
			gpio_debug_led_toggle();
		}

		HAL_Delay(1);
	}
}

void HAL_MspInit(void)
{
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	// JTAG-DP Disabled.
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
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
