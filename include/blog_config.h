#ifndef BLOG_CONFIG_H
#define BLOG_CONFIG_H

#define BLOG_VERSION_MAJOR 0
#define BLOG_VERSION_MINOR 9
#define BLOG_VERSION_PATCH 0

#include <stdint.h>

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#	define BLOG_STATIC_ASSERT(cond, msg) _Static_assert(cond, #msg)
#else
#	define BLOG_STATIC_ASSERT(cond, msg)                                      \
		typedef char blog_static_assert_##msg[(cond) ? 1 : -1]
#endif

#if !defined(BLOG_ASSERT)
#	ifdef NDEBUG
#		define BLOG_ASSERT(cond) ((void)0)
#	else
#		define BLOG_ASSERT(cond)                                              \
			do                                                                 \
			{                                                                  \
				if (!(cond))                                                   \
				{                                                              \
					__builtin_trap();                                          \
				}                                                              \
			} while (0)
#	endif
#endif

#include "blog_levels.h"

#ifndef BLOG_LEVEL
#	define BLOG_LEVEL BLOG_LEVEL_DEBUG
#endif

BLOG_STATIC_ASSERT(BLOG_LEVEL >= BLOG_LEVEL_DEBUG &&
                       BLOG_LEVEL <= BLOG_LEVEL_NONE,
                   blog_level_out_of_range);

#ifndef BLOG_RINGBUF_SIZE
#	define BLOG_RINGBUF_SIZE 256u
#endif

BLOG_STATIC_ASSERT((BLOG_RINGBUF_SIZE & (BLOG_RINGBUF_SIZE - 1u)) == 0u,
                   blog_ringbuf_size_must_be_power_of_2);
BLOG_STATIC_ASSERT(BLOG_RINGBUF_SIZE >= 8u, blog_ringbuf_size_too_small);

#ifndef BLOG_UART_BAUDRATE
#	define BLOG_UART_BAUDRATE 115200u
#endif

#ifndef BLOG_ENABLE_TIMESTAMP
#	define BLOG_ENABLE_TIMESTAMP 0
#endif

#ifndef BLOG_DMA_TX_BUF_SIZE
#	define BLOG_DMA_TX_BUF_SIZE 128u
#endif

#ifndef BLOG_BACKEND
#	define BLOG_BACKEND BLOG_BACKEND_UART_DMA
#endif

#define BLOG_BACKEND_UART_DMA 1
#define BLOG_BACKEND_SWO      2
#define BLOG_BACKEND_RTT      3
#define BLOG_BACKEND_FLASH    4

#ifndef BLOG_ENABLE_COLOR
#	define BLOG_ENABLE_COLOR 0
#endif

#ifdef BLOG_MODULE
#	ifndef BLOG_MODULE_WHITELIST
#		define BLOG_MODULE_WHITELIST
#	endif
#	ifndef BLOG_MODULE_BLACKLIST
#		define BLOG_MODULE_BLACKLIST
#	endif
#endif

#endif
