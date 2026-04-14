# BareLog

Minimal bare-metal logging library for ARM Cortex-M microcontrollers.

Written in C11 with zero heap usage, no stdio dependency, and a custom `printf` engine.

## Features

- **4 backends**: UART DMA, SWO/ITM, SEGGER RTT, Flash (crash persistence)
- **ISR-safe**: separate `LOG_*_ISR()` macros for interrupt context
- **Ring buffer**: lock-free DMA-friendly circular buffer with overflow tracking
- **Runtime log level**: `blog_set_level()` / `blog_get_level()`
- **Module filtering**: whitelist/blacklist up to 32 modules at runtime
- **Custom printf**: `%d` `%u` `%s` `%c` `%x` `%X` `%p` `%%` with width and zero-pad (`%08x`, `%4d`)
- **Hex dump**: `LOG_HEXDUMP()` with offset + hex + ASCII columns
- **Timestamps**: DWT Cycle Counter (optional)
- **ANSI colors**: optional terminal color codes
- **Tiny footprint**: ~2-4 KB ROM depending on backend

## Quick Start

### 1. Add to your project

```cmake
# Add as subdirectory or install
add_subdirectory(path/to/BareLog)

# Link to your target
target_link_libraries(your_app PRIVATE blog)
```

### 2. Configure

Optionally define before inclusion or via CMake:

| Define | Default | Description |
|--------|---------|-------------|
| `BLOG_BACKEND` | `1` (UART DMA) | `1`=UART DMA, `2`=SWO, `3`=RTT, `4`=Flash |
| `BLOG_LEVEL` | `0` (DEBUG) | Compile-time log level filter |
| `BLOG_RINGBUF_SIZE` | `256` | Ring buffer size (must be power of 2) |
| `BLOG_UART_BAUDRATE` | `115200` | UART baud rate |
| `BLOG_ENABLE_TIMESTAMP` | `0` | Enable DWT timestamp in prefix |
| `BLOG_ENABLE_COLOR` | `0` | Enable ANSI color codes |
| `BLOG_MODULE` | `0` | Module ID (0 = disabled) |

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
void USART1_IRQHandler(void)
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

## Examples

Minimal `main.c` for each backend in `examples/`:

```
examples/uart_dma/   Full demo: levels, hexdump, module filter, deinit
examples/swo/        Minimal SWO output loop
examples/rtt/        RTT speed test with hexdump
examples/flash/      Crash log write + register dump
```

Build an example by setting `BLOG_BACKEND` and swapping `src/main.c`:

```bash
# UART DMA (default)
just build-arm

# SWO
cmake -B build-arm --toolchain toolchain-arm.cmake -DBLOG_BACKEND=BLOG_BACKEND_SWO .
cmake --build build-arm
```

## ROM/RAM Footprint

Automatically measured by CI for all 4 backends (STM32F103, `-Os`).
See the **GitHub Actions job summary** for per-backend `text/data/bss` breakdown.

Typical figures (core + backend only, excluding startup):

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

## Porting

BareLog is portable across ARM Cortex-M devices. To port:

1. **Create `port/<target>/`** with `target.cmake`, startup, linker script
2. **Create `blog_uart_port.h`** — define UART instance, GPIO pins, DMA channel, clock
3. **Adapt `blog_uart.c`** — GPIO init (F4/F7/H7 use `MODER` instead of `CRL/CRH`)
4. **Adapt `blog_flash.c`** — flash programming API differs across families

SWO and RTT backends work on any Cortex-M3+ without modification.

## Building

```bash
# Host tests
just test

# ARM firmware (requires arm-none-eabi-gcc)
just build-arm

# Flash via J-Link
just flash-jlink

# Flash via ST-Link
just flash-stlink
```

## Project Structure

```
include/          Public API headers
src/              Core + backend implementations
port/stm32f1/     STM32F1 target (startup, linker, device headers)
cmsis/            CMSIS core headers (F0, F1, F4)
ext/              External: SEGGER RTT
tests/            Unity test suite (host)
tools/            Flash decode utility
examples/         Per-backend minimal examples
```

## License

BSD-3-Clause. See [LICENSE](LICENSE).
