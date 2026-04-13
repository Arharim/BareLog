cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED UHAL_TARGET)
	set(UHAL_TARGET stm32f1)
endif()

project(BareLog C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/port/${UHAL_TARGET}/target.cmake)

set(RTT_DIR ${CMAKE_SOURCE_DIR}/ext/segger-rtt/RTT)

set(BLOG_SOURCES
	${CMAKE_SOURCE_DIR}/src/blog.c
	${CMAKE_SOURCE_DIR}/src/blog_ringbuf.c
	${CMAKE_SOURCE_DIR}/src/blog_uart.c
	${CMAKE_SOURCE_DIR}/src/blog_swo.c
	${CMAKE_SOURCE_DIR}/src/blog_flash.c
	${CMAKE_SOURCE_DIR}/src/blog_timestamp.c
	${CMAKE_SOURCE_DIR}/src/blog_rtt.c
	${RTT_DIR}/SEGGER_RTT.c
)

set(BLOG_INCLUDES
	${CMAKE_SOURCE_DIR}/include
	${RTT_DIR}
)

add_library(blog STATIC ${BLOG_SOURCES})

target_include_directories(blog PUBLIC
	${BLOG_INCLUDES}
	${UHAL_PORT_INCLUDES}
)

target_compile_definitions(blog PUBLIC
	${UHAL_TARGET_DEFINES}
)

target_compile_options(blog PRIVATE
	${UHAL_MCU_FLAGS}
	-Wall -Wextra -Wpedantic
	-ffreestanding -nostdlib
	-Os -ffunction-sections -fdata-sections
)

add_executable(${PROJECT_NAME}.elf
	${UHAL_STARTUP_FILE}
	${UHAL_PORT_SOURCES}
	${CMAKE_SOURCE_DIR}/src/main.c
	${CMAKE_SOURCE_DIR}/src/syscalls.c
	${CMAKE_SOURCE_DIR}/src/blog_isr.c
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE blog)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
	${BLOG_INCLUDES}
	${UHAL_PORT_INCLUDES}
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
	${UHAL_TARGET_DEFINES}
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
	${UHAL_MCU_FLAGS}
	-Wall -Wextra -Wpedantic
	-ffreestanding -nostdlib
	-Os -ffunction-sections -fdata-sections
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
	${UHAL_MCU_FLAGS}
	-T${UHAL_LINKER_SCRIPT}
	-Wl,--gc-sections -Wl,--print-memory-usage
	-nostartfiles -specs=nosys.specs
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf>
		${PROJECT_NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf>
		${PROJECT_NAME}.bin
	COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
)
