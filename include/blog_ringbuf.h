#ifndef BLOG_RINGBUF_H
#define BLOG_RINGBUF_H

#include <stddef.h>
#include <stdint.h>

#ifndef BLOG_RINGBUF_SIZE
#	include "blog_config.h"
#endif

typedef struct
{
	uint8_t buf[BLOG_RINGBUF_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
	volatile uint16_t dropped;
} blog_ringbuf_t;

void blog_ringbuf_init(blog_ringbuf_t *rb);
uint16_t blog_ringbuf_push(blog_ringbuf_t *rb, const uint8_t *data,
                           uint16_t len);
uint16_t blog_ringbuf_push_isr(blog_ringbuf_t *rb, const uint8_t *data,
                               uint16_t len);
uint16_t blog_ringbuf_pop(blog_ringbuf_t *rb, uint8_t *data, uint16_t len);
uint16_t blog_ringbuf_pop_isr(blog_ringbuf_t *rb, uint8_t *data, uint16_t len);
uint16_t blog_ringbuf_available(const blog_ringbuf_t *rb);
uint16_t blog_ringbuf_get_dropped(const blog_ringbuf_t *rb);
void blog_ringbuf_clear_dropped(blog_ringbuf_t *rb);
void blog_ringbuf_flush(blog_ringbuf_t *rb);

#endif
