#ifndef BLOG_RTOS_H
#define BLOG_RTOS_H

#include "blog_config.h"

#if BLOG_RTOS == BLOG_RTOS_NONE

static inline void blog_rtos_lock(void)
{
}

static inline void blog_rtos_unlock(void)
{
}

#elif BLOG_RTOS == BLOG_RTOS_FREERTOS

#	include "FreeRTOS.h"
#	include "semphr.h"

static inline void blog_rtos_lock(void)
{
	extern SemaphoreHandle_t blog_mutex;
	if (blog_mutex != NULL)
	{
		xSemaphoreTakeRecursive(blog_mutex, portMAX_DELAY);
	}
}

static inline void blog_rtos_unlock(void)
{
	extern SemaphoreHandle_t blog_mutex;
	if (blog_mutex != NULL)
	{
		xSemaphoreGiveRecursive(blog_mutex);
	}
}

#elif BLOG_RTOS == BLOG_RTOS_THREADX

#	include "tx_api.h"

static inline void blog_rtos_lock(void)
{
	extern TX_MUTEX blog_mutex;
	tx_mutex_get(&blog_mutex, TX_WAIT_FOREVER);
}

static inline void blog_rtos_unlock(void)
{
	extern TX_MUTEX blog_mutex;
	tx_mutex_put(&blog_mutex);
}

#endif

void blog_rtos_init(void);

#endif
