
#include "gpio.h"
#include "gpio_conf.h"

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void gpio_init(void)
{

	// GPIO Ports Clock Enable.
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	// Debug Led.
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DEBUG_LED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DEBUG_LED_PORT, &GPIO_InitStruct);

	// RS485 Direction control.
	HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = RS485_DIR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RS485_DIR_PORT, &GPIO_InitStruct);
}

void gpio_debug_led_toggle(void)
{
	HAL_GPIO_TogglePin(DEBUG_LED_PORT, DEBUG_LED_PIN);
}

void gpio_rs485_dir_set(GPIO_PinState state)
{
	HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_PIN, state);
}