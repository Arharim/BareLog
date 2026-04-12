#include "blog.h"
#include "blog_config.h"
#include "blog_levels.h"
#include "blog_ringbuf.h"
#include "blog_uart.h"

#include <stdarg.h>
#include <stdint.h>

static blog_ringbuf_t blog_buf;

static const char *level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR"};

static uint16_t str_len(const char *s)
{
	uint16_t len = 0u;

	while (s[len] != '\0')
	{
		len++;
	}

	return len;
}

static uint16_t uint32_to_str(char *out, uint32_t val)
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

static uint16_t format_prefix(char *out, uint16_t out_size, blog_level_t level,
                              const char *file, int line)
{
	uint16_t pos = 0u;
	uint16_t slen;
	uint16_t nlen;

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

static uint16_t format_msg(char *out, uint16_t out_size, const char *fmt,
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

void blog_init(void)
{
	blog_ringbuf_init(&blog_buf);
	blog_uart_init();
}

void blog_flush(void)
{
	uint8_t tmp[64u];
	uint16_t count;

	do
	{
		count = blog_ringbuf_pop(&blog_buf, tmp, sizeof(tmp));
		if (count > 0u)
		{
			blog_uart_puts((const char *)tmp, count);
		}
	} while (count > 0u);
}

void blog_write(blog_level_t level, const char *file, int line, const char *fmt,
                ...)
{
	char msg[128u];
	uint16_t pos;
	va_list args;

	pos = format_prefix(msg, sizeof(msg), level, file, line);

	va_start(args, fmt);
	pos += format_msg(&msg[pos], (uint16_t)(sizeof(msg) - pos), fmt, args);
	va_end(args);

	if (pos < sizeof(msg))
	{
		msg[pos++] = '\r';
	}
	if (pos < sizeof(msg))
	{
		msg[pos++] = '\n';
	}

	blog_ringbuf_push(&blog_buf, (const uint8_t *)msg, pos);
}
