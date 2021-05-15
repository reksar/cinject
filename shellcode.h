#include <minwindef.h>

/*
 * mov rax, 0xaaaaaaaaaaaaaaaa ; "%s" fmt
 * mov rcx, rax
 * mov rax, 0xbbbbbbbbbbbbbbbb ; msg
 * mov rdx, rax
 * mov rax, 0xcccccccccccccccc ; printf()
 * call rax
 * ret
 */
const CHAR SHELLCODE[] =
  "\x48\xB8\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x48\x89\xC1\x48\xB8"
  "\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x48\x89\xC2\x48\xB8"
  "\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xD0\xC3";

const DWORD64 FMT_ADDRESS_OFFSET = 2;
const DWORD64 MSG_ADDRESS_OFFSET = 15;
const DWORD64 CALL_ADDRESS_OFFSET = 28;
const SIZE_T SZ_MESSAGE_MAX = 260;
const SIZE_T SZ_SHELLCODE = sizeof(SHELLCODE);
const SIZE_T SZ_MEMORY = SZ_SHELLCODE + SZ_MESSAGE_MAX;
