#ifndef USB_DEVICE_CONF_H
#define USB_DEVICE_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif


#define USBD_VID 0x1209
#define USBD_PID_FS 0xDB50
#define USBD_LANGID_STRING 1033
#define USBD_MANUFACTURER_STRING "MFMC LLC"
#define USBD_PRODUCT_STRING_FS "UART-Link over STM32"
#define USBD_CONFIGURATION_STRING_FS "CDC Config"
#define USBD_INTERFACE_STRING_FS "CDC Interface"




#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_CONF_H */