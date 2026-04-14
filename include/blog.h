#ifndef BLOG_H
#define BLOG_H

#include "blog_config.h"
#include "blog_irq.h"
#include "blog_levels.h"
#include "blog_ringbuf.h"

#if BLOG_RTOS != BLOG_RTOS_NONE
#	include "blog_rtos.h"
#endif

#if BLOG_BACKEND == BLOG_BACKEND_UART_DMA
#	include "blog_uart.h"
#elif BLOG_BACKEND == BLOG_BACKEND_SWO
#	include "blog_swo.h"
#elif BLOG_BACKEND == BLOG_BACKEND_RTT
#	include "blog_rtt.h"
#elif BLOG_BACKEND == BLOG_BACKEND_FLASH
#	include "blog_flash.h"
#endif

void blog_init(void);
void blog_deinit(void);
void blog_flush(void);
void blog_flush_blocking(uint32_t max_iterations);
void blog_set_level(blog_level_t level);
blog_level_t blog_get_level(void);

uint16_t blog_module_enabled(uint16_t mod);
void blog_set_module_whitelist(uint32_t mask);
void blog_set_module_blacklist(uint32_t mask);
uint32_t blog_get_module_whitelist(void);
uint32_t blog_get_module_blacklist(void);

void blog_write(blog_level_t level, const char *file, int line, const char *fmt,
                ...);
void blog_write_isr(blog_level_t level, const char *file, int line,
                    const char *fmt, ...);
void blog_hexdump(blog_level_t level, const char *file, int line,
                  const char *tag, const void *data, uint16_t len);

#define _BLOG_EMIT(level, fmt, ...)                                            \
	do                                                                         \
	{                                                                          \
		if ((level) >= BLOG_LEVEL && blog_module_enabled(BLOG_MODULE) != 0u)   \
		{                                                                      \
			blog_write((level), __FILE__, __LINE__, (fmt), ##__VA_ARGS__);     \
		}                                                                      \
	} while (0)

#define _BLOG_EMIT_ISR(level, fmt, ...)                                        \
	do                                                                         \
	{                                                                          \
		if ((level) >= BLOG_LEVEL && blog_module_enabled(BLOG_MODULE) != 0u)   \
		{                                                                      \
			blog_write_isr((level), __FILE__, __LINE__, (fmt), ##__VA_ARGS__); \
		}                                                                      \
	} while (0)

#if BLOG_LEVEL <= BLOG_LEVEL_DEBUG
#	define LOG_DEBUG(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#	define LOG_DEBUG_ISR(fmt, ...)                                            \
		_BLOG_EMIT_ISR(BLOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#	define LOG_DEBUG(fmt, ...)     ((void)0)
#	define LOG_DEBUG_ISR(fmt, ...) ((void)0)
#endif

#if BLOG_LEVEL <= BLOG_LEVEL_INFO
#	define LOG_INFO(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#	define LOG_INFO_ISR(fmt, ...)                                             \
		_BLOG_EMIT_ISR(BLOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#	define LOG_INFO(fmt, ...)     ((void)0)
#	define LOG_INFO_ISR(fmt, ...) ((void)0)
#endif

#if BLOG_LEVEL <= BLOG_LEVEL_WARN
#	define LOG_WARN(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#	define LOG_WARN_ISR(fmt, ...)                                             \
		_BLOG_EMIT_ISR(BLOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#	define LOG_WARN(fmt, ...)     ((void)0)
#	define LOG_WARN_ISR(fmt, ...) ((void)0)
#endif

#if BLOG_LEVEL <= BLOG_LEVEL_ERROR
#	define LOG_ERROR(fmt, ...) _BLOG_EMIT(BLOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#	define LOG_ERROR_ISR(fmt, ...)                                            \
		_BLOG_EMIT_ISR(BLOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#	define LOG_ERROR(fmt, ...)     ((void)0)
#	define LOG_ERROR_ISR(fmt, ...) ((void)0)
#endif

#define LOG_HEXDUMP(level, tag, data, len)                                     \
	do                                                                         \
	{                                                                          \
		if ((level) >= BLOG_LEVEL && blog_module_enabled(BLOG_MODULE) != 0u)   \
		{                                                                      \
			blog_hexdump((level), __FILE__, __LINE__, (tag), (data), (len));   \
		}                                                                      \
	} while (0)

#endif
