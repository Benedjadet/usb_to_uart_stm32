#ifndef USBD_DESC_C
#define USBD_DESC_C

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_def.h"

#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4)
#define         DEVICE_ID3          (UID_BASE + 0x8)

#define  USB_SIZ_STRING_SERIAL       (50)

// Descriptor for the Usb device.
extern USBD_DescriptorsTypeDef FS_Desc;

#ifdef __cplusplus
}
#endif

#endif /* USBD_DESC_C */

