#include "blog.h"

int main(void)
{
	blog_init();

	LOG_INFO("Flash logger started");
	LOG_ERROR("crash dump: pc=0x%08x lr=0x%08x", 0x08001234u, 0x08005678u);
	LOG_WARN("stack: 0x%08x", 0x20001000u);

	uint8_t regs[] = {0xFF, 0x00, 0xAB, 0xCD};
	LOG_HEXDUMP(BLOG_LEVEL_ERROR, "registers", regs, sizeof(regs));

	blog_flush_blocking(BLOG_FLUSH_MAX_ITERATIONS);
	blog_deinit();

	while (1)
	{
	}

	return 0;
}
