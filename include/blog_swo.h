#ifndef BLOG_SWO_H
#define BLOG_SWO_H

#include <stdint.h>

void blog_swo_init(void);
void blog_swo_putc(char c);
void blog_swo_puts(const char *s, uint16_t len);

#endif
