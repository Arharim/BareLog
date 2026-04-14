#include "blog.h"

int main(void)
{
	blog_init();

	LOG_INFO("UART DMA logger started");
	LOG_DEBUG("clock=%u Hz", 72000000u);
	LOG_WARN("free heap: %u bytes", 4096u);
	LOG_ERROR("sensor err 0x%04x", 0xDEADu);

	uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	LOG_HEXDUMP(BLOG_LEVEL_DEBUG, "packet", buf, sizeof(buf));

	blog_set_module_whitelist(0xFFFFFFFFu);
	blog_set_module_blacklist(0u);
	LOG_INFO("module filter configured");

	blog_set_level(BLOG_LEVEL_WARN);
	LOG_DEBUG("filtered out");
	LOG_WARN("visible warning");

	blog_set_level(BLOG_LEVEL_DEBUG);
	blog_flush_blocking(BLOG_FLUSH_MAX_ITERATIONS);
	blog_deinit();

	while (1)
	{
	}

	return 0;
}
