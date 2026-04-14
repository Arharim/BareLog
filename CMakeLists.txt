cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED UHAL_TARGET)
	set(UHAL_TARGET stm32f1)
endif()

project(BareLog C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/port/${UHAL_TARGET}/target.cmake)

set(RTT_DIR ${CMAKE_SOURCE_DIR}/ext/segger-rtt/RTT)

set(BLOG_CORE_SOURCES
	${CMAKE_SOURCE_DIR}/src/blog.c
	${CMAKE_SOURCE_DIR}/src/blog_ringbuf.c
	${CMAKE_SOURCE_DIR}/src/blog_timestamp.c
	${CMAKE_SOURCE_DIR}/src/blog_rtos.c
)

set(BLOG_BACKEND_SOURCES "")
set(BLOG_EXTRA_INCLUDES "")

if(NOT DEFINED BLOG_BACKEND)
	set(BLOG_BACKEND BLOG_BACKEND_UART_DMA)
endif()

if(BLOG_BACKEND STREQUAL "BLOG_BACKEND_UART_DMA")
	if(DEFINED UHAL_PORT_BACKEND_DIR AND EXISTS ${UHAL_PORT_BACKEND_DIR}/blog_uart.c)
		list(APPEND BLOG_BACKEND_SOURCES ${UHAL_PORT_BACKEND_DIR}/blog_uart.c)
	else()
		list(APPEND BLOG_BACKEND_SOURCES ${CMAKE_SOURCE_DIR}/src/blog_uart.c)
	endif()
elseif(BLOG_BACKEND STREQUAL "BLOG_BACKEND_SWO")
	list(APPEND BLOG_BACKEND_SOURCES ${CMAKE_SOURCE_DIR}/src/blog_swo.c)
elseif(BLOG_BACKEND STREQUAL "BLOG_BACKEND_RTT")
	list(APPEND BLOG_BACKEND_SOURCES
		${CMAKE_SOURCE_DIR}/src/blog_rtt.c
		${RTT_DIR}/SEGGER_RTT.c
	)
	list(APPEND BLOG_EXTRA_INCLUDES ${RTT_DIR})
elseif(BLOG_BACKEND STREQUAL "BLOG_BACKEND_FLASH")
	if(DEFINED UHAL_PORT_BACKEND_DIR AND EXISTS ${UHAL_PORT_BACKEND_DIR}/blog_flash.c)
		list(APPEND BLOG_BACKEND_SOURCES ${UHAL_PORT_BACKEND_DIR}/blog_flash.c)
	else()
		list(APPEND BLOG_BACKEND_SOURCES ${CMAKE_SOURCE_DIR}/src/blog_flash.c)
	endif()
endif()

set(BLOG_INCLUDES
	${CMAKE_SOURCE_DIR}/include
	${BLOG_EXTRA_INCLUDES}
)

add_library(blog STATIC ${BLOG_CORE_SOURCES} ${BLOG_BACKEND_SOURCES})

target_include_directories(blog PUBLIC
	${BLOG_INCLUDES}
	${UHAL_PORT_INCLUDES}
)

target_compile_definitions(blog PUBLIC
	${UHAL_TARGET_DEFINES}
	BLOG_BACKEND=${BLOG_BACKEND}
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
)

if(DEFINED UHAL_PORT_BACKEND_DIR AND EXISTS ${UHAL_PORT_BACKEND_DIR}/blog_isr.c)
	target_sources(${PROJECT_NAME}.elf PRIVATE ${UHAL_PORT_BACKEND_DIR}/blog_isr.c)
else()
	target_sources(${PROJECT_NAME}.elf PRIVATE ${CMAKE_SOURCE_DIR}/src/blog_isr.c)
endif()

target_link_libraries(${PROJECT_NAME}.elf PRIVATE blog)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
	${BLOG_INCLUDES}
	${UHAL_PORT_INCLUDES}
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
	${UHAL_TARGET_DEFINES}
	BLOG_BACKEND=${BLOG_BACKEND}
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
