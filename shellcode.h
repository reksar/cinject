#ifndef CINJECT_SHELLCODE_H_
#define CINJECT_SHELLCODE_H_

#include <minwindef.h>

/*
 * The shellcode is:
 *
 * mov rax, 0xaaaaaaaaaaaaaaaa ; "%s" fmt
 * mov rcx, rax
 * mov rax, 0xbbbbbbbbbbbbbbbb ; msg
 * mov rdx, rax
 * mov rax, 0xcccccccccccccccc ; printf()
 * call rax
 * ret
 * 
 * I compiled it with NASM, deleted all additional code and put 
 * build/shellcode.obj file with same content as below:
 * 
 * \x48\xB8\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x48\x89\xC1\x48\xB8
 * \xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x48\x89\xC2\x48\xB8
 * \xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xD0\xC3
 * 
 * xxd -i shellcode.obj > shellcode.obj.h
 */
#include "shellcode.obj.h"
extern unsigned char shellcode_obj[];
extern unsigned int shellcode_obj_len;

const BYTE* SHELLCODE = shellcode_obj;
const SIZE_T SZ_SHELLCODE = shellcode_obj_len;

const DWORD64 FMT_ADDRESS_OFFSET = 2;
const DWORD64 MSG_ADDRESS_OFFSET = 15;
const DWORD64 CALL_ADDRESS_OFFSET = 28;
const SIZE_T SZ_MESSAGE_MAX = 260;
const SIZE_T SZ_MEMORY = SZ_SHELLCODE + SZ_MESSAGE_MAX;

#endif  // CINJECT_SHELLCODE_H_
