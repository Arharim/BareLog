default: build-arm

# ── ARM cross-compile (requires arm-none-eabi-gcc) ──────────────────

build-arm chip="STM32F103xB":
	cmake -B build-arm -G "MinGW Makefiles" \
		-DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake \
		-DSTM32F1_CHIP={{ chip }} .
	cmake --build build-arm

build-arm-f4 chip="STM32F407VG":
	cmake -B build-arm-f4 -G "MinGW Makefiles" \
		-DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake \
		-DUHAL_TARGET=stm32f4 \
		-DSTM32F4_CHIP={{ chip }} .
	cmake --build build-arm-f4

rebuild: clean-arm build-arm

flash-stlink: build-arm
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program build-arm/BareLog.elf verify reset exit"

flash-jlink: build-arm
	JLink.exe -device STM32F103C8 -if SWD -speed 4000 \
		-commandfile flash.jlink

clean-arm:
	rm -rf build-arm

clean-arm-f4:
	rm -rf build-arm-f4

# ── Build specific backend ──────────────────────────────────────────

build-backend backend:
	cmake -B build-{{ backend }} -G "MinGW Makefiles" \
		-DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake \
		-DBLOG_BACKEND={{ backend }} .
	cmake --build build-{{ backend }}

build-uart: (build-backend "BLOG_BACKEND_UART_DMA")

build-swo: (build-backend "BLOG_BACKEND_SWO")

build-rtt: (build-backend "BLOG_BACKEND_RTT")

build-flash: (build-backend "BLOG_BACKEND_FLASH")

# ── Build all backends + show sizes ─────────────────────────────────

build-all: build-uart build-swo build-rtt build-flash

size:
	@echo "=== UART DMA ===" && arm-none-eabi-size build-BLOG_BACKEND_UART_DMA/BareLog.elf 2>/dev/null || echo "(not built)"
	@echo "=== SWO ===" && arm-none-eabi-size build-BLOG_BACKEND_SWO/BareLog.elf 2>/dev/null || echo "(not built)"
	@echo "=== RTT ===" && arm-none-eabi-size build-BLOG_BACKEND_RTT/BareLog.elf 2>/dev/null || echo "(not built)"
	@echo "=== Flash ===" && arm-none-eabi-size build-BLOG_BACKEND_FLASH/BareLog.elf 2>/dev/null || echo "(not built)"

# ── Host tests (native gcc) ─────────────────────────────────────────

test:
	cmake -B build-tests -S tests -G "MinGW Makefiles" \
		-DCMAKE_C_COMPILER=gcc
	cmake --build build-tests
	ctest --test-dir build-tests --output-on-failure

test-clean: clean-tests test

clean-tests:
	rm -rf build-tests

# ── Formatting ──────────────────────────────────────────────────────

fmt:
	clang-format -i src/*.c include/*.h port/stm32f1/include/*.h tests/*.c examples/*/*.c

# ── Flash decode ────────────────────────────────────────────────────

flash-decode file:
	python3 tools/blog_flash_decode.py {{ file }}

# ── All clean ───────────────────────────────────────────────────────

clean: clean-arm clean-arm-f4 clean-tests
	rm -rf build-BLOG_BACKEND_*
