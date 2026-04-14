#ifndef BLOG_UART_PORT_H
#define BLOG_UART_PORT_H

/*
 * STM32F4 UART DMA default configuration
 * USART2 on PA2/PA3, DMA2 Stream6 Channel 4
 *
 * Override by defining BLOG_UART_CLOCK_HZ before inclusion
 * or redefining these macros in your build.
 */

#ifndef BLOG_UARTx
#	define BLOG_UARTx USART2
#endif

#ifndef BLOG_UART_RCC_BIT
#	define BLOG_UART_RCC_BIT RCC_APB1ENR_USART2EN
#endif

#ifndef BLOG_UART_GPIO
#	define BLOG_UART_GPIO GPIOA
#endif

#ifndef BLOG_UART_GPIO_RCC
#	define BLOG_UART_GPIO_RCC RCC_AHB1ENR_GPIOAEN
#endif

#ifndef BLOG_UART_TX_PIN
#	define BLOG_UART_TX_PIN 2u
#endif

#ifndef BLOG_UART_RX_PIN
#	define BLOG_UART_RX_PIN 3u
#endif

#ifndef BLOG_UART_AF
#	define BLOG_UART_AF 7u
#endif

#ifndef BLOG_UART_CLOCK_HZ
#	define BLOG_UART_CLOCK_HZ 84000000u
#endif

#ifndef BLOG_UART_DMA_CHANNEL
#	define BLOG_UART_DMA_CHANNEL DMA2_Stream6
#endif

#ifndef BLOG_UART_DMA_IRQn
#	define BLOG_UART_DMA_IRQn DMA2_Stream6_IRQn
#endif

#ifndef BLOG_UART_DMA_TCIF_BIT
#	define BLOG_UART_DMA_TCIF_BIT (1u << 22u)
#endif

#ifndef BLOG_UART_DMA_CHANNEL_SEL
#	define BLOG_UART_DMA_CHANNEL_SEL 4u
#endif

#endif
