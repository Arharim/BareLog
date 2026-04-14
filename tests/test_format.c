#include "blog_levels.h"
#include "unity.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern uint16_t str_len(const char *s);
extern uint16_t str_copy(char *dst, const char *src, uint16_t max);
extern uint16_t uint32_to_str(char *out, uint32_t val);
extern uint16_t uint32_to_str_padded(char *out, uint16_t out_size, uint32_t val,
                                     uint16_t width, uint16_t zero_pad);
extern uint16_t uint32_to_hex(char *out, uint16_t out_size, uint32_t val,
                              const char *hex_table, uint16_t min_digits,
                              uint16_t zero_pad);
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

static uint16_t call_format_msg(char *out, uint16_t sz, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	uint16_t len = format_msg(out, sz, fmt, args);
	va_end(args);
	out[len] = '\0';
	return len;
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

static void test_str_copy_partial(void)
{
	char dst[4u];
	uint16_t len = str_copy(dst, "hello", 3u);
	TEST_ASSERT_EQUAL_UINT16(3u, len);
	TEST_ASSERT_EQUAL_CHAR('h', dst[0u]);
	TEST_ASSERT_EQUAL_CHAR('e', dst[1u]);
	TEST_ASSERT_EQUAL_CHAR('l', dst[2u]);
}

static void test_uint32_to_str_padded_zero(void)
{
	char buf[12u];
	uint16_t len = uint32_to_str_padded(buf, sizeof(buf), 42u, 5u, 1u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_UINT16(5u, len);
	TEST_ASSERT_EQUAL_STRING("00042", buf);
}

static void test_uint32_to_str_padded_space(void)
{
	char buf[12u];
	uint16_t len = uint32_to_str_padded(buf, sizeof(buf), 42u, 5u, 0u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_UINT16(5u, len);
	TEST_ASSERT_EQUAL_STRING("   42", buf);
}

static void test_uint32_to_str_padded_no_pad(void)
{
	char buf[12u];
	uint16_t len = uint32_to_str_padded(buf, sizeof(buf), 12345u, 3u, 1u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_UINT16(5u, len);
	TEST_ASSERT_EQUAL_STRING("12345", buf);
}

static void test_uint32_to_hex_default(void)
{
	char buf[12u];
	static const char hex[] = "0123456789abcdef";
	uint16_t len = uint32_to_hex(buf, sizeof(buf), 0xDEADu, hex, 0u, 0u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("dead", buf);
}

static void test_uint32_to_hex_zero_pad(void)
{
	char buf[12u];
	static const char hex[] = "0123456789abcdef";
	uint16_t len = uint32_to_hex(buf, sizeof(buf), 0xFFu, hex, 4u, 1u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("00ff", buf);
}

static void test_uint32_to_hex_zero(void)
{
	char buf[12u];
	static const char hex[] = "0123456789abcdef";
	uint16_t len = uint32_to_hex(buf, sizeof(buf), 0u, hex, 0u, 0u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("0", buf);
}

static void test_uint32_to_hex_full(void)
{
	char buf[12u];
	static const char hex[] = "0123456789abcdef";
	uint16_t len = uint32_to_hex(buf, sizeof(buf), 0xFFFFFFFFu, hex, 8u, 1u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("ffffffff", buf);
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
	TEST_ASSERT_EQUAL_UINT16(8u, len);
	TEST_ASSERT_EQUAL_STRING("hex=dead", buf);
}

static void test_format_hex_upper(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "HEX=%X", 0xDEADu);
	TEST_ASSERT_EQUAL_UINT16(8u, len);
	TEST_ASSERT_EQUAL_STRING("HEX=DEAD", buf);
}

static void test_format_hex_padded(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "hex=%08x", 0xFFu);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("hex=000000ff", buf);
}

static void test_format_pointer(void)
{
	char buf[64u];
	uint16_t len =
	    call_format_msg(buf, sizeof(buf), "ptr=%p", (void *)0x20000000u);
	TEST_ASSERT_EQUAL_UINT16(14u, len);
	TEST_ASSERT_EQUAL_STRING("ptr=0x20000000", buf);
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

static void test_format_trailing_percent(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "test%");
	TEST_ASSERT_EQUAL_UINT16(4u, len);
	TEST_ASSERT_EQUAL_STRING("test", buf);
}

static void test_format_zero_pad_int(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "val=%05d", 42);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("val=00042", buf);
}

static void test_format_width_uint(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "num=%4u", 7u);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("num=   7", buf);
}

static void test_format_int_zero(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "val=%d", 0);
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING("val=0", buf);
}

static void test_format_prefix_contains_level(void)
{
	char buf[64u];
	uint16_t len =
	    format_prefix(buf, sizeof(buf), BLOG_LEVEL_INFO, "main.c", 10);
	buf[len] = '\0';
	TEST_ASSERT_NOT_NULL(strstr(buf, "INFO"));
	TEST_ASSERT_NOT_NULL(strstr(buf, "main.c"));
	TEST_ASSERT_NOT_NULL(strstr(buf, "10"));
}

static void test_format_prefix_debug(void)
{
	char buf[64u];
	uint16_t len =
	    format_prefix(buf, sizeof(buf), BLOG_LEVEL_DEBUG, "app.c", 1);
	buf[len] = '\0';
	TEST_ASSERT_NOT_NULL(strstr(buf, "DEBUG"));
}

static void test_format_prefix_error(void)
{
	char buf[64u];
	uint16_t len =
	    format_prefix(buf, sizeof(buf), BLOG_LEVEL_ERROR, "err.c", 99);
	buf[len] = '\0';
	TEST_ASSERT_NOT_NULL(strstr(buf, "ERROR"));
	TEST_ASSERT_NOT_NULL(strstr(buf, "99"));
}

static void test_format_prefix_overflow(void)
{
	char buf[8u];
	uint16_t len =
	    format_prefix(buf, sizeof(buf), BLOG_LEVEL_INFO, "main.c", 10);
	TEST_ASSERT_TRUE(len <= sizeof(buf));
}

static void test_format_null_string(void)
{
	char buf[64u];
	uint16_t len = call_format_msg(buf, sizeof(buf), "hello");
	TEST_ASSERT_EQUAL_UINT16(5u, len);
}

static void test_format_buf_overflow(void)
{
	char buf[5u];
	uint16_t len = call_format_msg(buf, sizeof(buf) - 1u, "hello world");
	TEST_ASSERT_EQUAL_UINT16(4u, len);
	TEST_ASSERT_EQUAL_CHAR('h', buf[0u]);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_str_len_empty);
	RUN_TEST(test_str_len_hello);
	RUN_TEST(test_uint32_zero);
	RUN_TEST(test_uint32_12345);
	RUN_TEST(test_uint32_max);
	RUN_TEST(test_str_copy_partial);
	RUN_TEST(test_uint32_to_str_padded_zero);
	RUN_TEST(test_uint32_to_str_padded_space);
	RUN_TEST(test_uint32_to_str_padded_no_pad);
	RUN_TEST(test_uint32_to_hex_default);
	RUN_TEST(test_uint32_to_hex_zero_pad);
	RUN_TEST(test_uint32_to_hex_zero);
	RUN_TEST(test_uint32_to_hex_full);

	RUN_TEST(test_format_plain);
	RUN_TEST(test_format_int);
	RUN_TEST(test_format_neg_int);
	RUN_TEST(test_format_uint);
	RUN_TEST(test_format_string);
	RUN_TEST(test_format_char);
	RUN_TEST(test_format_hex);
	RUN_TEST(test_format_hex_upper);
	RUN_TEST(test_format_hex_padded);
	RUN_TEST(test_format_pointer);
	RUN_TEST(test_format_percent);
	RUN_TEST(test_format_mixed);
	RUN_TEST(test_format_trailing_percent);
	RUN_TEST(test_format_zero_pad_int);
	RUN_TEST(test_format_width_uint);
	RUN_TEST(test_format_int_zero);
	RUN_TEST(test_format_prefix_contains_level);
	RUN_TEST(test_format_prefix_debug);
	RUN_TEST(test_format_prefix_error);
	RUN_TEST(test_format_prefix_overflow);
	RUN_TEST(test_format_null_string);
	RUN_TEST(test_format_buf_overflow);

	return UNITY_END();
}
