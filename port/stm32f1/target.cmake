# STM32F1 target configuration (Cortex-M3)
# Usage: cmake -DUHAL_TARGET=stm32f1 -DSTM32F1_CHIP=STM32F103CB
#
# Supported chips (STM32F1_CHIP):
#   STM32F100xB  STM32F100xE
#   STM32F101x6  STM32F101xB  STM32F101xE  STM32F101xG
#   STM32F102x6  STM32F102xB
#   STM32F103x6  STM32F103xB  STM32F103xE  STM32F103xG
#   STM32F105xC  STM32F107xC

if(NOT DEFINED STM32F1_CHIP)
	set(STM32F1_CHIP STM32F103xB)
endif()

# ── Chip → device/startup mapping ────────────────────────────────────

string(TOLOWER ${STM32F1_CHIP} F1_DEVICE)

# Validate
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/port/stm32f1/device/${F1_DEVICE}.h)
	message(FATAL_ERROR "Unsupported STM32F1 chip: ${STM32F1_CHIP} (no header ${F1_DEVICE}.h)")
endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/port/stm32f1/startup/startup_${F1_DEVICE}.s)
	message(FATAL_ERROR "Unsupported STM32F1 chip: ${STM32F1_CHIP} (no startup)")
endif()

# ── Chip → memory map ────────────────────────────────────────────────

# Flash size
if(STM32F1_CHIP MATCHES "x6$")
	set(F1_FLASH_KB 32)
elseif(STM32F1_CHIP MATCHES "xB$")
	set(F1_FLASH_KB 128)
elseif(STM32F1_CHIP MATCHES "xE$")
	set(F1_FLASH_KB 512)
elseif(STM32F1_CHIP MATCHES "xG$")
	set(F1_FLASH_KB 1024)
elseif(STM32F1_CHIP MATCHES "xC$")
	set(F1_FLASH_KB 256)
else()
	set(F1_FLASH_KB 128)
endif()

# RAM size
if(STM32F1_CHIP MATCHES "STM32F100xB")
	set(F1_RAM_KB 8)
elseif(STM32F1_CHIP MATCHES "STM32F100xE")
	set(F1_RAM_KB 32)
elseif(STM32F1_CHIP MATCHES "STM32F101x6")
	set(F1_RAM_KB 6)
elseif(STM32F1_CHIP MATCHES "STM32F101xB")
	set(F1_RAM_KB 16)
elseif(STM32F1_CHIP MATCHES "STM32F101xE")
	set(F1_RAM_KB 48)
elseif(STM32F1_CHIP MATCHES "STM32F101xG")
	set(F1_RAM_KB 80)
elseif(STM32F1_CHIP MATCHES "STM32F102x6")
	set(F1_RAM_KB 6)
elseif(STM32F1_CHIP MATCHES "STM32F102xB")
	set(F1_RAM_KB 16)
elseif(STM32F1_CHIP MATCHES "STM32F103x6")
	set(F1_RAM_KB 10)
elseif(STM32F1_CHIP MATCHES "STM32F103xB")
	set(F1_RAM_KB 20)
elseif(STM32F1_CHIP MATCHES "STM32F103xE")
	set(F1_RAM_KB 64)
elseif(STM32F1_CHIP MATCHES "STM32F103xG")
	set(F1_RAM_KB 96)
elseif(STM32F1_CHIP MATCHES "STM32F105xC")
	set(F1_RAM_KB 64)
elseif(STM32F1_CHIP MATCHES "STM32F107xC")
	set(F1_RAM_KB 64)
else()
	set(F1_RAM_KB 20)
endif()

# HCLK (max)
if(STM32F1_CHIP MATCHES "^STM32F100")
	set(F1_HCLK_MHZ 24)
elseif(STM32F1_CHIP MATCHES "^STM32F101x6|^STM32F102x6|^STM32F103x6")
	set(F1_HCLK_MHZ 72)
elseif(STM32F1_CHIP MATCHES "^STM32F101xB|^STM32F102xB|^STM32F103xB")
	set(F1_HCLK_MHZ 72)
elseif(STM32F1_CHIP MATCHES "^STM32F101xE|^STM32F103xE")
	set(F1_HCLK_MHZ 72)
elseif(STM32F1_CHIP MATCHES "^STM32F101xG|^STM32F103xG")
	set(F1_HCLK_MHZ 72)
elseif(STM32F1_CHIP MATCHES "^STM32F105|^STM32F107")
	set(F1_HCLK_MHZ 72)
else()
	set(F1_HCLK_MHZ 72)
endif()

# ── UART clock (APB2 for USART1 on F1) ───────────────────────────────

set(F1_UART_CLOCK_HZ 72000000)

# ── Toolchain settings ───────────────────────────────────────────────

set(UHAL_MCU_FLAGS "-mcpu=cortex-m3" "-mthumb")

# The CMSIS define is mixed-case: STM32F103xE, STM32F107xC, etc.
# Convert user input to proper define format
string(TOUPPER ${F1_DEVICE} F1_DEFINE)
string(REGEX REPLACE "XB$" "xB" F1_DEFINE ${F1_DEFINE})
string(REGEX REPLACE "XE$" "xE" F1_DEFINE ${F1_DEFINE})
string(REGEX REPLACE "XG$" "xG" F1_DEFINE ${F1_DEFINE})
string(REGEX REPLACE "X6$" "x6" F1_DEFINE ${F1_DEFINE})
string(REGEX REPLACE "XC$" "xC" F1_DEFINE ${F1_DEFINE})

set(UHAL_TARGET_DEFINES ${F1_DEFINE})

set(UHAL_STARTUP_FILE ${CMAKE_SOURCE_DIR}/port/stm32f1/startup/startup_${F1_DEVICE}.s)

set(UHAL_LINKER_SCRIPT ${CMAKE_BINARY_DIR}/STM32F1_LINKER.ld)
configure_file(
	${CMAKE_SOURCE_DIR}/port/stm32f1/linker_scripts/STM32F1_FLASH.ld.in
	${UHAL_LINKER_SCRIPT}
	@ONLY
)

set(UHAL_PORT_SOURCES
	${CMAKE_SOURCE_DIR}/port/stm32f1/system_stm32f10x.c
)

set(UHAL_PORT_INCLUDES
	${CMAKE_SOURCE_DIR}/cmsis/f1/Include
	${CMAKE_SOURCE_DIR}/port/stm32f1/device
	${CMAKE_SOURCE_DIR}/port/stm32f1/include
)

message(STATUS "STM32F1 chip: ${STM32F1_CHIP}")
message(STATUS "  Flash:    ${F1_FLASH_KB} KB")
message(STATUS "  RAM:      ${F1_RAM_KB} KB")
message(STATUS "  HCLK:     ${F1_HCLK_MHZ} MHz")
