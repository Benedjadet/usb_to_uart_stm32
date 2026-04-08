
#include "usb_to_spi_main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "buffer.h"

#include "spi1.h"
#include "rcc.h"
#include "gpio.h"
#include "stdio.h"
#include "cli.h"

// UART TX buffer.
uint8_t spi_tx_data[SPI_TX_BUF_SIZE] = {0};

// UART TX busy flag.
boolean_t spi_tx_busy = false;

// UART RX buffer.
uint8_t spi_rx_data[SPI_RX_BUF_SIZE] = {0};

uint8_t from_console_buffer[2048] = {0};

buffer_handle_t from_console = {0};

uint8_t to_console_buffer[2048] = {0};

buffer_handle_t to_console = {0};

gpio_t gpios[SPI_GPIO_MAX] = {
	{SPI_CS1_PORT, SPI_CS1_PIN},
	{SPI_CS2_PORT, SPI_CS2_PIN},
	{SPI_CS3_PORT, SPI_CS3_PIN},
	{SPI_CS4_PORT, SPI_CS4_PIN},
	{SPI_CS5_PORT, SPI_CS5_PIN},
};

uint8_t connected = 0;

// Заглушка для USB.
USBD_CDC_LineCodingTypeDef g_linecoding = {
	.bitrate = 115200,
	.format = 0,	 // 1 stop bit
	.paritytype = 0, // none
	.datatype = 8};

/**
 * @brief Инициализация GPIO.
 */
void cs_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	for (uint32_t i = 0; i < SPI_GPIO_MAX; i++)
	{
		GPIO_InitStruct.Pin = gpios[i].pin;
		HAL_GPIO_Init(gpios[i].port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(gpios[i].port, gpios[i].pin, GPIO_PIN_RESET);
	}
}

/**
 * @brief Функция необходима для реализации функции printf().
 * @param file
 * @param ptr
 * @param len
 * @return
 */
int _write(int file, char *ptr, int len)
{
	buffer_append(&to_console, (uint8_t *)ptr, (size_t)len);
	return len;
}

void HAL_MspInit(void);

/**
 * @brief Каллбэк когда CDC настраивает порт.
 * @param linecoding настройки порта.
 * @return 0 в случае успеха.
 */
uint8_t CDC_SetLineCoding_CB(USBD_CDC_LineCodingTypeDef linecoding)
{
	// Нет необходимости в настройке порта.
	return 0;
}

void CDC_SetControlLineState_CB(uint8_t *pbuf)
{
	if ((pbuf[2] & 0x01) && !connected)
	{
		connected = 1;
		CLI_Header();
	}
	else if (!(pbuf[2] & 0x01))
	{
		connected = 0;
	}
}

/**
 * @brief Каллбэк когда CDC запрашивает настройки порта.
 * @param buf Буфер который будет отправлен на ПК.
 */
void CDC_GetLineCoding_CB(uint8_t *buf)
{
	buf[0] = (uint8_t)(g_linecoding.bitrate);
	buf[1] = (uint8_t)(g_linecoding.bitrate >> 8);
	buf[2] = (uint8_t)(g_linecoding.bitrate >> 16);
	buf[3] = (uint8_t)(g_linecoding.bitrate >> 24);

	buf[4] = g_linecoding.format;
	buf[5] = g_linecoding.paritytype;
	buf[6] = g_linecoding.datatype;
}

/**
 * @brief Прием данных от хоста.
 * @param Buf Указатель на данные.
 * @param Len Количество байт.
 */
void CDC_Receive_FC_CB(uint8_t *Buf, uint16_t Len)
{
	buffer_append(&from_console, Buf, Len);
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
	cs_gpio_init();
	// spi_init();

	usb_device_init();

	uint32_t ticks_last = 0;

	size_t tx_len = 0;
	uint8_t tx_buf[256] = {0};

	size_t rx_len = 0;
	uint8_t rx_buf[256] = {0};

	boolean_t usb_busy_flag = false;

	buffer_init(&to_console, to_console_buffer, sizeof(to_console_buffer), NULL, NULL);
	buffer_init(&from_console, from_console_buffer, sizeof(from_console_buffer), NULL, NULL);

	while (1)
	{

		if (buffer_len(&from_console, &tx_len) != BUFFER_OK)
		{
			Error_Handler();
		}

		if (tx_len != 0)
		{
			tx_len = 1;
			buffer_get(&from_console, tx_buf, &tx_len);
			CLI_Input(tx_buf[0]);
		}

		// Отправка данных в UART.
		if (spi_tx_busy == false)
		{
			// if (buffer_len(&spi_tx_buffer, &tx_len) != BUFFER_OK)
			// {
			// 	Error_Handler();
			// }

			if (tx_len > 256)
			{
				tx_len = 256;
			}
			if (tx_len != 0)
			{
				// if (buffer_get(&uart_tx_buf, tx_buf, &tx_len) != BUFFER_OK)
				// {
				// 	Error_Handler();
				// }

				// Этот флаг должен выставляться раньше чем запускается передача по DMA.
				// Потому что если будет иначе прерывание которое его сбрасывает произойдет раньше чем он установится.
				spi_tx_busy = true;
				// HAL_UART_Transmit_DMA(&huart1, tx_buf, tx_len);
			}
		}

		if (!usb_busy_flag)
		{
			if (buffer_len(&to_console, &rx_len) != BUFFER_OK)
			{
				Error_Handler();
			}

			if (rx_len > CDC_DATA_FS_MAX_PACKET_SIZE)
			{
				rx_len = CDC_DATA_FS_MAX_PACKET_SIZE;
			}

			if (rx_len != 0)
			{
				if (buffer_get(&to_console, rx_buf, &rx_len) != BUFFER_OK)
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

#ifdef DEBUG
		// Светодиодная индикация.

		uint32_t ticks = HAL_GetTick();

		if (ticks >= ticks_last + 1000)
		{
			ticks_last = ticks;
			gpio_debug_led_toggle();
		}
#endif /* DEBUG */

		HAL_Delay(1);
	}
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
	if (buffer_get(&hbuf, data_buf2, &data_size) != BUFFER_OK)
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
