#include "blog_rtt.h"
#include "SEGGER_RTT.h"

#ifndef BLOG_RTT_CHANNEL
#	define BLOG_RTT_CHANNEL 0u
#endif

#ifndef BLOG_RTT_BUFFER_SIZE
#	define BLOG_RTT_BUFFER_SIZE 256u
#endif

#ifndef BLOG_RTT_MODE
#	define BLOG_RTT_MODE SEGGER_RTT_MODE_NO_BLOCK_SKIP
#endif

void blog_rtt_init(void)
{
	SEGGER_RTT_ConfigUpBuffer(BLOG_RTT_CHANNEL, "BareLog", NULL,
	                          BLOG_RTT_BUFFER_SIZE, BLOG_RTT_MODE);
}

void blog_rtt_putc(char c)
{
	SEGGER_RTT_PutChar(BLOG_RTT_CHANNEL, (unsigned char)c);
}

void blog_rtt_puts(const char *s, uint16_t len)
{
	SEGGER_RTT_Write(BLOG_RTT_CHANNEL, s, (unsigned)len);
}

uint16_t blog_rtt_available(void)
{
	unsigned avail = SEGGER_RTT_HasData(BLOG_RTT_CHANNEL);
	return (uint16_t)avail;
}
