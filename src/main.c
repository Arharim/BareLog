#include "blog.h"

int main(void)
{
	blog_init();

	LOG_INFO("System started");
	LOG_DEBUG("value=%d", 42);
	LOG_WARN("low mem: %u bytes", 128u);
	LOG_ERROR("err code 0x%x", 0xDEADu);

	blog_set_level(BLOG_LEVEL_WARN);
	LOG_DEBUG("this will be skipped");
	LOG_WARN("only warnings and errors");

	while (1)
	{
		blog_flush();
	}

	return 0;
}
