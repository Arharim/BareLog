#include "blog_ringbuf.h"
#include "unity.h"

static blog_ringbuf_t rb;

void setUp(void)
{
	blog_ringbuf_init(&rb);
}

void tearDown(void)
{
}

static void test_init_empty(void)
{
	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_available(&rb));
	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_get_dropped(&rb));
}

static void test_push_pop_single(void)
{
	uint8_t data = 0xABu;
	uint8_t out = 0u;
	uint16_t pushed;
	uint16_t popped;

	pushed = blog_ringbuf_push(&rb, &data, 1u);
	TEST_ASSERT_EQUAL_UINT16(1u, pushed);
	TEST_ASSERT_EQUAL_UINT16(1u, blog_ringbuf_available(&rb));

	popped = blog_ringbuf_pop(&rb, &out, 1u);
	TEST_ASSERT_EQUAL_UINT16(1u, popped);
	TEST_ASSERT_EQUAL_UINT8(0xABu, out);
	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_available(&rb));
}

static void test_push_pop_multiple(void)
{
	uint8_t data[] = {1u, 2u, 3u, 4u, 5u};
	uint8_t out[5u];
	uint16_t pushed;
	uint16_t popped;

	pushed = blog_ringbuf_push(&rb, data, 5u);
	TEST_ASSERT_EQUAL_UINT16(5u, pushed);
	TEST_ASSERT_EQUAL_UINT16(5u, blog_ringbuf_available(&rb));

	popped = blog_ringbuf_pop(&rb, out, 5u);
	TEST_ASSERT_EQUAL_UINT16(5u, popped);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(data, out, 5u);
}

static void test_full_wrap(void)
{
	uint8_t data[16u];
	uint8_t out[16u];
	uint16_t pushed;
	uint16_t popped;
	uint16_t i;

	for (i = 0u; i < 16u; i++)
	{
		data[i] = (uint8_t)(i + 10u);
	}

	pushed = blog_ringbuf_push(&rb, data, 16u);
	TEST_ASSERT_EQUAL_UINT16(15u, pushed);

	popped = blog_ringbuf_pop(&rb, out, 15u);
	TEST_ASSERT_EQUAL_UINT16(15u, popped);

	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_available(&rb));
}

static void test_push_full_rejects(void)
{
	uint8_t data[20u];
	uint16_t pushed;
	uint16_t i;

	for (i = 0u; i < 20u; i++)
	{
		data[i] = (uint8_t)i;
	}

	pushed = blog_ringbuf_push(&rb, data, 20u);
	TEST_ASSERT_EQUAL_UINT16(15u, pushed);
}

static void test_pop_empty(void)
{
	uint8_t out = 0u;
	uint16_t popped;

	popped = blog_ringbuf_pop(&rb, &out, 1u);
	TEST_ASSERT_EQUAL_UINT16(0u, popped);
}

static void test_flush(void)
{
	uint8_t data[] = {1u, 2u, 3u};

	blog_ringbuf_push(&rb, data, 3u);
	TEST_ASSERT_EQUAL_UINT16(3u, blog_ringbuf_available(&rb));

	blog_ringbuf_flush(&rb);
	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_available(&rb));
}

static void test_partial_pop(void)
{
	uint8_t data[] = {10u, 20u, 30u, 40u};
	uint8_t out[2u];
	uint16_t popped;

	blog_ringbuf_push(&rb, data, 4u);

	popped = blog_ringbuf_pop(&rb, out, 2u);
	TEST_ASSERT_EQUAL_UINT16(2u, popped);
	TEST_ASSERT_EQUAL_UINT8(10u, out[0u]);
	TEST_ASSERT_EQUAL_UINT8(20u, out[1u]);
	TEST_ASSERT_EQUAL_UINT16(2u, blog_ringbuf_available(&rb));

	popped = blog_ringbuf_pop(&rb, out, 2u);
	TEST_ASSERT_EQUAL_UINT16(2u, popped);
	TEST_ASSERT_EQUAL_UINT8(30u, out[0u]);
	TEST_ASSERT_EQUAL_UINT8(40u, out[1u]);
}

