; NASM produces a lot of additional code during compilation, that is useless 
; for shellcoding. So there are labels `shellcode_begin` and `shellcode_end`,
; that helps to cut necessary binary fragment.
;
; Joining lines from the `xxd -ps shellcode.obj` output into one line gives:
; "<useless shit>abcdef<shellcode>fecba<useless shit>"

shellcode_begin: db 0xab, 0xcd, 0xef

; -----------------------------------------------------------------------------
; Preserving registers with convention:
; https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention
;
; Usually, a callee shall not to preserve volatile registers, but there is an 
; intrusion into the normal process flow, so it is needed for correct return 
; from thread and keep the process working.

push rax
push rcx
push rdx
push r8
push r9
push r10
push r11
; There are 7 64-bit values on the stack (56 bytes), but the stack will always 
; be maintained 16-byte aligned, so fit it to 64 % 16 == 0:
sub rsp, 8

; We can not to PUSH 128-bit (16-bytes) XMM registers, so ...

sub rsp, 16
movdqu [rsp], xmm0

sub rsp, 16
movdqu [rsp], xmm1

sub rsp, 16
movdqu [rsp], xmm2

sub rsp, 16
movdqu [rsp], xmm3

sub rsp, 16
movdqu [rsp], xmm4

sub rsp, 16
movdqu [rsp], xmm5

; -----------------------------------------------------------------------------
; Shellcode payload.
;
; We can only use MOV RAX to set a 64-bit value (QWORD) into a register. So we
; pass addresses into other registers through the RAX register.
;
; The first four int args are passed in RCX, RDX, R8 and R9 in that order.
; Additional int args are passed on the stack. So calling `printf(fmt, msg)`
; requires `&fmt` -> RCX and `&msg` -> RDX.
;
; See about 64-bit NASM at https://nasm.us/xdoc/2.15.03rc8/html/nasmdo12.html
;
; Note: 0x[a|b|c]*16 addresses are stubs that will be replaced during injection.

mov rax, 0xaaaaaaaaaaaaaaaa ; "%s" fmt
mov rcx, rax

mov rax, 0xbbbbbbbbbbbbbbbb ; msg
mov rdx, rax

mov rax, 0xcccccccccccccccc ; printf()
call rax

; -----------------------------------------------------------------------------
; Restore registers before return.

movdqu xmm5, [rsp]
add rsp, 16

movdqu xmm4, [rsp]
add rsp, 16

movdqu xmm3, [rsp]
add rsp, 16

movdqu xmm2, [rsp]
add rsp, 16

movdqu xmm1, [rsp]
add rsp, 16

movdqu xmm0, [rsp]
add rsp, 16

add rsp, 8
pop r11
pop r10
pop r9
pop r8
pop rdx
pop rcx
pop rax

; -----------------------------------------------------------------------------
; Return to kernel32, then MOV ECX, EAX and CALL &RtlExitUserThread.
ret

shellcode_end: db 0xfe, 0xdc, 0xba
