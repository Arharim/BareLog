#ifndef BLOG_H
#define BLOG_H

#include "blog_config.h"
#include "blog_levels.h"
#include "blog_ringbuf.h"
#include "blog_uart.h"

void blog_init(void);
void blog_flush(void);
void blog_write(blog_level_t level, const char *file, int line, const char *fmt,
                ...);

#define _BLOG_EMIT(level, fmt, ...)                                            \
	do                                                                         \
	{                                                                          \
		if ((level) >= BLOG_LEVEL)                                             \
		{                                                                      \
			blog_write((level), __FILE__, __LINE__, (fmt), ##__VA_ARGS__);     \
		}                                                                      \
	} while (0)

#if BLOG_LEVEL <= BLOG_LEVEL_DEBUG
#	define LOG_DEBUG(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#	define LOG_DEBUG(fmt, ...) ((void)0)
#endif

#if BLOG_LEVEL <= BLOG_LEVEL_INFO
#	define LOG_INFO(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#	define LOG_INFO(fmt, ...) ((void)0)
#endif

#if BLOG_LEVEL <= BLOG_LEVEL_WARN
#	define LOG_WARN(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#	define LOG_WARN(fmt, ...) ((void)0)
#endif

#if BLOG_LEVEL <= BLOG_LEVEL_ERROR
#	define LOG_ERROR(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#	define LOG_ERROR(fmt, ...) ((void)0)
#endif

#endif
