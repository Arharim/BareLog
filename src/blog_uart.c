#include "blog_uart.h"
#include "blog_config.h"
#include "blog_uart_port.h"
#include "stm32f10x.h"

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

void blog_uart_init(void)
{
	uint16_t brr_val;

	gpio_init();

	RCC->APB2ENR |= BLOG_UART_RCC_BIT;

	brr_val = (uint16_t)(BLOG_UART_CLOCK_HZ / BLOG_UART_BAUDRATE);
	BLOG_UARTx->BRR = brr_val;

	BLOG_UARTx->CR1 = (uint16_t)(USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
}

void blog_uart_putc(char c)
{
	while ((BLOG_UARTx->SR & USART_SR_TXE) == 0u)
	{
	}

	BLOG_UARTx->DR = (uint16_t)c;
}

void blog_uart_puts(const char *s, uint16_t len)
{
	uint16_t i;

	for (i = 0u; i < len; i++)
	{
		blog_uart_putc(s[i]);
	}
}
