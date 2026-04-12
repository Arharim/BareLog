#include "blog_ringbuf.h"
#include "blog_irq.h"

void blog_ringbuf_init(blog_ringbuf_t *rb)
{
	uint32_t primask = blog_irq_save();

	rb->head = 0u;
	rb->tail = 0u;

	uint16_t i;
	for (i = 0u; i < BLOG_RINGBUF_SIZE; i++)
	{
		rb->buf[i] = 0u;
	}

	blog_irq_restore(primask);
}

static uint16_t ringbuf_push_impl(blog_ringbuf_t *rb, const uint8_t *data,
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

uint16_t blog_ringbuf_push(blog_ringbuf_t *rb, const uint8_t *data,
                           uint16_t len)
{
	uint32_t primask = blog_irq_save();
	uint16_t result = ringbuf_push_impl(rb, data, len);
	blog_irq_restore(primask);
	return result;
}

uint16_t blog_ringbuf_push_isr(blog_ringbuf_t *rb, const uint8_t *data,
                               uint16_t len)
{
	return ringbuf_push_impl(rb, data, len);
}

static uint16_t ringbuf_pop_impl(blog_ringbuf_t *rb, uint8_t *data,
                                 uint16_t len)
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

uint16_t blog_ringbuf_pop(blog_ringbuf_t *rb, uint8_t *data, uint16_t len)
{
	uint32_t primask = blog_irq_save();
	uint16_t result = ringbuf_pop_impl(rb, data, len);
	blog_irq_restore(primask);
	return result;
}

uint16_t blog_ringbuf_pop_isr(blog_ringbuf_t *rb, uint8_t *data, uint16_t len)
{
	return ringbuf_pop_impl(rb, data, len);
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
	uint32_t primask = blog_irq_save();
	rb->tail = rb->head;
	blog_irq_restore(primask);
}
