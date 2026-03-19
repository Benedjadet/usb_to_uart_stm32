
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "buffer.h"

#include "uart1.h"
#include "rcc.h"
#include "gpio.h"

uint8_t uart_tx_data[2048] = {0};
buffer_handle_t uart_tx_buf = {0};

uint8_t uart_rx_data[2048] = {0};
uint8_t uart_rx_byte = 0;
buffer_handle_t uart_rx_buf = {0};

void HAL_MspInit(void);


USBD_CDC_LineCodingTypeDef uart_linecoding = {
	.bitrate = 9600,
	.format = 0,	 // 1 stop bit
	.paritytype = 0, // none
	.datatype = 8};

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
		HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
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
