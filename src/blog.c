#include "blog.h"
#include "blog_config.h"
#include "blog_irq.h"
#include "blog_levels.h"
#include "blog_ringbuf.h"
#include "blog_timestamp.h"

#include <stdarg.h>
#include <stdint.h>

static blog_ringbuf_t blog_buf;
static volatile blog_level_t blog_runtime_level = BLOG_LEVEL;

#ifdef BLOG_TEST
#	define BLOG_STATIC
#else
#	define BLOG_STATIC static
#endif

#define BLOG_TX_BUF_SIZE 128u
static uint8_t blog_tx_buf[BLOG_TX_BUF_SIZE];

static const char *level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR"};

#if BLOG_ENABLE_COLOR
static const char *level_colors[] = {
    "\033[36m",
    "\033[32m",
    "\033[33m",
    "\033[31m",
};
#	define BLOG_COLOR_RESET "\033[0m"
#endif

BLOG_STATIC uint16_t str_len(const char *s)
{
	uint16_t len = 0u;
	while (s[len] != '\0')
	{
		len++;
	}
	return len;
}

BLOG_STATIC uint16_t str_copy(char *dst, const char *src, uint16_t max)
{
	uint16_t i = 0u;
	while (src[i] != '\0' && i < max)
	{
		dst[i] = src[i];
		i++;
	}
	return i;
}

BLOG_STATIC uint16_t uint32_to_str(char *out, uint32_t val)
{
	char tmp[10u];
	uint16_t i = 0u;
	uint16_t j;

	if (val == 0u)
	{
		out[0] = '0';
		return 1u;
	}

	while (val > 0u)
	{
		tmp[i] = (char)('0' + (val % 10u));
		val /= 10u;
		i++;
	}

	for (j = 0u; j < i; j++)
	{
		out[j] = tmp[i - 1u - j];
	}

	return i;
}

BLOG_STATIC uint16_t format_prefix(char *out, uint16_t out_size,
                                   blog_level_t level, const char *file,
                                   int line)
{
	uint16_t pos = 0u;
	uint16_t slen;
	uint16_t nlen;

#if BLOG_ENABLE_TIMESTAMP
	uint32_t ts = blog_timestamp_get();
	out[pos++] = '[';
	nlen = uint32_to_str(&out[pos], ts);
	pos += nlen;
	out[pos++] = ']';
	out[pos++] = ' ';
#endif

#if BLOG_ENABLE_COLOR
	pos += str_copy(&out[pos], level_colors[level], out_size - pos);
#endif

	out[pos++] = '[';
	slen = str_len(level_strings[level]);
	if ((pos + slen + 2u) < out_size)
	{
		uint16_t i;
		for (i = 0u; i < slen; i++)
		{
			out[pos++] = level_strings[level][i];
		}
	}
	out[pos++] = ']';

#if BLOG_ENABLE_COLOR
	pos += str_copy(&out[pos], BLOG_COLOR_RESET, out_size - pos);
#endif

	out[pos++] = ' ';

	slen = str_len(file);
	if ((pos + slen + 1u) < out_size)
	{
		uint16_t i;
		for (i = 0u; i < slen; i++)
		{
			out[pos++] = file[i];
		}
	}

	out[pos++] = ':';
	nlen = uint32_to_str(&out[pos], (uint32_t)line);
	pos += nlen;
	out[pos++] = ':';
	out[pos++] = ' ';

	return pos;
}

BLOG_STATIC uint16_t format_msg(char *out, uint16_t out_size, const char *fmt,
                                va_list args)
{
	uint16_t pos = 0u;
	uint16_t i = 0u;

	while (fmt[i] != '\0')
	{
		if (pos >= out_size)
		{
			break;
		}

		if (fmt[i] == '%')
		{
			i++;
			if (fmt[i] == 'd' || fmt[i] == 'i')
			{
				int val = va_arg(args, int);
				uint32_t uval;
				if (val < 0)
				{
					out[pos++] = '-';
					uval = (uint32_t)(-val);
				}
				else
				{
					uval = (uint32_t)val;
				}
				pos += uint32_to_str(&out[pos], uval);
			}
			else if (fmt[i] == 'u')
			{
				uint32_t val = va_arg(args, uint32_t);
				pos += uint32_to_str(&out[pos], val);
			}
			else if (fmt[i] == 's')
			{
				const char *s = va_arg(args, const char *);
				while (*s != '\0')
				{
					if (pos >= out_size)
						break;
					out[pos++] = *s++;
				}
			}
			else if (fmt[i] == 'c')
			{
				char c = (char)va_arg(args, int);
				out[pos++] = c;
			}
			else if (fmt[i] == 'x')
			{
				static const char hex[] = "0123456789abcdef";
				uint32_t val = va_arg(args, uint32_t);
				int shift;
				for (shift = 28; shift >= 0; shift -= 4)
				{
					if (pos >= out_size)
						break;
					out[pos++] = hex[(val >> shift) & 0xFu];
				}
			}
			else if (fmt[i] == '%')
			{
				out[pos++] = '%';
			}
			i++;
		}
		else
		{
			out[pos++] = fmt[i++];
		}
	}

	return pos;
}

