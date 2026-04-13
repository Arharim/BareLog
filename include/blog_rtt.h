#ifndef BLOG_RTT_H
#define BLOG_RTT_H

#include <stdint.h>

void blog_rtt_init(void);
void blog_rtt_putc(char c);
void blog_rtt_puts(const char *s, uint16_t len);
uint16_t blog_rtt_available(void);

#endif
