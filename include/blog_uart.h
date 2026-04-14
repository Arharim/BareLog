#ifndef BLOG_UART_H
#define BLOG_UART_H

#include <stdint.h>

void blog_uart_init(void);
void blog_uart_deinit(void);
void blog_uart_dma_send(const uint8_t *data, uint16_t len);
uint16_t blog_uart_dma_running(void);
void blog_uart_dma_irq_handler(void);

#endif
