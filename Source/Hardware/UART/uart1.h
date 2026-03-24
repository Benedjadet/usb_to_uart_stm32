#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C"
{
#endif

	extern UART_HandleTypeDef huart1;
	extern DMA_HandleTypeDef hdma_usart1_rx;

	void uart_init(uint32_t baudrate, uint8_t parity, uint8_t stopbits);
	void uart_reset_ore(UART_HandleTypeDef *huart);

	extern void UART_IdleCallback(UART_HandleTypeDef *huart);

	void USART1_IRQHandler(void);
	void DMA1_Channel4_IRQHandler(void);
	void DMA1_Channel5_IRQHandler(void);


	extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_H */
