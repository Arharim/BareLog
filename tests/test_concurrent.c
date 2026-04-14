#include "blog_ringbuf.h"
#include "unity.h"
#include <pthread.h>
#include <stdint.h>
#include <string.h>

static blog_ringbuf_t rb;

void setUp(void)
{
	blog_ringbuf_init(&rb);
}

void tearDown(void)
{
}

static void *writer_thread(void *arg)
{
	(void)arg;
	uint8_t data[4] = {0xAAu, 0xBBu, 0xCCu, 0xDDu};
	uint32_t i;

	for (i = 0u; i < 10000u; i++)
	{
		blog_ringbuf_push(&rb, data, 4u);
	}

	return NULL;
}

static void *reader_thread(void *arg)
{
	(void)arg;
	uint8_t buf[8u];
	uint32_t total = 0u;
	uint32_t target = 40000u;

	while (total < target)
	{
		uint16_t n = blog_ringbuf_pop(&rb, buf, sizeof(buf));
		total += (uint32_t)n;
		if (n == 0u)
		{
			uint16_t dropped = blog_ringbuf_get_dropped(&rb);
			if (total + (uint32_t)dropped >= target)
			{
				break;
			}
			sched_yield();
		}
	}

	return NULL;
}

static void test_concurrent_single_writer_single_reader(void)
{
	pthread_t writer, reader;
	uint16_t avail_before;

	blog_ringbuf_init(&rb);
	avail_before = blog_ringbuf_available(&rb);
	TEST_ASSERT_EQUAL_UINT16(0u, avail_before);

	pthread_create(&writer, NULL, writer_thread, NULL);
	pthread_create(&reader, NULL, reader_thread, NULL);

	pthread_join(writer, NULL);
	pthread_join(reader, NULL);

	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_available(&rb));
}

#define NUM_WRITERS 4

static void *multi_writer_func(void *arg)
{
	uint8_t id = (uint8_t)(uintptr_t)arg;
	uint8_t data[2] = {id, id};
	uint32_t i;

	for (i = 0u; i < 5000u; i++)
	{
		blog_ringbuf_push(&rb, data, 2u);
	}

	return NULL;
}

static void *multi_reader_func(void *arg)
{
	(void)arg;
	uint8_t buf[8u];
	uint32_t total = 0u;
	uint32_t target = (uint32_t)(NUM_WRITERS * 5000u * 2u);

	while (total < target)
	{
		uint16_t n = blog_ringbuf_pop(&rb, buf, sizeof(buf));
		total += (uint32_t)n;
		if (n == 0u)
		{
			uint16_t dropped = blog_ringbuf_get_dropped(&rb);
			if (total + (uint32_t)dropped >= target)
			{
				break;
			}
			sched_yield();
		}
	}

	return NULL;
}

static void test_concurrent_multi_writer(void)
{
	pthread_t writers[NUM_WRITERS];
	pthread_t reader;
	uint16_t i;

	blog_ringbuf_init(&rb);

	for (i = 0u; i < NUM_WRITERS; i++)
	{
		pthread_create(&writers[i], NULL, multi_writer_func,
		               (void *)(uintptr_t)(i + 1u));
	}
	pthread_create(&reader, NULL, multi_reader_func, NULL);

	for (i = 0u; i < NUM_WRITERS; i++)
	{
		pthread_join(writers[i], NULL);
	}
	pthread_join(reader, NULL);
}

static void *stress_writer(void *arg)
{
	(void)arg;
	uint8_t data[1] = {0x42u};
	uint32_t i;

	for (i = 0u; i < 100000u; i++)
	{
		blog_ringbuf_push(&rb, data, 1u);
	}

	return NULL;
}

static void *stress_reader(void *arg)
{
	(void)arg;
	uint8_t buf[16u];
	uint32_t total = 0u;
	uint32_t target = 100000u;

	while (total < target)
	{
		uint16_t n = blog_ringbuf_pop(&rb, buf, sizeof(buf));
		total += (uint32_t)n;
		if (n == 0u)
		{
			uint16_t dropped = blog_ringbuf_get_dropped(&rb);
			if (total + (uint32_t)dropped >= target)
			{
				break;
			}
			sched_yield();
		}
	}

	return NULL;
}

static void test_concurrent_stress(void)
{
	pthread_t writer, reader;

	blog_ringbuf_init(&rb);

	pthread_create(&writer, NULL, stress_writer, NULL);
	pthread_create(&reader, NULL, stress_reader, NULL);

	pthread_join(writer, NULL);
	pthread_join(reader, NULL);
}

static void test_concurrent_no_data_loss(void)
{
	pthread_t writer, reader;
	uint8_t data[2] = {0xA5u, 0xA5u};
	uint8_t buf[4u];
	uint32_t written = 0u;
	uint32_t read_back = 0u;
	uint32_t i;

	blog_ringbuf_init(&rb);

	for (i = 0u; i < 1000u; i++)
	{
		blog_ringbuf_push(&rb, data, 2u);
		written += 2u;

		while (blog_ringbuf_available(&rb) > 0u)
		{
			uint16_t n = blog_ringbuf_pop(&rb, buf, sizeof(buf));
			read_back += (uint32_t)n;
		}
	}

	uint16_t dropped = blog_ringbuf_get_dropped(&rb);
	TEST_ASSERT_EQUAL_UINT32(written, read_back + (uint32_t)dropped);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_concurrent_single_writer_single_reader);
	RUN_TEST(test_concurrent_multi_writer);
	RUN_TEST(test_concurrent_stress);
	RUN_TEST(test_concurrent_no_data_loss);

	return UNITY_END();
}
