#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C"
{
#endif

	extern UART_HandleTypeDef huart1;

	void uart_init(uint32_t baudrate, uint8_t parity, uint8_t stopbits);
	void uart_reset_ore(UART_HandleTypeDef *huart);

	void USART1_IRQHandler(void);


	extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_H */
