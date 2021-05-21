; See about 64-bit NASM at https://nasm.us/xdoc/2.15.03rc8/html/nasmdo12.html
;
; If is needed to set a 64-bit value (QWORD) into register, we can use 
; MOV RAX only. So we pass addresses to other registers through the RAX.
;
; The first four integer arguments are passed in RCX, RDX, R8 and R9, 
; in that order. Additional integer arguments are passed on the stack.
; So calling printf(fmt, msg) requires fmt at RCX and msg at RDX.

; NASM produces a lot of additional code during compilation, that is useless 
; for shellcoding, so there are labels `shellcode_begin` and `shellcode_end`
; to help to cut necessary compiled fragment.
; You can find `abcdef<shellcode>fecba` with `xxd -ps shellcode.obj`
shellcode_begin: db 0xab, 0xcd, 0xef

mov rax, 0xaaaaaaaaaaaaaaaa ; "%s" fmt
mov rcx, rax

mov rax, 0xbbbbbbbbbbbbbbbb ; msg
mov rdx, rax

mov rax, 0xcccccccccccccccc ; printf()
call rax
ret

shellcode_end: db 0xfe, 0xdc, 0xba
