#include <stddef.h>

void DMA1_Channel4_IRQHandler(void)
{
	extern void blog_uart_dma_irq_handler(void);
	blog_uart_dma_irq_handler();
}
