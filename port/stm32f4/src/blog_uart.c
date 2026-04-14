#include "blog_uart.h"
#include "blog_config.h"
#include "blog_uart_port.h"
#include "stm32f4xx.h"

static volatile uint16_t dma_tx_len;

static void gpio_init(void)
{
	RCC->AHB1ENR |= BLOG_UART_GPIO_RCC;

	uint32_t tx_shift = (uint32_t)(BLOG_UART_TX_PIN * 2u);
	BLOG_UART_GPIO->MODER &= ~(3u << tx_shift);
	BLOG_UART_GPIO->MODER |= (2u << tx_shift);

	uint32_t rx_shift = (uint32_t)(BLOG_UART_RX_PIN * 2u);
	BLOG_UART_GPIO->MODER &= ~(3u << rx_shift);
	BLOG_UART_GPIO->MODER |= (2u << rx_shift);

	if (BLOG_UART_TX_PIN < 8u)
	{
		uint32_t af_shift = (uint32_t)(BLOG_UART_TX_PIN * 4u);
		BLOG_UART_GPIO->AFR[0] &= ~(0x0Fu << af_shift);
		BLOG_UART_GPIO->AFR[0] |= ((uint32_t)BLOG_UART_AF << af_shift);
	}
	else
	{
		uint32_t af_shift = (uint32_t)((BLOG_UART_TX_PIN - 8u) * 4u);
		BLOG_UART_GPIO->AFR[1] &= ~(0x0Fu << af_shift);
		BLOG_UART_GPIO->AFR[1] |= ((uint32_t)BLOG_UART_AF << af_shift);
	}

	if (BLOG_UART_RX_PIN < 8u)
	{
		uint32_t af_shift = (uint32_t)(BLOG_UART_RX_PIN * 4u);
		BLOG_UART_GPIO->AFR[0] &= ~(0x0Fu << af_shift);
		BLOG_UART_GPIO->AFR[0] |= ((uint32_t)BLOG_UART_AF << af_shift);
	}
	else
	{
		uint32_t af_shift = (uint32_t)((BLOG_UART_RX_PIN - 8u) * 4u);
		BLOG_UART_GPIO->AFR[1] &= ~(0x0Fu << af_shift);
		BLOG_UART_GPIO->AFR[1] |= ((uint32_t)BLOG_UART_AF << af_shift);
	}

	BLOG_UART_GPIO->OSPEEDR |= (3u << tx_shift);
	BLOG_UART_GPIO->PUPDR &= ~(3u << tx_shift);
}

static void dma_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	BLOG_UART_DMA_CHANNEL->CR = 0u;
	dma_tx_len = 0u;

	NVIC_SetPriority(BLOG_UART_DMA_IRQn, 1u);
	NVIC_EnableIRQ(BLOG_UART_DMA_IRQn);
}

void blog_uart_init(void)
{
	gpio_init();
	dma_init();

	RCC->APB1ENR |= BLOG_UART_RCC_BIT;

	uint16_t brr_val = (uint16_t)(BLOG_UART_CLOCK_HZ / BLOG_UART_BAUDRATE);
	BLOG_UARTx->BRR = brr_val;

	BLOG_UARTx->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	BLOG_UARTx->CR3 = USART_CR3_DMAT;
}

void blog_uart_deinit(void)
{
	BLOG_UART_DMA_CHANNEL->CR &= ~DMA_SxCR_EN;
	NVIC_DisableIRQ(BLOG_UART_DMA_IRQn);

	BLOG_UARTx->CR1 &= ~USART_CR1_UE;
	BLOG_UARTx->CR3 &= ~USART_CR3_DMAT;

	dma_tx_len = 0u;
}

void blog_uart_dma_send(const uint8_t *data, uint16_t len)
{
	if (len == 0u)
	{
		return;
	}

	dma_tx_len = len;

	BLOG_UART_DMA_CHANNEL->CR &= ~DMA_SxCR_EN;
	while ((BLOG_UART_DMA_CHANNEL->CR & DMA_SxCR_EN) != 0u)
	{
	}

	BLOG_UART_DMA_CHANNEL->PAR = (uint32_t)&(BLOG_UARTx->DR);
	BLOG_UART_DMA_CHANNEL->M0AR = (uint32_t)data;
	BLOG_UART_DMA_CHANNEL->NDTR = (uint32_t)len;
	BLOG_UART_DMA_CHANNEL->CR =
	    (uint32_t)((BLOG_UART_DMA_CHANNEL_SEL << 25u) | (1u << 6u) |
	               (1u << 4u) | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE |
	               DMA_SxCR_EN);
}

uint16_t blog_uart_dma_running(void)
{
	if ((BLOG_UART_DMA_CHANNEL->CR & DMA_SxCR_EN) != 0u)
	{
		return 1u;
	}

	return 0u;
}

void blog_uart_dma_irq_handler(void)
{
	if ((DMA2->HISR & BLOG_UART_DMA_TCIF_BIT) != 0u)
	{
		DMA2->HIFCR = BLOG_UART_DMA_TCIF_BIT;
	}

	dma_tx_len = 0u;
	BLOG_UART_DMA_CHANNEL->CR &= ~DMA_SxCR_EN;
}