static void test_dropped_counter(void)
{
	uint8_t data[20u];
	uint16_t i;

	for (i = 0u; i < 20u; i++)
	{
		data[i] = (uint8_t)i;
	}

	blog_ringbuf_push(&rb, data, 20u);
	TEST_ASSERT_EQUAL_UINT16(5u, blog_ringbuf_get_dropped(&rb));
}

static void test_dropped_accumulate(void)
{
	uint8_t data[20u];
	uint16_t i;

	for (i = 0u; i < 20u; i++)
	{
		data[i] = (uint8_t)i;
	}

	blog_ringbuf_push(&rb, data, 20u);
	TEST_ASSERT_EQUAL_UINT16(5u, blog_ringbuf_get_dropped(&rb));

	blog_ringbuf_pop(&rb, data, 15u);

	blog_ringbuf_push(&rb, data, 20u);
	TEST_ASSERT_EQUAL_UINT16(10u, blog_ringbuf_get_dropped(&rb));
}

static void test_dropped_saturate(void)
{
	uint8_t data[16u];
	uint16_t i;

	for (i = 0u; i < 16u; i++)
	{
		data[i] = (uint8_t)i;
	}

	blog_ringbuf_push(&rb, data, 16u);

	for (i = 0u; i < 5000u; i++)
	{
		blog_ringbuf_push(&rb, data, 16u);
	}

	TEST_ASSERT_EQUAL_UINT16(65535u, blog_ringbuf_get_dropped(&rb));
}

static void test_clear_dropped(void)
{
	uint8_t data[20u];
	uint16_t i;

	for (i = 0u; i < 20u; i++)
	{
		data[i] = (uint8_t)i;
	}

	blog_ringbuf_push(&rb, data, 20u);
	TEST_ASSERT_TRUE(blog_ringbuf_get_dropped(&rb) > 0u);

	blog_ringbuf_clear_dropped(&rb);
	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_get_dropped(&rb));
}

static void test_flush_clears_dropped(void)
{
	uint8_t data[20u];
	uint16_t i;

	for (i = 0u; i < 20u; i++)
	{
		data[i] = (uint8_t)i;
	}

	blog_ringbuf_push(&rb, data, 20u);
	blog_ringbuf_flush(&rb);
	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_get_dropped(&rb));
}

static void test_isr_push_pop(void)
{
	uint8_t data[] = {0xAAu, 0xBBu, 0xCCu};
	uint8_t out[3u];
	uint16_t pushed;
	uint16_t popped;

	pushed = blog_ringbuf_push_isr(&rb, data, 3u);
	TEST_ASSERT_EQUAL_UINT16(3u, pushed);

	popped = blog_ringbuf_pop_isr(&rb, out, 3u);
	TEST_ASSERT_EQUAL_UINT16(3u, popped);
	TEST_ASSERT_EQUAL_UINT8(0xAAu, out[0u]);
	TEST_ASSERT_EQUAL_UINT8(0xBBu, out[1u]);
	TEST_ASSERT_EQUAL_UINT8(0xCCu, out[2u]);
}

static void test_push_zero_len(void)
{
	uint8_t data = 0u;
	uint16_t pushed = blog_ringbuf_push(&rb, &data, 0u);
	TEST_ASSERT_EQUAL_UINT16(0u, pushed);
	TEST_ASSERT_EQUAL_UINT16(0u, blog_ringbuf_available(&rb));
}

static void test_pop_zero_len(void)
{
	uint8_t out = 0u;
	uint16_t popped = blog_ringbuf_pop(&rb, &out, 0u);
	TEST_ASSERT_EQUAL_UINT16(0u, popped);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_init_empty);
	RUN_TEST(test_push_pop_single);
	RUN_TEST(test_push_pop_multiple);
	RUN_TEST(test_full_wrap);
	RUN_TEST(test_push_full_rejects);
	RUN_TEST(test_pop_empty);
	RUN_TEST(test_flush);
	RUN_TEST(test_partial_pop);
	RUN_TEST(test_dropped_counter);
	RUN_TEST(test_dropped_accumulate);
	RUN_TEST(test_dropped_saturate);
	RUN_TEST(test_clear_dropped);
	RUN_TEST(test_flush_clears_dropped);
	RUN_TEST(test_isr_push_pop);
	RUN_TEST(test_push_zero_len);
	RUN_TEST(test_pop_zero_len);

	return UNITY_END();
}
