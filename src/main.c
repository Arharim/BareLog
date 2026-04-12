#include "blog.h"

int main(void)
{
	blog_init();

	LOG_INFO("System started");
	LOG_DEBUG("value=%d", 42);
	LOG_WARN("low mem: %u bytes", 128u);
	LOG_ERROR("err code 0x%x", 0xDEADu);

	while (1)
	{
		blog_flush();
	}

	return 0;
}
