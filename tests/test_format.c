#include "blog_levels.h"
#include "unity.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern uint16_t str_len(const char *s);
extern uint16_t uint32_to_str(char *out, uint32_t val);
extern uint16_t format_prefix(char *out, uint16_t out_size, int level,
                              const char *file, int line);
extern uint16_t format_msg(char *out, uint16_t out_size, const char *fmt,
                           va_list args);

void setUp(void)
{
}
void tearDown(void)
{
}

static void test_str_len_empty(void)
{
	TEST_ASSERT_EQUAL_UINT16(0u, str_len(""));
}

static void test_str_len_hello(void)
{
	TEST_ASSERT_EQUAL_UINT16(5u, str_len("hello"));
}

static void test_uint32_zero(void)
{
	char buf[12u];
	memset(buf, 0, sizeof(buf));
	uint16_t len = uint32_to_str(buf, 0u);
	TEST_ASSERT_EQUAL_UINT16(1u, len);
	TEST_ASSERT_EQUAL_CHAR('0', buf[0u]);
}

static void test_uint32_12345(void)
{
	char buf[12u];
	memset(buf, 0, sizeof(buf));
	uint16_t len = uint32_to_str(buf, 12345u);
	TEST_ASSERT_EQUAL_UINT16(5u, len);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("12345", buf);
}

static void test_uint32_max(void)
{
	char buf[12u];
	memset(buf, 0, sizeof(buf));
	uint16_t len = uint32_to_str(buf, 4294967295u);
	TEST_ASSERT_EQUAL_UINT16(10u, len);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("4294967295", buf);
}

static uint16_t call_format_msg(char *out, uint16_t sz, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	uint16_t len = format_msg(out, sz, fmt, args);
	va_end(args);
	out[len] = '\0';
	return len;
}

static void test_format_plain(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "hello");
	TEST_ASSERT_EQUAL_UINT16(5u, len);
	TEST_ASSERT_EQUAL_STRING("hello", buf);
}

static void test_format_int(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "val=%d", 42);
	TEST_ASSERT_EQUAL_UINT16(6u, len);
	TEST_ASSERT_EQUAL_STRING("val=42", buf);
}

static void test_format_neg_int(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "val=%d", -7);
	TEST_ASSERT_EQUAL_UINT16(6u, len);
	TEST_ASSERT_EQUAL_STRING("val=-7", buf);
}

static void test_format_uint(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "num=%u", 255u);
	TEST_ASSERT_EQUAL_UINT16(7u, len);
	TEST_ASSERT_EQUAL_STRING("num=255", buf);
}

static void test_format_string(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "msg=%s", "ok");
	TEST_ASSERT_EQUAL_UINT16(6u, len);
	TEST_ASSERT_EQUAL_STRING("msg=ok", buf);
}

static void test_format_char(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "ch=%c", 'X');
	TEST_ASSERT_EQUAL_UINT16(4u, len);
	TEST_ASSERT_EQUAL_STRING("ch=X", buf);
}

static void test_format_hex(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "hex=%x", 0xDEADu);
	TEST_ASSERT_EQUAL_UINT16(12u, len);
	TEST_ASSERT_EQUAL_STRING("hex=0000dead", buf);
}

static void test_format_percent(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "100%%");
	TEST_ASSERT_EQUAL_UINT16(4u, len);
	TEST_ASSERT_EQUAL_STRING("100%", buf);
}

static void test_format_mixed(void)
{
	char buf[64u];
	uint16_t len =
	    call_format_msg(buf, sizeof(buf), "a=%d b=%u c=%s", -1, 2u, "hi");
	TEST_ASSERT_EQUAL_UINT16(13u, len);
	TEST_ASSERT_EQUAL_STRING("a=-1 b=2 c=hi", buf);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_str_len_empty);
	RUN_TEST(test_str_len_hello);
	RUN_TEST(test_uint32_zero);
	RUN_TEST(test_uint32_12345);
	RUN_TEST(test_uint32_max);

	RUN_TEST(test_format_plain);
	RUN_TEST(test_format_int);
	RUN_TEST(test_format_neg_int);
	RUN_TEST(test_format_uint);
	RUN_TEST(test_format_string);
	RUN_TEST(test_format_char);
	RUN_TEST(test_format_hex);
	RUN_TEST(test_format_percent);
	RUN_TEST(test_format_mixed);

	return UNITY_END();
}
