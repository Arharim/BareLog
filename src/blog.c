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

static volatile uint32_t blog_mod_whitelist = BLOG_MODULE_WHITELIST_INIT;
static volatile uint32_t blog_mod_blacklist = BLOG_MODULE_BLACKLIST_INIT;

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

BLOG_STATIC uint16_t uint32_to_str_padded(char *out, uint16_t out_size,
                                          uint32_t val, uint16_t width,
                                          uint16_t zero_pad)
{
	char tmp[10u];
	uint16_t len = 0u;
	uint16_t j;
	uint16_t written;
	uint16_t padding;
	char pad_char;

	if (val == 0u)
	{
		tmp[len++] = '0';
	}
	else
	{
		while (val > 0u)
		{
			tmp[len++] = (char)('0' + (val % 10u));
			val /= 10u;
		}
	}

	padding = 0u;
	if (width > len && width <= 10u)
	{
		padding = (uint16_t)(width - len);
	}

	written = 0u;
	pad_char = zero_pad ? '0' : ' ';

	for (j = 0u; j < padding && written < out_size; j++)
	{
		out[written++] = pad_char;
	}

	for (j = len; j > 0u && written < out_size; j--)
	{
		out[written++] = tmp[j - 1u];
	}

	return written;
}

BLOG_STATIC uint16_t uint32_to_hex(char *out, uint16_t out_size, uint32_t val,
                                   const char *hex_table, uint16_t min_digits,
                                   uint16_t zero_pad)
{
	char tmp[8u];
	uint16_t start;
	uint16_t j;
	uint16_t written;
	int shift;

	for (shift = 28; shift >= 0; shift -= 4)
	{
		tmp[(uint16_t)((28 - shift) / 4u)] = hex_table[(val >> shift) & 0xFu];
	}

	if (zero_pad && min_digits > 0u && min_digits <= 8u)
	{
		start = (8u > min_digits) ? (uint16_t)(8u - min_digits) : 0u;
	}
	else
	{
		start = 0u;
		while (start < 7u && tmp[start] == '0')
		{
			start++;
		}
		if (min_digits > 0u && (8u - start) < min_digits)
		{
			start = (8u > min_digits) ? (uint16_t)(8u - min_digits) : 0u;
		}
	}

	written = 0u;
	for (j = start; j < 8u && written < out_size; j++)
	{
		out[written++] = tmp[j];
	}

	return written;
}

