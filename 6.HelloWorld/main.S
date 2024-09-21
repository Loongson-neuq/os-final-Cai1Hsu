section .text
   global _start
_start:
    mov eax, 0x04
    mov ebx, 0x2      ; Standard Output
    mov ecx, hello
    mov edx, 14
    syscall

    mov eax, 0x01    ; Don't forget to call exit
    mov ebx, 0x0
    syscall

hello:  db      "Hello, World!", 10