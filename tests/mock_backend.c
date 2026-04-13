#include <stdint.h>

void blog_uart_init(void)
{
}

uint16_t blog_uart_dma_running(void)
{
	return 0u;
}

void blog_uart_dma_send(const uint8_t *data, uint16_t len)
{
	(void)data;
	(void)len;
}

void blog_timestamp_init(void)
{
}

uint32_t blog_timestamp_get(void)
{
	return 0u;
}
