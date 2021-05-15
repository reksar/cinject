/*
 * Injector for test program from
 * https://xakep.ru/2015/08/12/code-injection/
 */

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

#include "shellcode.h"

struct ProcessEntry
{
  const CHAR* Name;
  const DWORD PID;
  const HANDLE Handle;
  const LPVOID Pointer;
};

DWORD GetPID(const CHAR* Name)
{
  const LPPROCESSENTRY32 Process = new PROCESSENTRY32{ sizeof(PROCESSENTRY32) };
  HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

  DWORD PID = NULL;
  BOOL IsNameFound = FALSE;
  BOOL HasProcess = Process32First(Snapshot, Process);
  while (!IsNameFound && HasProcess)
  {
    IsNameFound = !strcmp(Name, (CHAR*)Process->szExeFile);
    if (IsNameFound)
      PID = Process->th32ProcessID;
    
    HasProcess = Process32Next(Snapshot, Process);
  }

  CloseHandle(Snapshot);
  return PID;
}

LPVOID GetProcessPointer(const CHAR* Name, const DWORD PID)
{
  const LPMODULEENTRY32 Module = new MODULEENTRY32{ sizeof(MODULEENTRY32) };
  const HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

  LPVOID ProcessPointer = nullptr;
  BOOL IsNameFound = FALSE;
  BOOL HasModule = Module32First(Snapshot, Module);
  while (!IsNameFound && HasModule)
  {
    IsNameFound = !strcmp(Name, (CHAR*)Module->szModule);
    if (IsNameFound)
      ProcessPointer = Module->modBaseAddr;
    
    HasModule = Module32Next(Snapshot, Module);
  }

  CloseHandle(Snapshot);
  return ProcessPointer;
}

ProcessEntry HandleProcess(const CHAR* Name)
{
  const auto PID = GetPID(Name);
  const auto Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
  const auto Address = GetProcessPointer(Name, PID);
  return ProcessEntry{ Name, PID, Handle, Address };
}

BOOL CanReadDefaultMessage(ProcessEntry Process)
{
  const CHAR MESSAGE[] = "default message";
  const DWORD64 MESSAGE_OFFSET = 0x2238;
  const BYTE szMessage = sizeof(MESSAGE);
  const LPVOID pMessage = (BYTE*)Process.Pointer + MESSAGE_OFFSET;
  CHAR Buffer[szMessage];
  ReadProcessMemory(Process.Handle, pMessage, &Buffer, szMessage, nullptr);
  return !strcmp(MESSAGE, Buffer);
}

LPVOID Inject(ProcessEntry Process)
{
  const DWORD64 FMT_OFFSET = 0x2220;
  const DWORD64 CALL_OFFSET = 0x1080;

  const auto Handle = Process.Handle;

  const auto pMemory = VirtualAllocEx(
    Handle, nullptr, SZ_MEMORY, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  const auto ProcessAddress = reinterpret_cast<DWORD64>(Process.Pointer);
  const DWORD64 FmtAddress = ProcessAddress + FMT_OFFSET;
  const DWORD64 CallAddress = ProcessAddress + CALL_OFFSET;

  const auto ShellcodeAddress = reinterpret_cast<DWORD64>(pMemory);
  const DWORD64 MsgAddress = ShellcodeAddress + SZ_SHELLCODE;

  const LPVOID pFmtAddress = (BYTE*)pMemory + FMT_ADDRESS_OFFSET;
  const LPVOID pMsgAddress = (BYTE*)pMemory + MSG_ADDRESS_OFFSET;
  const LPVOID pCallAddress = (BYTE*)pMemory + CALL_ADDRESS_OFFSET;

  const auto szAddress = sizeof(DWORD64);
  
  return WriteProcessMemory(Handle, pMemory, SHELLCODE, SZ_SHELLCODE, NULL)
    && WriteProcessMemory(Handle, pFmtAddress, &FmtAddress, szAddress, NULL)
    && WriteProcessMemory(Handle, pMsgAddress, &MsgAddress, szAddress, NULL)
    && WriteProcessMemory(Handle, pCallAddress, &CallAddress, szAddress, NULL)
      ? pMemory : nullptr;
}

BOOL WriteMessage(
  const CHAR Message[SZ_MESSAGE_MAX],
  ProcessEntry Process,
  LPVOID pMemory)
{
  const LPVOID pMessage = (BYTE*)pMemory + SZ_SHELLCODE;
  return WriteProcessMemory(
    Process.Handle, pMessage, Message, sizeof(Message), NULL);
}

INT main()
{
  const auto Process = HandleProcess("test.exe");
  if (!(Process.PID && Process.Handle && Process.Pointer))
    return 1;
  
  printf("%s at %p\n", Process.Name, Process.Pointer);

  if (!CanReadDefaultMessage(Process))
    return 2;

  const auto pMemory = Inject(Process);
  if (!pMemory)
    return 3;
  
  printf("Allocated %llu bytes at %p\n", SZ_MEMORY, pMemory);

  const BOOL IsMessageWritten = WriteMessage("OWNED!", Process, pMemory);
  if (!IsMessageWritten)
    return 4;

  printf("Done!");
  return NULL;
}
