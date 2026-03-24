#include "buffer.h"
#include "memory.h"

static uint32_t __buffer_istart(void *handle);
static uint32_t __buffer_iend(void *handle);

uint8_t buffer_init(buffer_handle_t *handle, uint8_t *buffer, size_t size, uint32_t (*istart_cb)(void *), uint32_t (*iend_cb)(void *))
{
	if (handle == NULL)
	{
		return BUFFER_NULL_HANDLE;
	}

	if (size == 0)
	{
		return BUFFER_ZERO_LEN;
	}

	// Инициализируем все переменные.
	handle->size = size;
	handle->buffer = buffer;
	handle->istart = 0;
	handle->iend = 0;

	if (istart_cb == NULL)
	{
		handle->istart_cb = __buffer_istart;
	}
	else
	{
		handle->istart_cb = istart_cb;
	}

	if (iend_cb == NULL)
	{
		handle->iend_cb = __buffer_iend;
	}
	else
	{
		handle->iend_cb = iend_cb;
	}

	handle->inited = true;

	return BUFFER_OK;
}

uint8_t buffer_append(buffer_handle_t *handle, const uint8_t *data, size_t len)
{
	if (handle == NULL)
	{
		return BUFFER_NULL_HANDLE;
	}

	if (!handle->inited)
	{
		return BUFFER_NOINITED;
	}

	if (len == 0)
	{
		return BUFFER_ZERO_LEN;
	}

	size_t _len = 0;
	buffer_len(handle, &_len);

	if (len > handle->size - _len)
	{
		return BUFFER_OVERRIDE;
	}

	size_t empty_bytes_to_bound = handle->size - handle->iend_cb(handle);

	if (len <= empty_bytes_to_bound)
	{
		memcpy(&handle->buffer[handle->iend_cb(handle)], data, len);
		handle->iend += len;
	}
	else
	{
		memcpy(&handle->buffer[handle->iend_cb(handle)], data, empty_bytes_to_bound);

		size_t from_start_offset = len - empty_bytes_to_bound;
		memcpy(&handle->buffer[0], data + empty_bytes_to_bound, from_start_offset);
		handle->iend = from_start_offset;

		handle->crossed_border = 1;
	}

	return BUFFER_OK;
}

uint8_t buffer_len(buffer_handle_t *handle, size_t *len)
{
	if (handle == NULL)
	{
		return BUFFER_NULL_HANDLE;
	}

	if (!handle->inited)
	{
		return BUFFER_NOINITED;
	}

	if (handle->istart_cb(handle) <= handle->iend_cb(handle))
	{
		*len = handle->iend_cb(handle) - handle->istart_cb(handle);
	}
	else
	{
		*len = (handle->size - handle->istart_cb(handle)) + handle->iend_cb(handle);
	}

	return BUFFER_OK;
}

uint8_t buffer_get(buffer_handle_t *handle, uint8_t *data, size_t *len)
{
	if (handle == NULL)
	{
		return BUFFER_NULL_HANDLE;
	}

	if (!handle->inited)
	{
		return BUFFER_NOINITED;
	}

	if (*len == 0)
	{
		return BUFFER_ZERO_LEN;
	}

	// Если пользователь запрашивает больше байт чем есть в буфере то возвращаем все что есть в буфере и меняем значение len.
	size_t _len = 0;
	buffer_len(handle, &_len);

	if (_len < *len)
	{
		// Если запрашиваемая длина больше чем размер буфера.
		// Возвращаем все что есть.
		*len = _len;
	}

	// Проверяем переходит ли запрос через границу буфера.

	// Вычисляем сколько байт между началом данных и концом буфера.
	size_t byte_to_bound = handle->size - handle->istart_cb(handle);

	// if (handle->istart_cb(handle) <= handle->iend_cb(handle))
	if (*len <= byte_to_bound)
	{
		// Если объем запрашиваемых длинных не превышает объема данных до границы буфера.
		// Копируем все одной посылкой.
		uint32_t src_istart = handle->istart_cb(handle);
		uint8_t *src_ptr = &(handle->buffer[src_istart]);

		memcpy(data, src_ptr, *len);

		// Смещаем индекс начала буфера.
		handle->istart += *len;
	}
	else
	{
		// Если объем запрашиваемых длинных превышает объема данных до границы буфера.
		// Копируем все что осталось.
		uint32_t src_istart = handle->istart_cb(handle);
		memcpy(data, &(handle->buffer[src_istart]), byte_to_bound);

		// Вычисляем склько нужно скопировать во второй посылке.
		size_t from_start_offset = *len - byte_to_bound;
		memcpy(data + byte_to_bound, &(handle->buffer[0]), from_start_offset);

		// Смещаем индекс начала буфера.
		handle->istart = from_start_offset;
		handle->crossed_border = 0;
	}

	return BUFFER_OK;
}

uint8_t buffer_flush(buffer_handle_t *handle)
{
	if (handle == NULL)
	{
		return BUFFER_NULL_HANDLE;
	}

	if (!handle->inited)
	{
		return BUFFER_NOINITED;
	}

	handle->istart = 0;
	handle->iend = 0;
	handle->crossed_border = 0;

	return BUFFER_OK;
}

static uint32_t __buffer_istart(void *handle)
{
	return ((buffer_handle_t *)handle)->istart;
}

static uint32_t __buffer_iend(void *handle)
{
	return ((buffer_handle_t *)handle)->iend;
}