BLOG_STATIC uint16_t format_prefix(char *out, uint16_t out_size,
                                   blog_level_t level, const char *file,
                                   int line)
{
	uint16_t pos = 0u;
	uint16_t slen;
	uint16_t nlen;
	uint16_t i;

#if BLOG_ENABLE_TIMESTAMP
	uint32_t ts = blog_timestamp_get();
	if (pos < out_size)
		out[pos++] = '[';
	if (pos < out_size)
	{
		nlen = uint32_to_str(&out[pos], ts);
		pos += nlen;
	}
	if (pos < out_size)
		out[pos++] = ']';
	if (pos < out_size)
		out[pos++] = ' ';
#endif

#if BLOG_ENABLE_COLOR
	pos += str_copy(&out[pos], level_colors[level], out_size - pos);
#endif

	if (pos < out_size)
		out[pos++] = '[';
	slen = str_len(level_strings[level]);
	if ((pos + slen + 2u) < out_size)
	{
		for (i = 0u; i < slen; i++)
		{
			if (pos < out_size)
				out[pos++] = level_strings[level][i];
		}
	}
	if (pos < out_size)
		out[pos++] = ']';

#if BLOG_ENABLE_COLOR
	pos += str_copy(&out[pos], BLOG_COLOR_RESET, out_size - pos);
#endif

	if (pos < out_size)
		out[pos++] = ' ';

	slen = str_len(file);
	if ((pos + slen + 1u) < out_size)
	{
		for (i = 0u; i < slen; i++)
		{
			if (pos < out_size)
				out[pos++] = file[i];
		}
	}

	if (pos < out_size)
		out[pos++] = ':';
	if (pos < out_size)
	{
		nlen = uint32_to_str(&out[pos], (uint32_t)line);
		pos += nlen;
	}
	if (pos < out_size)
		out[pos++] = ':';
	if (pos < out_size)
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

		if (fmt[i] != '%')
		{
			out[pos++] = fmt[i++];
			continue;
		}

		i++;
		if (fmt[i] == '\0')
		{
			break;
		}

		uint16_t zero_pad = 0u;
		if (fmt[i] == '0')
		{
			zero_pad = 1u;
			i++;
			if (fmt[i] == '\0')
			{
				break;
			}
		}

		uint16_t width = 0u;
		while (fmt[i] >= '0' && fmt[i] <= '9')
		{
			width = (uint16_t)(width * 10u + (uint16_t)(fmt[i] - '0'));
			i++;
			if (fmt[i] == '\0')
			{
				break;
			}
		}
		if (fmt[i] == '\0')
		{
			break;
		}

		char spec = fmt[i++];

		if (spec == 'd' || spec == 'i')
		{
			int val = va_arg(args, int);
			uint32_t uval;
			if (val < 0)
			{
				if (pos < out_size)
				{
					out[pos++] = '-';
				}
				uval = (uint32_t)(-val);
			}
			else
			{
				uval = (uint32_t)val;
			}
			pos += uint32_to_str_padded(&out[pos], (uint16_t)(out_size - pos),
			                            uval, width, zero_pad);
		}
		else if (spec == 'u')
		{
			uint32_t val = va_arg(args, uint32_t);
			pos += uint32_to_str_padded(&out[pos], (uint16_t)(out_size - pos),
			                            val, width, zero_pad);
		}
		else if (spec == 'x')
		{
			static const char hex_lo[] = "0123456789abcdef";
			uint32_t val = va_arg(args, uint32_t);
			pos += uint32_to_hex(&out[pos], (uint16_t)(out_size - pos), val,
			                     hex_lo, width, zero_pad);
		}
		else if (spec == 'X')
		{
			static const char hex_hi[] = "0123456789ABCDEF";
			uint32_t val = va_arg(args, uint32_t);
			pos += uint32_to_hex(&out[pos], (uint16_t)(out_size - pos), val,
			                     hex_hi, width, zero_pad);
		}
		else if (spec == 'p')
		{
			uintptr_t val = (uintptr_t)va_arg(args, void *);
			if (pos + 2u < out_size)
			{
				out[pos++] = '0';
				out[pos++] = 'x';
			}
			static const char hex_lo[] = "0123456789abcdef";
			pos += uint32_to_hex(&out[pos], (uint16_t)(out_size - pos),
			                     (uint32_t)val, hex_lo, 8u, 1u);
		}
		else if (spec == 's')
		{
			const char *s = va_arg(args, const char *);
			while (*s != '\0')
			{
				if (pos >= out_size)
				{
					break;
				}
				out[pos++] = *s++;
			}
		}
		else if (spec == 'c')
		{
			char c = (char)va_arg(args, int);
			if (pos < out_size)
			{
				out[pos++] = c;
			}
		}
		else if (spec == '%')
		{
			if (pos < out_size)
			{
				out[pos++] = '%';
			}
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

static void backend_deinit(void)
{
#if BLOG_BACKEND == BLOG_BACKEND_UART_DMA
	blog_uart_deinit();
#elif BLOG_BACKEND == BLOG_BACKEND_SWO
	blog_swo_deinit();
#elif BLOG_BACKEND == BLOG_BACKEND_RTT
	blog_rtt_deinit();
#elif BLOG_BACKEND == BLOG_BACKEND_FLASH
	blog_flash_deinit();
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

uint16_t blog_module_enabled(uint16_t mod)
{
	if (mod == 0u)
	{
		return 1u;
	}

	uint32_t bit = (uint32_t)(1u << (mod - 1u));
	if ((bit & blog_mod_blacklist) != 0u)
	{
		return 0u;
	}
	if ((bit & blog_mod_whitelist) != 0u)
	{
		return 1u;
	}
	return 0u;
}

void blog_flush_blocking(uint32_t max_iterations);

void blog_init(void)
{
	blog_ringbuf_init(&blog_buf);
	backend_init();
#if BLOG_ENABLE_TIMESTAMP
	blog_timestamp_init();
#endif
}

void blog_deinit(void)
{
	blog_flush_blocking(BLOG_FLUSH_MAX_ITERATIONS);
	backend_deinit();
	blog_ringbuf_flush(&blog_buf);
}

void blog_set_level(blog_level_t level)
{
	blog_runtime_level = level;
}

blog_level_t blog_get_level(void)
{
	return blog_runtime_level;
}

void blog_set_module_whitelist(uint32_t mask)
{
	blog_mod_whitelist = mask;
}

void blog_set_module_blacklist(uint32_t mask)
{
	blog_mod_blacklist = mask;
}

uint32_t blog_get_module_whitelist(void)
{
	return blog_mod_whitelist;
}

uint32_t blog_get_module_blacklist(void)
{
	return blog_mod_blacklist;
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

void blog_flush_blocking(uint32_t max_iterations)
{
	uint32_t iter = 0u;

	while (iter < max_iterations)
	{
		blog_flush();

		if (blog_ringbuf_available(&blog_buf) == 0u && backend_busy() == 0u)
		{
			break;
		}

		iter++;
	}
}

static void do_blog_write(blog_level_t level, const char *file, int line,
                          const char *fmt, va_list args, uint16_t use_isr)
{
	if (level < blog_runtime_level)
	{
		return;
	}

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

void blog_hexdump(blog_level_t level, const char *file, int line,
                  const char *tag, const void *data, uint16_t len)
{
	if (level < blog_runtime_level)
	{
		return;
	}

	static const char hex[] = "0123456789abcdef";
	const uint8_t *bytes = (const uint8_t *)data;
	uint16_t offset = 0u;

	while (offset < len)
	{
		char line_buf[80u];
		uint16_t pos = 0u;
		uint16_t j;

		if (offset == 0u)
		{
			pos += format_prefix(line_buf, sizeof(line_buf), level, file, line);
		}
		else
		{
			for (j = 0u; j < 12u && pos < sizeof(line_buf); j++)
			{
				line_buf[pos++] = ' ';
			}
		}

		if (offset == 0u && tag != NULL)
		{
			pos += str_copy(&line_buf[pos], tag,
			                (uint16_t)(sizeof(line_buf) - pos));
			pos += str_copy(&line_buf[pos], " ",
			                (uint16_t)(sizeof(line_buf) - pos));
		}

		pos += uint32_to_str_padded(&line_buf[pos],
		                            (uint16_t)(sizeof(line_buf) - pos),
		                            (uint32_t)offset, 4u, 1u);
		line_buf[pos++] = ':';
		line_buf[pos++] = ' ';

		for (j = 0u; j < 16u && (offset + j) < len; j++)
		{
			if (pos + 3u < sizeof(line_buf))
			{
				line_buf[pos++] = hex[(bytes[offset + j] >> 4u) & 0xFu];
				line_buf[pos++] = hex[bytes[offset + j] & 0xFu];
				line_buf[pos++] = ' ';
			}
		}

		for (; j < 16u; j++)
		{
			if (pos + 3u < sizeof(line_buf))
			{
				line_buf[pos++] = ' ';
				line_buf[pos++] = ' ';
				line_buf[pos++] = ' ';
			}
		}

		line_buf[pos++] = ' ';

		for (j = 0u; j < 16u && (offset + j) < len; j++)
		{
			char c = (char)bytes[offset + j];
			line_buf[pos++] = (c >= 0x20 && c <= 0x7E) ? c : '.';
		}

		if (pos + 2u < sizeof(line_buf))
		{
			line_buf[pos++] = '\r';
			line_buf[pos++] = '\n';
		}

		blog_ringbuf_push(&blog_buf, (const uint8_t *)line_buf, pos);
		offset += 16u;
	}
}
