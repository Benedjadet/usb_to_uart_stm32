#ifndef GPIO_H
#define GPIO_H



#ifdef __cplusplus
extern "C" {
#endif





void gpio_init(void);
void gpio_debug_led_toggle(void);
void gpio_rs485_dir_set(GPIO_PinState state);

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */