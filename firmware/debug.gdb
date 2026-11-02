# Manual debugging without VS Code:
#   openocd -f openocd.cfg
#   gdb-multiarch -x debug.gdb build/Debug/mouse-mover.elf

set confirm off
set pagination off
set print pretty on

target extended-remote localhost:3333

monitor reset halt
load
monitor reset halt

break main
continue
