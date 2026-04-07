#ifndef USBD_CDC_IF_H
#define USBD_CDC_IF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "usbd_cdc.h"

// Receive buffer size.
#define APP_RX_DATA_SIZE 2048

// Transmit buffer size.
#define APP_TX_DATA_SIZE 2048

	// CDC Interface callback.
	extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

	// CDC Receive callback.
	extern void CDC_Receive_FC_CB(uint8_t *Buf, uint16_t Len);

	// CDC Set Line Coding.
	extern uint8_t CDC_SetLineCoding_CB(USBD_CDC_LineCodingTypeDef linecoding);

	// CDC Get Line Coding.
	extern void CDC_GetLineCoding_CB(uint8_t* buf);

	// CDC Transmit.
	uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif /* USBD_CDC_IF_H */
