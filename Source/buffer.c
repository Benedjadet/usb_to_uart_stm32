#include "buffer.h"
#include "memory.h"

uint8_t buffer_init(buffer_handle_t *handle, uint8_t *buffer, size_t size)
{
	// Инициализируем все переменные.
	handle->size = size;
	handle->buffer = buffer;
	handle->istart = 0;
	handle->iend = 0;
	handle->inited = true;

	return BUFFER_OK;
}

uint8_t buffer_append(buffer_handle_t *handle, const uint8_t *data, size_t len)
{

	if (handle->iend + len > handle->size - 1)
	{
		size_t to_end_len = handle->size - handle->iend;
		size_t from_start_offset = len - to_end_len;
		memcpy(&handle->buffer[handle->iend], data, to_end_len);
		memcpy(&handle->buffer[0], data + to_end_len, from_start_offset);
		handle->iend = from_start_offset;
	}
	else
	{
		memcpy(&handle->buffer[handle->iend], data, len);
		handle->iend += len;
	}

	return BUFFER_OK;
}

size_t buffer_len(buffer_handle_t *handle)
{
	if (handle->istart <= handle->iend)
	{
		return handle->iend - handle->istart;
	}
	else
	{
		return (handle->size - handle->istart) + handle->iend;
	}
}

uint8_t buffer_get(buffer_handle_t *handle, uint8_t *data, size_t *len)
{
	size_t buf_len = buffer_len(handle);

	if (buf_len < *len)
	{
		// Если запрашиваемая длина больше чем размер буфера.
		// Возвращаем все что есть.
		*len = buf_len;
	}

	if (handle->istart <= handle->iend)
	{
		// Если индекс начала не опережает индекс конца буфера.
		// Копируем все одной посылкой.
		memcpy(data, &handle->buffer[handle->istart], *len);

		// Смещаем индекс начала буфера.
		handle->istart += *len;
	}
	else
	{
		// Если индекс начала опережает индекс конца буфера.
		// Вычисляем склько нужно скопировать в первой посылке.
		size_t to_end_len = handle->size - handle->istart;
		memcpy(data, &handle->buffer[handle->istart], to_end_len);

		// Вычисляем склько нужно скопировать во второй посылке.
		size_t from_start_offset = *len - to_end_len;
		memcpy(data + to_end_len, &handle->buffer[0], from_start_offset);

		// Смещаем индекс начала буфера.
		handle->istart = from_start_offset;
	}

	return BUFFER_OK;
}