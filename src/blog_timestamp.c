#include "blog_timestamp.h"
#include "cmsis_compiler.h"

#if defined(__CORTEX_M) && (__CORTEX_M >= 3u)
#	include "core_cm3.h"
#endif

void blog_timestamp_init(void)
{
#if defined(__CORTEX_M) && (__CORTEX_M >= 3u)
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0u;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#endif
}

uint32_t blog_timestamp_get(void)
{
#if defined(__CORTEX_M) && (__CORTEX_M >= 3u)
	return DWT->CYCCNT;
#else
	return 0u;
#endif
}
