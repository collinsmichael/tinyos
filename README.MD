# TinyOS

A minimalist operating system written in 935 lines of pure c,
(exactly 1500 if you include whitespace and comments)
and an fat12 compliant bootloader (about 200 lines of code)
The bootloader is written in assembler and considered a
seperate work.

This is a 32 bit single tasking, unix like operating system.
Features standard io. All devices are files (like in unix)
all devices benefit from printf style functionality.

Minimalist C standard runtime subset (malloc.h memory.h string.h stdio.h)
First fit memory allocation. Simple but effective. Extremely optimized
shell commandline interpretter. Supports launching userland programs
with argc, argv. serial port and terminal device drivers, even a userland
program that acts as a serial based networking terminal.

This Operating System can be built in debug mode in which all IO
is simulated through arrays. This mode allows you to test the
correctness of the design by debuggering inside Visual Studio.
When built in release mode full hardware IO is enabled.

My normal development flow is to design a module, implement the code,
verify by running in Visual Studio in debug mode, build in release,
test in a VM, if all goes well then the floppy disc image is written
to a physical floppy and tested on real hardware, as some code may
run fine in a generic VM but will not run correctly on real hardware.
