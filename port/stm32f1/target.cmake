# STM32F1 target configuration (Cortex-M3)
# Used by main CMakeLists.txt when -DUHAL_TARGET=stm32f1

set(UHAL_MCU_FLAGS "-mcpu=cortex-m3" "-mthumb")
set(UHAL_TARGET_DEFINES STM32F103xB STM32F10X_MD)

set(UHAL_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/port/stm32f1/linker_scripts/STM32F103XB_FLASH.ld)
set(UHAL_STARTUP_FILE  ${CMAKE_SOURCE_DIR}/port/stm32f1/startup/startup_stm32f103xb.s)

set(UHAL_PORT_SOURCES
    ${CMAKE_SOURCE_DIR}/port/stm32f1/system_stm32f10x.c
)

set(UHAL_PORT_INCLUDES
    ${CMAKE_SOURCE_DIR}/cmsis/f1/Include
    ${CMAKE_SOURCE_DIR}/port/stm32f1/device
    ${CMAKE_SOURCE_DIR}/port/stm32f1/include
)
