/*
 * Injector for test program from
 * https://xakep.ru/2015/08/12/code-injection/
 */

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

struct ProcessEntry
{
  const CHAR* Name;
  const DWORD PID;
  const HANDLE Handle;
  const LPVOID Pointer;
};

DWORD GetPID(const CHAR* ProcessName)
{
  PROCESSENTRY32W entry;
  entry.dwSize = sizeof(PROCESSENTRY32W);
  HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (Process32First(Snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
  {
    while (Process32Next(Snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
    {
      if (strcmp((CHAR*)entry.szExeFile, ProcessName) == 0)
      {
        CloseHandle(Snapshot);
        return entry.th32ProcessID;
      }
    }
  }
  CloseHandle(Snapshot);
  return NULL;
}

LPVOID GetProcessPointer(const CHAR* Name, const DWORD PID)
{
  const LPMODULEENTRY32 Module = new MODULEENTRY32{ sizeof(MODULEENTRY32) };
  const HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

  LPVOID ProcessPointer = NULL;
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

ProcessEntry OpenProcess(const CHAR* Name)
{
  const auto PID = GetPID(Name);
  const auto Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
  const auto Address = GetProcessPointer(Name, PID);
  return ProcessEntry{ Name, PID, Handle, Address };
}

LPVOID Inject(ProcessEntry Process)
{
  const auto Handle = Process.Handle;

  const CHAR SHELLCODE[] = "\x48\xB8\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x48\x89\xC1\x48\xB8\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x48\x89\xC2\x48\xB8\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xD0\xC3";
  const auto zsShellcode = sizeof(SHELLCODE);

  const SIZE_T SZ_MAX_MESSAGE = 260;
  const DWORD64 FMT_OFFSET = 0x2220;
  const DWORD64 CALL_OFFSET = 0x1080;

  const auto szMemory = zsShellcode + SZ_MAX_MESSAGE;
  const auto pMemory = VirtualAllocEx(
    Handle,
    NULL,
    szMemory,
    MEM_COMMIT,
    PAGE_EXECUTE_READWRITE);
  
  printf("Allocated %llu bytes at %p\n", szMemory, pMemory);

  const auto ProcessAddress = reinterpret_cast<DWORD64>(Process.Pointer);
  const DWORD64 FmtAddress = ProcessAddress + FMT_OFFSET;
  const DWORD64 CallAddress = ProcessAddress + CALL_OFFSET;

  const auto ShellcodeAddress = reinterpret_cast<DWORD64>(pMemory);
  const DWORD64 MsgAddress = ShellcodeAddress + zsShellcode;

  const LPVOID pFmtAddress = (BYTE*)pMemory + 2;
  const LPVOID pMsgAddress = (BYTE*)pMemory + 14;
  const LPVOID pCallAddress = (BYTE*)pMemory + 28;
  
  const auto szAddress = sizeof(DWORD64);
  
  return WriteProcessMemory(Handle, pMemory, SHELLCODE, zsShellcode, NULL)
    && WriteProcessMemory(Handle, pFmtAddress, &FmtAddress, szAddress, NULL)
    && WriteProcessMemory(Handle, pMsgAddress, &MsgAddress, szAddress, NULL)
    && WriteProcessMemory(Handle, pCallAddress, &CallAddress, szAddress, NULL)
      ? pMemory : NULL;
}

void PrintProcessInfo(ProcessEntry Process)
{
  printf("%s at %p\n", Process.Name, Process.Pointer);
}

void PrintDefaultMessage(ProcessEntry Process)
{
  LPVOID Message = (BYTE*)Process.Pointer + 0x2238;
  const BYTE MESSAGE_LENGTH = 16; // "default message"
  CHAR localBuffer[MESSAGE_LENGTH];
  ReadProcessMemory(
    Process.Handle,
    Message,
    &localBuffer,
    sizeof(CHAR) * MESSAGE_LENGTH,
    0);
  printf("Message: %s\n", localBuffer);
}

INT main()
{
  const auto Process = OpenProcess("test.exe");
  PrintProcessInfo(Process);

  if (!Process.PID || !Process.Handle || !Process.Pointer)
    return 1;
  
  PrintDefaultMessage(Process);

  const auto pMemory = Inject(Process);
  if (!pMemory)
    return 2;

  /*
  const BOOL IsMessageWritten = WriteMessage(hProcess, pVictimMemory);
  if (!IsMessageWritten)
    return 3;
  */

  printf("Done!");
  return NULL;
}
