#ifndef BUFFER_H
#define BUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif

#define BUFFER_OK (0)
#define BUFFER_NOINITED (1)
#define BUFFER_NULL_HANDLE (2)
#define BUFFER_OVERRIDE (3)
#define BUFFER_ZERO_LEN (4)

	typedef struct buffer_s
	{
		// Размер буфера.
		size_t size;

		// Функция возвращает индекс начала данных.
		uint32_t (*istart_cb)(void *);

		// Функция возвращает индекс конца данных + 1.
		uint32_t (*iend_cb)(void *);

		// Локальный индекс начала данных.
		uint32_t istart;

		// Локальный индекс конца данных + 1.
		uint32_t iend;

		// Буфер перешел границу.
		uint8_t crossed_border;

		// Инициализация экземпляра.
		boolean_t inited;

		// Указатель на буфер данных.
		uint8_t *buffer;
	} buffer_handle_t;

	uint8_t buffer_init(buffer_handle_t *handle, uint8_t *buffer, size_t size, uint32_t (*istart_cb)(void *), uint32_t (*iend_cb)(void *));
	uint8_t buffer_append(buffer_handle_t *handle, const uint8_t *data, size_t len);
	uint8_t buffer_get(buffer_handle_t *handle, uint8_t *data, size_t *len);
	uint8_t buffer_flush(buffer_handle_t *handle);
	uint8_t buffer_len(buffer_handle_t *handle, size_t *len);

#ifdef __cplusplus
}
#endif

#endif /* UART_BUFFER_H */