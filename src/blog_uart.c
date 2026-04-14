#include "blog_uart.h"
#include "blog_config.h"
#include "blog_uart_port.h"
#include "stm32f10x.h"

static volatile uint16_t dma_tx_len;

static void gpio_init(void)
{
	RCC->APB2ENR |= BLOG_UART_GPIO_RCC;

	if (BLOG_UART_TX_PIN < 8u)
	{
		uint32_t shift = (uint32_t)(BLOG_UART_TX_PIN * 4u);
		BLOG_UART_GPIO->CRL &= ~(0x0Fu << shift);
		BLOG_UART_GPIO->CRL |= ((uint32_t)0x0Bu << shift);
	}
	else
	{
		uint32_t shift = (uint32_t)((BLOG_UART_TX_PIN - 8u) * 4u);
		BLOG_UART_GPIO->CRH &= ~(0x0Fu << shift);
		BLOG_UART_GPIO->CRH |= ((uint32_t)0x0Bu << shift);
	}

	if (BLOG_UART_RX_PIN < 8u)
	{
		uint32_t shift = (uint32_t)(BLOG_UART_RX_PIN * 4u);
		BLOG_UART_GPIO->CRL &= ~(0x0Fu << shift);
		BLOG_UART_GPIO->CRL |= ((uint32_t)0x04u << shift);
	}
	else
	{
		uint32_t shift = (uint32_t)((BLOG_UART_RX_PIN - 8u) * 4u);
		BLOG_UART_GPIO->CRH &= ~(0x0Fu << shift);
		BLOG_UART_GPIO->CRH |= ((uint32_t)0x04u << shift);
	}
}

static void dma_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	BLOG_UART_DMA_CHANNEL->CCR = 0u;
	dma_tx_len = 0u;

	NVIC_SetPriority(BLOG_UART_DMA_IRQn, 1u);
	NVIC_EnableIRQ(BLOG_UART_DMA_IRQn);
}

void blog_uart_init(void)
{
	uint16_t brr_val;

	gpio_init();
	dma_init();

	RCC->APB2ENR |= BLOG_UART_RCC_BIT;

	brr_val = (uint16_t)(BLOG_UART_CLOCK_HZ / BLOG_UART_BAUDRATE);
	BLOG_UARTx->BRR = brr_val;

	BLOG_UARTx->CR1 = (uint16_t)(USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
	BLOG_UARTx->CR3 = USART_CR3_DMAT;
}

void blog_uart_deinit(void)
{
	BLOG_UART_DMA_CHANNEL->CCR &= ~(uint32_t)0x01u;
	NVIC_DisableIRQ(BLOG_UART_DMA_IRQn);

	BLOG_UARTx->CR1 &= ~(uint16_t)USART_CR1_UE;
	BLOG_UARTx->CR3 &= ~(uint16_t)USART_CR3_DMAT;

	dma_tx_len = 0u;
}

void blog_uart_dma_send(const uint8_t *data, uint16_t len)
{
	if (len == 0u)
	{
		return;
	}

	dma_tx_len = len;

	BLOG_UART_DMA_CHANNEL->CCR &= ~(uint32_t)0x01u;
	BLOG_UART_DMA_CHANNEL->CMAR = (uint32_t)data;
	BLOG_UART_DMA_CHANNEL->CPAR = (uint32_t)&(BLOG_UARTx->DR);
	BLOG_UART_DMA_CHANNEL->CNDTR = (uint32_t)len;
	BLOG_UART_DMA_CHANNEL->CCR =
	    (uint32_t)((1u << 7u) | (1u << 4u) | (1u << 1u) | (1u << 3u) | 0x01u);
}

uint16_t blog_uart_dma_running(void)
{
	if ((BLOG_UART_DMA_CHANNEL->CCR & 0x01u) != 0u)
	{
		return 1u;
	}

	return 0u;
}

void blog_uart_dma_irq_handler(void)
{
	if ((DMA1->ISR & BLOG_UART_DMA_TCIF_BIT) != 0u)
	{
		DMA1->IFCR = BLOG_UART_DMA_TCIF_BIT;
	}

	dma_tx_len = 0u;
	BLOG_UART_DMA_CHANNEL->CCR &= ~(uint32_t)0x01u;
}
