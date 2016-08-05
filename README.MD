# TinyOS

A minimalist operating system written in 935 lines of pure c,
(exactly 1500 if you include whitespace and comments)
and an fat12 compliant bootloader (about 200 lines of code)

This is a 32 bit single tasking, unix like operating system.
Features standard io. All devices are files (like in unix)
all devices benefit from printf style functionality.

Minimalist C standard runtime subset
(malloc.h memory.h string.h stdio.h)

First fit memory alllocation. Simple but effective.

Extremely optimized shell commandline interpretter.
supports launching userland programs with argc, argv

serial port and terminal device drivers

even an userland program that's a serial based networking terminal
