#ifndef BLOG_CONFIG_H
#define BLOG_CONFIG_H

#include "blog_levels.h"

#ifndef BLOG_LEVEL
#	define BLOG_LEVEL BLOG_LEVEL_DEBUG
#endif

#ifndef BLOG_RINGBUF_SIZE
#	define BLOG_RINGBUF_SIZE 256u
#endif

#ifndef BLOG_UART_BAUDRATE
#	define BLOG_UART_BAUDRATE 115200u
#endif

#ifndef BLOG_ENABLE_TIMESTAMP
#	define BLOG_ENABLE_TIMESTAMP 0
#endif

#ifndef BLOG_DMA_TX_BUF_SIZE
#	define BLOG_DMA_TX_BUF_SIZE 128u
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
