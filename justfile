default: build-arm

# ── ARM cross-compile (requires arm-none-eabi-gcc) ──────────────────

build-arm:
	cmake --fresh -B build-arm -G "MinGW Makefiles" \
		-DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake .
	cmake --build build-arm

rebuild: clean-arm build-arm

flash-stlink: build-arm
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program build-arm/BareLog.elf verify reset exit"

flash-jlink: build-arm
	JLink.exe -device STM32F103C8 -if SWD -speed 4000 \
		-commandfile flash.jlink

clean-arm:
	rm -rf build-arm

# ── Host tests (native gcc) ─────────────────────────────────────────

test:
	cmake --fresh -B build-tests -S tests -G "MinGW Makefiles" \
		-DCMAKE_C_COMPILER=gcc
	cmake --build build-tests
	ctest --test-dir build-tests --output-on-failure

clean-tests:
	rm -rf build-tests

# ── Formatting ──────────────────────────────────────────────────────

fmt:
	clang-format -i src/*.c include/*.h port/stm32f1/include/*.h tests/*.c

# ── All clean ───────────────────────────────────────────────────────

clean: clean-arm clean-tests
