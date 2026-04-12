#ifndef BLOG_UART_H
#define BLOG_UART_H

#include <stdint.h>

void blog_uart_init(void);
void blog_uart_putc(char c);
void blog_uart_puts(const char *s, uint16_t len);

#endif
