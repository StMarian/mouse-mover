# Firmware

## Architecture

```
app   App/Src, App/Drivers   state machine, settings, button, 7-segment driver (plain C)
 |
bsp   App/Bsp                pins, timers, flash, watchdog (the only code that includes the HAL)
 |
STM32CubeMX HAL + CMSIS      Core/, Drivers/
```

- **Interrupts only post events.** The 1 Hz timer pushes a tick into a lock-free
  single-producer/single-consumer queue. The main loop pops events, runs a
  table-driven state machine and sleeps in `WFI` until the next interrupt.
  The display refresh interrupt only reads the display buffer.
- **Layering is enforced by the build.** The `app` library is compiled without the
  HAL include paths, so including a HAL header outside `App/Bsp` fails to compile.
  This is also what lets `app` run on a PC in the tests.
- **Settings** live in the last 1 KB flash page, reserved in the linker script.
  The record has a magic number, version and check value. It is validated on load
  (falling back to the default) and rewritten only when the value changes.
- **Watchdog.** The independent watchdog resets the MCU after ~2 s without a kick.
  Fault handlers switch the motor off first.

## Requirements

- `arm-none-eabi-gcc`, CMake 3.22+, Ninja
- OpenOCD and an ST-Link for flashing
- A host C compiler for the tests

## Build

```bash
cmake --preset Debug
cmake --build --preset Debug
```

## Flash

```bash
openocd -f openocd.cfg -c "program build/Release/mouse-mover.elf verify reset exit"
```

## Tests

Unit tests for everything above `App/Bsp` (state machine, settings, button,
event queue, display driver), built with Unity, AddressSanitizer and
UndefinedBehaviorSanitizer. The board is replaced by a fake (`test/fakes/fake_bsp.c`)
that records what the firmware did to the motor, display and flash.

```bash
cmake -S test -B build/tests
cmake --build build/tests
ctest --test-dir build/tests --output-on-failure
```
