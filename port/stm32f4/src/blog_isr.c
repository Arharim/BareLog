#include "blog_config.h"

#if BLOG_BACKEND == BLOG_BACKEND_UART_DMA
#	include "blog_uart.h"

void DMA2_Stream6_IRQHandler(void)
{
	blog_uart_dma_irq_handler();
}
#endif
