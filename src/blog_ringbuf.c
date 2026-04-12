#include "blog_ringbuf.h"

void blog_ringbuf_init(blog_ringbuf_t *rb)
{
	uint16_t i;

	rb->head = 0u;
	rb->tail = 0u;

	for (i = 0u; i < BLOG_RINGBUF_SIZE; i++)
	{
		rb->buf[i] = 0u;
	}
}

uint16_t blog_ringbuf_push(blog_ringbuf_t *rb, const uint8_t *data,
                           uint16_t len)
{
	uint16_t i;

	for (i = 0u; i < len; i++)
	{
		uint16_t next = (uint16_t)((rb->head + 1u) % BLOG_RINGBUF_SIZE);

		if (next == rb->tail)
		{
			break;
		}

		rb->buf[rb->head] = data[i];
		rb->head = next;
	}

	return i;
}

uint16_t blog_ringbuf_pop(blog_ringbuf_t *rb, uint8_t *data, uint16_t len)
{
	uint16_t i;

	for (i = 0u; i < len; i++)
	{
		if (rb->head == rb->tail)
		{
			break;
		}

		data[i] = rb->buf[rb->tail];
		rb->tail = (uint16_t)((rb->tail + 1u) % BLOG_RINGBUF_SIZE);
	}

	return i;
}

uint16_t blog_ringbuf_available(const blog_ringbuf_t *rb)
{
	uint16_t head = rb->head;
	uint16_t tail = rb->tail;

	if (head >= tail)
	{
		return (uint16_t)(head - tail);
	}

	return (uint16_t)(BLOG_RINGBUF_SIZE - tail + head);
}

void blog_ringbuf_flush(blog_ringbuf_t *rb)
{
	rb->tail = rb->head;
}
