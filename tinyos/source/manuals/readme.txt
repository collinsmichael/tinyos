/* ========================================================================= */
/* File Name : source/manuals/readme.txt                                     */
/* File Date : 5 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* Detailed requirements analysis and project specifications.                */
/* Done List                                                                 */
/*   o  keyboard input                                                       */
/*   o  console output                                                       */
/*   o  memory management (throw away after paging is supported)             */
/*   o  standard input output                                                */
/*   o  minimalist c standard library                                        */
/*   o  serial port communication                                            */
/*                                                                           */
/* ToDo List                                                                 */
/*   o  file system manager (FAT 16) jump back to 16 bit to call interrupts  */
/*   o  process scheduler (interrupts must be implemented first)             */
/*   o  device management (floppy driver first)                              */
/*   o  32 bit interrupts                                                    */
/*   o  multitasking                                                         */
/*   o  virtual memory and paging                                            */
/* ========================================================================= */

logon as root:
logon: "root"
password: ""

logon as mike:
logon: "mike"
password: "letmein"

to log out:
user > "logout"
or
user > "exit"

to execute program
user > "program commandlineargs"

example
user > "chat"
will execute the serial port chat terminal
everything you type will be echoed to the other chat peer

user : "exit"
will exit the chat program

This kernel was designed to be debuggable. When in debug mode all hardware IO is
rerouted to user defined arrays.
The keyboard will read commands directly from a string.
look in source/devices/keyboard.c and find kbrd_getc.
these commands will be executed by the shell.
in debug mode portio ignores bytes.


This is a 32 bit protected mode kernel, written mostly in pure c with a minimal
use of assembler. Microsoft Visual Studio Express 2008 was used for development
and testing. Which can be used to debug the system. In debug mode all reserved
opcodes such a in/out are enclosed in NDEBUG directives. All input and output
can be mocked up in an array while stepping through a debug session.

The kernel is split into the following parts:

   o The boot sector. which simply parses just enough of the file system so it
     can read the executable into memory and execute the entry point. The file
     system used is FAT 16.

   o PE executables have a 16 bit MS DOS stub which gets executed on 16 bit
     systems. The kernel takes advantage of this by customizing the 16 bit stub
     so that it can perform any last minute BIOS interrupts, before entering
     32 bit protected. Once in 32 bit protected mode interrupts are disabled.
     The executable image is then remapped from where the MSDOS stub lives to
     where the 32 bit executable lives. Finally execution passes to the 32 bit
     kernel.

   o Memory management is achieved via a minimalist first fit algorithm. This
     interface is almost identical to K&R C alloc and free, in which the burden
     is on the userland programs to maintain the size of each block being freed
     this is a throw away function, to be replaced by a more sophisticated paging
     mechanism.
    
   o Standard input and output is achieved like on unix by treating everything
     as a file. To simplify things the FILE structure used by kopen includes a
     few function pointers, such as kclose, kgetc, kputc.

Devices supported include:
   o PS2 keyboard
   o RS232 serial port
   o VESA video modes are set up in the stub while still in 16 bit real mode
