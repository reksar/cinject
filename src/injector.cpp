/*
 * Injector for test program from
 * https://xakep.ru/2015/08/12/code-injection/
 */

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

// You can generate it with "Extract shellcode" from `.vscode\tasks.json`, but
// it will be automatically rewritted during default build task.
#include "shellcode.h"

const SIZE_T SZ_MESSAGE_MAX = 260;
const SIZE_T SZ_MEMORY = SZ_MESSAGE_MAX + SZ_SHELLCODE;

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
  while (! IsNameFound && HasProcess)
  {
    IsNameFound = ! strcmp(Name, (CHAR*)Process->szExeFile);
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
  while (! IsNameFound && HasModule)
  {
    IsNameFound = ! strcmp(Name, (CHAR*)Module->szModule);
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
  return ! strcmp(MESSAGE, Buffer);
}

BOOL WriteShellcode(const ProcessEntry Process, const LPVOID pMemory)
{
  return WriteProcessMemory(
    Process.Handle, pMemory, SHELLCODE, SZ_SHELLCODE, NULL);
}

BOOL RewriteAddresses(const ProcessEntry Process, const LPVOID pMemory)
{
  const DWORD64 FMT_OFFSET = 0x2220;
  const DWORD64 CALL_OFFSET = 0x1080;

  const LPVOID pFmtAddress = (BYTE*)pMemory + FMT_ADDRESS_OFFSET;
  const LPVOID pMsgAddress = (BYTE*)pMemory + MSG_ADDRESS_OFFSET;
  const LPVOID pCallAddress = (BYTE*)pMemory + CALL_ADDRESS_OFFSET;

  const auto ProcessAddress = reinterpret_cast<DWORD64>(Process.Pointer);
  const auto FmtAddress = ProcessAddress + FMT_OFFSET;
  const auto CallAddress = ProcessAddress + CALL_OFFSET;

  const auto ShellcodeAddress = reinterpret_cast<DWORD64>(pMemory);
  const auto MsgAddress = ShellcodeAddress + SZ_SHELLCODE;

  const auto szAddress = sizeof(DWORD64);
  const auto Handle = Process.Handle;

  return WriteProcessMemory(Handle, pFmtAddress, &FmtAddress, szAddress, NULL)
    && WriteProcessMemory(Handle, pMsgAddress, &MsgAddress, szAddress, NULL)
    && WriteProcessMemory(Handle, pCallAddress, &CallAddress, szAddress, NULL);
}

LPVOID Inject(const ProcessEntry Process)
{
  const auto pMemory = VirtualAllocEx(
    Process.Handle, nullptr, SZ_MEMORY, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  return WriteShellcode(Process, pMemory)
    && RewriteAddresses(Process, pMemory)
      ? pMemory : nullptr;
}

BOOL WriteMessage(
  const CHAR Message[SZ_MESSAGE_MAX],
  const ProcessEntry Process,
  const LPVOID pMemory)
{
  const LPVOID pMessage = (BYTE*)pMemory + SZ_SHELLCODE;
  const auto szMessage = strlen(Message);
  return szMessage
    ? WriteProcessMemory(Process.Handle, pMessage, Message, szMessage, NULL)
    : false;
}

void RunShellcode(const ProcessEntry Process, const LPVOID pMemory)
{
  const auto pStart = (LPTHREAD_START_ROUTINE)pMemory;
  const auto hThread = CreateRemoteThread(
    Process.Handle, NULL, NULL, pStart, NULL, NULL, NULL);
  CloseHandle(hThread);
}

INT main()
{
  const auto Process = HandleProcess("test.exe");
  if (! (Process.PID && Process.Handle && Process.Pointer))
    return 1;
  
  printf("%s at %p\n", Process.Name, Process.Pointer);

  if (! CanReadDefaultMessage(Process))
    return 2;

  const auto pMemory = Inject(Process);
  if (! pMemory)
    return 3;
  
  printf("Allocated %llu bytes at %p\n", SZ_MEMORY, pMemory);

  CHAR Message[SZ_MESSAGE_MAX];
  while (fgets(Message, SZ_MESSAGE_MAX, stdin) && strlen(Message) > 1)
  {
    WriteMessage(Message, Process, pMemory);
    RunShellcode(Process, pMemory);
    rewind(stdin);
  }

  return NULL;
}
