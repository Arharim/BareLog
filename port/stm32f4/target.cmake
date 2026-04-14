# STM32F4 target configuration (Cortex-M4F)
# Used by main CMakeLists.txt when -DUHAL_TARGET=stm32f4
# Target: STM32F407VGT6 (STM32F4-Discovery)

set(UHAL_MCU_FLAGS "-mcpu=cortex-m4" "-mthumb" "-mfloat-abi=hard" "-mfpu=fpv4-sp-d16")
set(UHAL_TARGET_DEFINES STM32F407xx)

set(UHAL_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/port/stm32f4/linker_scripts/STM32F407VGTx_FLASH.ld)
set(UHAL_STARTUP_FILE  ${CMAKE_SOURCE_DIR}/port/stm32f4/startup/startup_stm32f407xx.s)

set(UHAL_PORT_SOURCES
    ${CMAKE_SOURCE_DIR}/port/stm32f4/system_stm32f4xx.c
)

set(UHAL_PORT_INCLUDES
    ${CMAKE_SOURCE_DIR}/cmsis/f4/Include
    ${CMAKE_SOURCE_DIR}/port/stm32f4/device
    ${CMAKE_SOURCE_DIR}/port/stm32f4/include
)

set(UHAL_PORT_BACKEND_DIR ${CMAKE_SOURCE_DIR}/port/stm32f4/src)
