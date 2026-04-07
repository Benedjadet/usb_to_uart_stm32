#ifndef GPIO_H
#define GPIO_H



#ifdef __cplusplus
extern "C" {
#endif




typedef struct gpio_s
{
	GPIO_TypeDef* port;
	uint32_t pin;
}gpio_t;



void gpio_init(void);
void gpio_debug_led_toggle(void);

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */