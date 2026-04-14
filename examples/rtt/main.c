#include "blog.h"

int main(void)
{
	blog_init();

	LOG_INFO("RTT logger started");
	LOG_DEBUG("speed test: %u %u %u", 1u, 2u, 3u);

	uint8_t data[32];
	for (uint16_t i = 0u; i < sizeof(data); i++)
	{
		data[i] = (uint8_t)i;
	}
	LOG_HEXDUMP(BLOG_LEVEL_INFO, "buffer", data, sizeof(data));

	blog_flush_blocking(BLOG_FLUSH_MAX_ITERATIONS);
	blog_deinit();

	while (1)
	{
	}

	return 0;
}
