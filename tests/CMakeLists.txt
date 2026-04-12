cmake_minimum_required(VERSION 3.20)
project(BareLogTests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(unity
	GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
	GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

add_executable(test_ringbuf
	test_ringbuf.c
	${CMAKE_SOURCE_DIR}/../src/blog_ringbuf.c
)

target_include_directories(test_ringbuf PRIVATE
	${CMAKE_SOURCE_DIR}/../include
)

target_compile_definitions(test_ringbuf PRIVATE
	BLOG_RINGBUF_SIZE=16
)

target_link_libraries(test_ringbuf unity)

enable_testing()
add_test(NAME ringbuf COMMAND test_ringbuf)
