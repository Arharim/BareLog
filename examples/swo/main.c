#include "blog.h"

int main(void)
{
	blog_init();

	LOG_INFO("SWO logger started");
	LOG_DEBUG("core clock=%u", 72000000u);

	for (uint32_t i = 0u; i < 5u; i++)
	{
		LOG_INFO("iteration %u", i);
	}

	LOG_WARN("done");
	blog_flush_blocking(BLOG_FLUSH_MAX_ITERATIONS);
	blog_deinit();

	while (1)
	{
	}

	return 0;
}
