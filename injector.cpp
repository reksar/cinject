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
  BYTE* Address;
};

DWORD GetPID(const CHAR* ProcessName)
{
  PROCESSENTRY32W entry;
  entry.dwSize = sizeof(PROCESSENTRY32W);
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (Process32First(snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
  {
    while (Process32Next(snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
    {
      if (strcmp((CHAR*)entry.szExeFile, ProcessName) == 0)
      {
        CloseHandle(snapshot);
        return entry.th32ProcessID;
      }
    }
  }
  CloseHandle(snapshot);
  return NULL;
}

BYTE* GetBaseAddress(const CHAR* lpModuleName, DWORD dwProcessId)
{
  MODULEENTRY32W lpModuleEntry = { 0 };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
  if (! snapshot)
    return NULL;
  
  lpModuleEntry.dwSize = sizeof(lpModuleEntry);
  BOOL bModule = Module32First(snapshot, (LPMODULEENTRY32)&lpModuleEntry);
  while (bModule)
  {
    if (!strcmp((CHAR*)lpModuleEntry.szModule, lpModuleName))
    {
      CloseHandle(snapshot);
      return lpModuleEntry.modBaseAddr;
    }
    bModule = Module32Next(snapshot, (LPMODULEENTRY32)&lpModuleEntry);
  }
  CloseHandle(snapshot);
  return NULL;
}

ProcessEntry OpenProcess(const CHAR* Name)
{
  const auto PID = GetPID(Name);
  const auto Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
  const auto Address = GetBaseAddress(Name, PID);
  return ProcessEntry { Name, PID, Handle, Address };
}

LPVOID Inject(ProcessEntry Process)
{
  const CHAR Shellcode[] = "\x48\xB8\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x48\x89\xC1\x48\xB8\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x48\x89\xC2\x48\xB8\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xD0\xC3";
  const DWORD64 FmtAddress = 0x7ff671e72220;
  const SIZE_T ShellcodeSize = sizeof(Shellcode);
  const SIZE_T AddressSize = sizeof(DWORD64);

  const LPVOID VictimMemory = VirtualAllocEx(
    Process.Handle,
    NULL,
    ShellcodeSize,
    MEM_COMMIT,
    PAGE_EXECUTE_READWRITE);
  
  printf("Allocated %llu bytes at %p\n", ShellcodeSize, VictimMemory);

  const LPVOID pFmtAddress = (BYTE*)VictimMemory + 2;
  
  return WriteProcessMemory(
    Process.Handle,
    VictimMemory,
    Shellcode,
    ShellcodeSize,
    NULL)
  && WriteProcessMemory(
    Process.Handle,
    pFmtAddress,
    &FmtAddress,
    AddressSize,
    NULL)
  ? VictimMemory : NULL;
}

void PrintProcessInfo(ProcessEntry Process)
{
  printf("%s at %p\n", Process.Name, Process.Address);
}

void PrintDefaultMessage(ProcessEntry Process)
{
  BYTE* pBuffer = Process.Address + 0x2238;
  const BYTE MESSAGE_LENGTH = 16; // "default message"
  CHAR localBuffer[MESSAGE_LENGTH];
  ReadProcessMemory(
    Process.Handle,
    (void*)pBuffer,
    &localBuffer,
    sizeof(CHAR) * MESSAGE_LENGTH,
    0);
  printf("Buffer: %s\n", localBuffer);
}

INT main()
{
  const auto Process = OpenProcess("test.exe");
  PrintProcessInfo(Process);

  if (!Process.PID || !Process.Handle || !Process.Address)
    return 1;
  
  PrintDefaultMessage(Process);

  const auto VictimMemory = Inject(Process);
  if (!VictimMemory)
    return 2;

  /*TODO
  const BOOL IsMessageWritten = WriteMessage(hProcess, pVictimMemory);
  if (!IsMessageWritten)
    return 3;
  */
  printf("Done!");
  return NULL;
}
