# BareLog

Minimal bare-metal logging library for ARM Cortex-M microcontrollers.

Written in C11 with zero heap usage, no stdio dependency, and a custom `printf` engine.

## Features

- **4 backends**: UART DMA, SWO/ITM, SEGGER RTT, Flash (crash persistence)
- **ISR-safe**: separate `LOG_*_ISR()` macros for interrupt context
- **RTOS support**: optional FreeRTOS / ThreadX mutex-based flush
- **Ring buffer**: lock-free DMA-friendly circular buffer with overflow tracking
- **Runtime log level**: `blog_set_level()` / `blog_get_level()`
- **Module filtering**: whitelist/blacklist up to 32 modules at runtime
- **Custom printf**: `%d` `%ld` `%lld` `%u` `%lu` `%s` `%c` `%x` `%lx` `%X` `%p` `%%` with width and zero-pad
- **Hex dump**: `LOG_HEXDUMP()` with offset + hex + ASCII columns
- **Timestamps**: DWT Cycle Counter (optional)
- **ANSI colors**: optional terminal color codes
- **Tiny footprint**: ~2-4 KB ROM depending on backend

## Supported Targets

| Family | Chips | Flash | RAM | Max HCLK |
|--------|-------|-------|-----|----------|
| **STM32F1** | F100, F101, F102, F103, F105, F107 (14 variants) | 32K-1024K | 6K-96K | 72 MHz |
| **STM32F4** | F401, F405, F407, F410, F411, F412, F413, F423, F427, F429, F437, F439, F446, F469, F479 (40+ chips) | 64K-2048K | 32K-384K | 84-180 MHz |

Select a chip via CMake — linker script, startup, device header, and clocks are configured automatically.

## Quick Start

### 1. Add to your project

```cmake
add_subdirectory(path/to/BareLog)
target_link_libraries(your_app PRIVATE blog)
```

### 2. Configure

Select target chip and backend:

```bash
# STM32F103CB (Blue Pill)
cmake -B build --toolchain toolchain-arm.cmake \
    -DSTM32F1_CHIP=STM32F103xB -DBLOG_BACKEND=BLOG_BACKEND_UART_DMA .

# STM32F429VI (Discovery)
cmake -B build --toolchain toolchain-arm.cmake \
    -DUHAL_TARGET=stm32f4 -DSTM32F4_CHIP=STM32F429VI -DBLOG_BACKEND=BLOG_BACKEND_SWO .

# STM32F446RE (Nucleo)
cmake -B build --toolchain toolchain-arm.cmake \
    -DUHAL_TARGET=stm32f4 -DSTM32F4_CHIP=STM32F446RE .
```

Additional compile-time options:

| Define | Default | Description |
|--------|---------|-------------|
| `BLOG_BACKEND` | `1` (UART DMA) | `1`=UART DMA, `2`=SWO, `3`=RTT, `4`=Flash |
| `BLOG_LEVEL` | `0` (DEBUG) | Compile-time log level filter |
| `BLOG_RINGBUF_SIZE` | `256` | Ring buffer size (must be power of 2) |
| `BLOG_UART_BAUDRATE` | `115200` | UART baud rate |
| `BLOG_ENABLE_TIMESTAMP` | `0` | Enable DWT timestamp in prefix |
| `BLOG_ENABLE_COLOR` | `0` | Enable ANSI color codes |
| `BLOG_MODULE` | `0` | Module ID (0 = disabled) |
| `BLOG_RTOS` | `0` (none) | `0`=bare, `1`=FreeRTOS, `2`=ThreadX |

### 3. Use

```c
#include "blog.h"

int main(void)
{
    blog_init();

    LOG_INFO("System started, clock=%u", SystemCoreClock);
    LOG_DEBUG("value=0x%08x", reg);
    LOG_WARN("Low memory: %u bytes free", free);
    LOG_ERROR("Sensor %d failed", id);

    uint8_t buf[64];
    LOG_HEXDUMP(BLOG_LEVEL_INFO, "RX", buf, sizeof(buf));

    blog_flush_blocking(BLOG_FLUSH_MAX_ITERATIONS);
    blog_deinit();
}
```

### ISR context

```c
void DMA1_Channel4_IRQHandler(void)
{
    LOG_DEBUG_ISR("ISR flag=0x%x", flags);
}
```

### Module filtering

```c
#define BLOG_MODULE 1u  // in module source file

// In main:
blog_set_module_whitelist(0x03);  // enable modules 1 and 2
blog_set_module_blacklist(0x04);  // disable module 3
```

### RTOS integration

```cmake
# CMakeLists.txt
target_compile_definitions(blog PUBLIC BLOG_RTOS=1)  # FreeRTOS
```

```c
// FreeRTOS: call blog_init() after scheduler starts
blog_init();  // creates recursive mutex automatically
```

## Examples

Minimal `main.c` for each backend in `examples/`:

```
examples/uart_dma/   Full demo: levels, hexdump, module filter, deinit
examples/swo/        Minimal SWO output loop
examples/rtt/        RTT speed test with hexdump
examples/flash/      Crash log write + register dump
```

## ROM/RAM Footprint

Automatically measured by CI for all 4 backends (STM32F103xB, `-Os`).

| Backend | ROM (text) | RAM (data+bss) |
|---------|-----------|----------------|
| UART DMA | 2884 B | 1944 B |
| SWO | 2652 B | 1936 B |
| SEGGER RTT | 3520 B | 2280 B |
| Flash | 2864 B | 1944 B |

## Backend Comparison

| Backend | Speed | Requires | Use case |
|---------|-------|----------|----------|
| UART DMA | 115200+ baud | UART pins | Production, general purpose |
| SWO/ITM | ~2 MHz | Debug probe + SWO pin | Development, no UART needed |
| SEGGER RTT | ~1 MB/s | J-Link probe | High-speed development |
| Flash | Write-only | Flash sector | Crash log persistence |

## Building

```bash
just test                    # Host tests (Unity + pthreads)
just build-arm STM32F103xE   # STM32F1 (default: STM32F103xB)
just build-arm-f4 STM32F429VI # STM32F4 (default: STM32F407VG)
just build-all               # All 4 backends
just size                    # Show arm-none-eabi-size for all
just flash-jlink             # Flash via J-Link
just flash-stlink            # Flash via ST-Link/OpenOCD
```

## Project Structure

```
include/          Public API headers
src/              Core + backend implementations
port/stm32f1/     STM32F1 full lineup (14 variants)
port/stm32f4/     STM32F4 full lineup (40+ chips)
cmsis/            CMSIS core headers (F0, F1, F4)
ext/              External: SEGGER RTT
tests/            Unity test suite (host, 57 tests)
tools/            Flash decode utility
examples/         Per-backend minimal examples
```

## License

BSD-3-Clause. See [LICENSE](LICENSE).
