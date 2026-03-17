#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_def.h"

	// USB Device initialization function.
	void usb_device_init(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_H */
