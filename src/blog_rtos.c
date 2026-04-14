#include "blog_rtos.h"

#if BLOG_RTOS == BLOG_RTOS_FREERTOS
#	include "FreeRTOS.h"
#	include "semphr.h"

SemaphoreHandle_t blog_mutex;

void blog_rtos_init(void)
{
	blog_mutex = xSemaphoreCreateRecursiveMutex();
}

#elif BLOG_RTOS == BLOG_RTOS_THREADX
#	include "tx_api.h"

TX_MUTEX blog_mutex;

void blog_rtos_init(void)
{
	tx_mutex_create(&blog_mutex, "BareLog", TX_NO_INHERIT);
}

#else

void blog_rtos_init(void)
{
}

#endif
