;/****************************************************************************
; * boot_stub_startup.s — Vector table at 0x00000000 (boot stub region)
; * APP vectors live at BOOT_APP_FLASH_START (startup_hc32f460.s).
; ****************************************************************************/
Stack_Size      EQU     0x1000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stub_Stack_Mem  SPACE   Stack_Size
Stub_initial_sp

                PRESERVE8
                THUMB

                AREA    STUB_RESET, DATA, READONLY
                EXPORT  __Stub_Vectors

__Stub_Vectors  DCD     Stub_initial_sp
                DCD     Stub_Reset_Handler
                DCD     Stub_Default_Handler
                DCD     Stub_Default_Handler
                DCD     Stub_Default_Handler
                DCD     Stub_Default_Handler
                DCD     Stub_Default_Handler
                DCD     0
                DCD     0
                DCD     0
                DCD     0
                DCD     Stub_Default_Handler
                DCD     Stub_Default_Handler
                DCD     0
                DCD     Stub_Default_Handler
                DCD     Stub_Default_Handler

                AREA    |.text|, CODE, READONLY

                EXPORT  Stub_Reset_Handler
                IMPORT  boot_stub_on_reset

Stub_Reset_Handler PROC
                CPSID   I
                LDR     R0, =boot_stub_on_reset
                BX      R0
                ENDP

Stub_Default_Handler PROC
                B       .
                ENDP

                ALIGN
                END
