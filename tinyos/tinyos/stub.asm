
 ; ======================================================================= ;
 ;                                                                         ;
 ; File Name : STUB.ASM                                                    ;
 ; File Date : 2 November 2014                                             ;
 ; Author(s) : Michael Collins                                             ;
 ;                                                                         ;
 ; ----------------------------------------------------------------------- ;
 ; [1] Build E820 Memory Map                                               ;
 ; [2] Enter Vesa Video Mode                                               ;
 ; ======================================================================= ;
 
 Base      = 0x0600                           ;
 format    binary  as 'EXE'                   ;
 org       Base                               ;
 use16                                        ;

 mzhdr:
 .e_magic:    dw 0x5A4D
 .e_cblp:     dw 0x0000
 .e_cp:       dw pesig/512
 .e_crlc:     dw 0x0000
 .e_cparhdr:  dw 0x0004
 .e_minalloc: dw 0x0000
 .e_maxalloc: dw 0xFFFF
 .e_ss:       dw 0x0000
 .e_sp:       dw 0xFFF0
 .e_csum:     dw 0x0000
 .e_ip:       dw 0x0000
 .e_cs:       dw 0x0000
 .e_lsarlc:   dw 0x0000
 .e_ovno:     dw 0x0000
 .e_res:      dw 0x0000, 0x0000, 0x0000, 0x0000
 .e_oemid:    dw 0x0000
 .e_oeminfo:  dw 0x0000
 .e_res2:     dw 0x0000, 0x0000, 0x0000, 0x0000
              dw 0x0000, 0x0000, 0x0000, 0x0000
              dw 0x0000, 0x0000
 .e_lfanew:   dd pesig


 Boot:
 ; --------------
 ; Setup segments
 ; --------------
           cli                                ;
           xor     ax, ax                     ;
           mov     ds, ax                     ;
           mov     es, ax                     ;
           mov     ss, ax                     ;
           mov     sp, 0x7C00                 ;
           sti                                ;
           cld                                ;
           jmp     0x0000:Init                ;

 Init:            
 ; ---------------
 ; Turn off floppy
 ; ---------------                 
           mov     dx, 0x03F2                 ;
           mov     al, 0x00                   ;
           out     dx, al                     ;

 ; ---------------------
 ; Build E820 Memory Map
 ; ---------------------
           push    e820/16
           pop     es
           xor     di, di
           call    do_e820
           mov     word [ NumE820s ], ax

 ; -------------------------------------
 ; Text Mode 80x25 16 colors (no cursor)
 ; -------------------------------------
           mov     ax, 0x0003                 ;
           int     0x10                       ;
           mov     ax, 0x0100                 ;
           mov     cx, 0x2000                 ;
           int     0x10                       ;

 ; --------------------
 ; Enter protected mode
 ; --------------------
           cli                                ;
           lgdt    [ gdtr ]                   ;
           mov     eax, cr0                   ;
           or      al, 0x01                   ;
           mov     cr0, eax                   ;
           jmp     0x0010:pm32                ;


 ; use the INT 0x15, eax = 0xE820 BIOS function to get a memory map
 ; inputs: es:di -> destination buffer for 24 byte entries
 ; outputs: bp = entry count, trashes all registers except esi
 do_e820:
           xor     ebx, ebx                             ; ebx must be 0 to start
           xor     bp, bp                               ; keep an entry count in bp
           mov     edx, 0x534D4150                      ; Place "SMAP" into edx
           mov     eax, 0x0000E820                      ;
           mov     dword [ es : di + 0x14 ], 0x00000001 ; force a valid ACPI 3.X entry
           mov     ecx, 0x00000018                      ; ask for 24 bytes
           int     0x15                                 ;
           jc      short .failed                        ; carry set on first call means "unsupported function"
           mov     edx, 0x534D4150                      ; Some BIOSes apparently trash this register?
           cmp     eax, edx                             ; on success, eax must have been reset to "SMAP"
           jne     short .failed                        ;
           test    ebx, ebx                             ; ebx = 0 implies list is only 1 entry long (worthless)
           je      short .failed                        ;
           jmp     short .jmpin                         ;
 .e820lp:                                               ;
           mov     eax, 0x0000E820                      ; eax, ecx get trashed on every int 0x15 call
           mov     dword [ es : di + 0x14 ], 0x00000001 ; force a valid ACPI 3.X entry
           mov     ecx, 0x00000018                      ; ask for 24 bytes again
           int     0x15                                 ;
           jc      short .e820f                         ; carry set means "end of list already reached"
           mov     edx, 0x534D4150                      ; repair potentially trashed register
 .jmpin:                                                ;
           jcxz    .skipent                             ; skip any 0 length entries
           cmp     cl, 0x00000014                       ; got a 24 byte ACPI 3.X response?
           jbe     short .notext                        ;
          ;test    byte [ es : di + 0x14 ], 0x01        ; if so: is the "ignore this data" bit clear?
          ;je      short .skipent                       ;
 .notext:                                               ;
           mov     ecx, dword [ es : di + 0x08 ]        ; get lower dword of memory region length
           or      ecx, dword [ es : di + 0x0C ]        ; "or" it with upper dword to test for zero
           jz      .skipent                             ; if length qword is 0, skip entry
           inc     bp                                   ; got a good entry: ++count, move to next storage spot
           add     di, 0x00000018                       ;
           dec     si                                   ;
           jz      .e820f                               ;
 .skipent:                                              ;
           test    ebx, ebx                             ; if ebx resets to 0, list is complete
           jne     short .e820lp                        ;
 .e820f:                                                ;
           mov     ax, bp                               ;
           clc                                          ; there is "jc" on end of list to this point, so the carry must be cleared
           ret                                          ;
 .failed:                                               ;
           xor     ax, ax                               ;
           stc                                          ; "function unsupported" error exit
           ret                                          ;


 ; ----------------
 ; Set up selectors
 ; ----------------
 use32
 pm32:
           mov     ax, 0x0008                 ;
           mov     ds, ax                     ;
           mov     es, ax                     ;
           mov     fs, ax                     ;
           mov     gs, ax                     ;
           mov     ss, ax                     ;
           mov     esp, 0x0000FFF0            ;
           mov     ebp, esp

 ; -----------------------------
 ; Remap executable to ImageBase
 ; -----------------------------
 Copy:
           mov     ebx, mzhdr
           mov     edx, dword [ ebx + 0x3C ] ; mz.elfanew
           add     edx, ebx

           mov     edi, dword [ edx + 0x34 ] ; pe.ImageBase
           mov     ecx, dword [ edx + 0x50 ] ; pe.ImageSize
           mov     esi, ebx
           rep     movsb
          
           add     ebx, dword [ edx + 0x28 ] ; pe.AddressOfEntryPoint
           call    ebx                       ; Pass execution to mainCRTStartup
           jmp     $                         ; Just in case mainCRTStartup tries to return here


           db      Base+0x0180 - $ dup ? ; Padding

 gdt:      dq      0x0000000000000000         ; 0x00 NULL
 gdtData:  dq      0x00CF92000000FFFF         ; 0x08 DATA
 gdtCode:  dq      0x00CF98000000FFFF         ; 0x10 CODE
 gdtr:     dw      gdtr - gdt - 1             ; DESCRIPTOR
           dd      gdt                        ;
           dw      0xFFFF


 NumE820s: dw ?

           db      Base + 0x200 - $ dup 0xFF

;0800:0000
 e820:     db 0x200 dup 0x00

 pesig = $ - $$
