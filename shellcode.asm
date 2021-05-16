; See 64-bit NASM at https://nasm.us/xdoc/2.15.03rc8/html/nasmdo12.html
;
; If is needed to set a 64-bit value (QWORD) into register, we can use 
; MOV RAX only. So we pass addresses to other registers through the RAX.
;
; The first four integer arguments are passed in RCX, RDX, R8 and R9, 
; in that order. Additional integer arguments are passed on the stack.
; So calling printf(fmt, msg) requires fmt at RCX and msg at RDX.

mov rax, 0xaaaaaaaaaaaaaaaa ; "%s" fmt
mov rcx, rax

mov rax, 0xbbbbbbbbbbbbbbbb ; msg
mov rdx, rax

mov rax, 0xcccccccccccccccc ; printf()
call rax
ret
