# mouse-mover

A battery-powered dock that keeps a computer awake without installing anything
on it. The mouse sits on top of the dock. Every few minutes a coin vibration
motor under the mouse's optical sensor runs for a second, and the sensor reads
that as movement.

STM32F103C8T6 · C · STM32 HAL · CMake · Unity · KiCad · OpenSCAD

## Features

- Countdown to the next pulse on a 2-digit 7-segment display, `--` while the motor runs
- Period of 2, 4, 9, 14 or 59 minutes, set with a single button
- The setting survives power-off (stored in flash)
- Runs from 2 × AA cells through a 3.3 V boost converter

## Usage

Hold the button for 0.5 s to open the settings. The display blinks and shows
the current period; each short press steps to the next one. After 5 s without
a press the choice is saved and the countdown restarts.

## Hardware

- MCU: STM32F103C8T6
- Power: 2 × AA, MCP1640 boost converter to 3.3 V, power switch
- Display: 2-digit common-cathode 7-segment, multiplexed by the MCU
- Input: one push button, debounced with an RC filter
- Output: coin vibration motor switched by an N-MOSFET, with a flyback diode
- SWD header for programming and debugging

## Repository

| Folder | Contents |
|---|---|
| [`firmware/`](firmware) | STM32CubeMX project, application code, host unit tests |
| [`hardware/`](hardware) | KiCad schematic and PCB |
| [`case/`](case) | 3D-printed case (OpenSCAD) |

## License

See [LICENSE](LICENSE).
