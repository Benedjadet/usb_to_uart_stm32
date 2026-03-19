#ifndef BUFFER_H
#define BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif



#define BUFFER_OK (0)
#define BUFFER_NOINITED (1)
#define BUFFER_OVERRIDE (2)

typedef struct buffer_s
{
	size_t size;
	size_t istart;
	size_t iend;
	boolean_t inited;
	uint8_t *buffer;
} buffer_handle_t;

uint8_t buffer_init(buffer_handle_t *handle, uint8_t *buffer, size_t size);
uint8_t buffer_append(buffer_handle_t *handle, const uint8_t *data, size_t len);
uint8_t buffer_get(buffer_handle_t *handle, uint8_t *data, size_t *len);
size_t buffer_len(buffer_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* UART_BUFFER_H */