static void backend_init(void)
{
#if BLOG_BACKEND == BLOG_BACKEND_UART_DMA
	blog_uart_init();
#elif BLOG_BACKEND == BLOG_BACKEND_SWO
	blog_swo_init();
#elif BLOG_BACKEND == BLOG_BACKEND_RTT
	blog_rtt_init();
#elif BLOG_BACKEND == BLOG_BACKEND_FLASH
	blog_flash_init();
#endif
}

static uint16_t backend_busy(void)
{
#if BLOG_BACKEND == BLOG_BACKEND_UART_DMA
	return blog_uart_dma_running();
#else
	return 0u;
#endif
}

static void backend_send(const uint8_t *data, uint16_t len)
{
#if BLOG_BACKEND == BLOG_BACKEND_UART_DMA
	blog_uart_dma_send(data, len);
#elif BLOG_BACKEND == BLOG_BACKEND_SWO
	blog_swo_puts((const char *)data, len);
#elif BLOG_BACKEND == BLOG_BACKEND_RTT
	blog_rtt_puts((const char *)data, len);
#elif BLOG_BACKEND == BLOG_BACKEND_FLASH
	blog_flash_write(data, len);
#endif
}

static uint16_t format_overflow_msg(char *out, uint16_t out_size,
                                    uint16_t dropped)
{
	uint16_t pos = 0u;

	pos += str_copy(&out[pos], "--- ", out_size - pos);
	pos += uint32_to_str(&out[pos], (uint32_t)dropped);
	pos += str_copy(&out[pos], " messages lost ---\r\n", out_size - pos);

	return pos;
}

void blog_init(void)
{
	blog_ringbuf_init(&blog_buf);
	backend_init();
#if BLOG_ENABLE_TIMESTAMP
	blog_timestamp_init();
#endif
}

void blog_set_level(blog_level_t level)
{
	blog_runtime_level = level;
}

blog_level_t blog_get_level(void)
{
	return blog_runtime_level;
}

void blog_flush(void)
{
	if (backend_busy() != 0u)
	{
		return;
	}

	uint16_t dropped = blog_ringbuf_get_dropped(&blog_buf);
	if (dropped > 0u)
	{
		char overflow_msg[48u];
		uint16_t olen =
		    format_overflow_msg(overflow_msg, sizeof(overflow_msg), dropped);
		blog_ringbuf_clear_dropped(&blog_buf);
		blog_ringbuf_push(&blog_buf, (const uint8_t *)overflow_msg, olen);
	}

	uint16_t count = blog_ringbuf_pop(&blog_buf, blog_tx_buf, BLOG_TX_BUF_SIZE);
	if (count > 0u)
	{
		backend_send(blog_tx_buf, count);
	}
}

static void do_blog_write(blog_level_t level, const char *file, int line,
                          const char *fmt, va_list args, uint16_t use_isr)
{
	char msg[192u];
	uint16_t pos;

	pos = format_prefix(msg, sizeof(msg), level, file, line);
	pos += format_msg(&msg[pos], (uint16_t)(sizeof(msg) - pos), fmt, args);

	if (pos < sizeof(msg))
	{
		msg[pos++] = '\r';
	}
	if (pos < sizeof(msg))
	{
		msg[pos++] = '\n';
	}

	if (use_isr != 0u)
	{
		blog_ringbuf_push_isr(&blog_buf, (const uint8_t *)msg, pos);
	}
	else
	{
		blog_ringbuf_push(&blog_buf, (const uint8_t *)msg, pos);
	}
}

void blog_write(blog_level_t level, const char *file, int line, const char *fmt,
                ...)
{
	va_list args;
	va_start(args, fmt);
	do_blog_write(level, file, line, fmt, args, 0u);
	va_end(args);
}

void blog_write_isr(blog_level_t level, const char *file, int line,
                    const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_blog_write(level, file, line, fmt, args, 1u);
	va_end(args);
}
