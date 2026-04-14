#include "blog_swo.h"
#include "cmsis_compiler.h"

#if defined(__CORTEX_M) && (__CORTEX_M >= 3u)

#	include "core_cm3.h"

#	define ITM_STIM_U32   (*(volatile uint32_t *)0xE0000000u)
#	define ITM_TER        (*(volatile uint32_t *)0xE0000E00u)
#	define ITM_TCR        (*(volatile uint32_t *)0xE0000E80u)
#	define ITM_TCR_ITMENA (1u << 0u)

void blog_swo_init(void)
{
	ITM_TCR = ITM_TCR_ITMENA;
	ITM_TER = 1u;
}

void blog_swo_deinit(void)
{
	ITM_TCR = 0u;
	ITM_TER = 0u;
}

void blog_swo_putc(char c)
{
	if ((ITM_TCR & ITM_TCR_ITMENA) == 0u)
	{
		return;
	}

	while ((ITM_STIM_U32 & 1u) == 0u)
	{
	}

	ITM_STIM_U32 = (uint32_t)(uint8_t)c;
}

void blog_swo_puts(const char *s, uint16_t len)
{
	uint16_t i;

	for (i = 0u; i < len; i++)
	{
		blog_swo_putc(s[i]);
	}
}

#else

void blog_swo_init(void)
{
}

void blog_swo_deinit(void)
{
}

void blog_swo_putc(char c)
{
	(void)c;
}

void blog_swo_puts(const char *s, uint16_t len)
{
	(void)s;
	(void)len;
}

#endif
