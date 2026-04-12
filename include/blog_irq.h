#ifndef BLOG_IRQ_H
#define BLOG_IRQ_H

#include <stdint.h>

#if defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_7M__) ||                    \
    defined(__ARM_ARCH_7EM__) || defined(__CORTEX_M)

static inline uint32_t blog_irq_save(void)
{
	uint32_t primask;
	__asm__ volatile("mrs %0, primask" : "=r"(primask));
	__asm__ volatile("cpsid i");
	return primask;
}

static inline void blog_irq_restore(uint32_t primask)
{
	__asm__ volatile("msr primask, %0" ::"r"(primask));
}

#else

static inline uint32_t blog_irq_save(void)
{
	return 0u;
}

static inline void blog_irq_restore(uint32_t primask)
{
	(void)primask;
}

#endif

#endif
