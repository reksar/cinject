/*
 * Injector for test program from
 * https://xakep.ru/2015/08/12/code-injection/
 */

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

DWORD getPID(const CHAR *PrName)
{
  PROCESSENTRY32W entry;
  entry.dwSize = sizeof(PROCESSENTRY32W);
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (Process32First(snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
  {
    while (Process32Next(snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
    {
      if (strcmp((CHAR *)entry.szExeFile, PrName) == 0)
      {
        CloseHandle(snapshot);
        return entry.th32ProcessID;
      }
    }
  }
  CloseHandle(snapshot);
  return NULL;
}

BYTE *GetModuleBase(const CHAR *lpModuleName, DWORD dwProcessId)
{
  MODULEENTRY32W lpModuleEntry = { 0 };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
  if (! snapshot)
    return NULL;
  
  lpModuleEntry.dwSize = sizeof(lpModuleEntry);
  BOOL bModule = Module32First(snapshot, (LPMODULEENTRY32)&lpModuleEntry);
  while (bModule)
  {
    if (!strcmp((CHAR *)lpModuleEntry.szModule, lpModuleName))
    {
      CloseHandle(snapshot);
      return lpModuleEntry.modBaseAddr;
    }
    bModule = Module32Next(snapshot, (LPMODULEENTRY32)&lpModuleEntry);
  }
  CloseHandle(snapshot);
  return NULL;
}

INT main()
{
  const CHAR *PrName = "test.exe";

  DWORD PID;
  if (!(PID = getPID(PrName)))
  {
    printf("Process not found: %s\n", PrName);
    return 1;
  }
  printf("Process %s\nPID: %d\n", PrName, PID);

  HANDLE hProcess;
  if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID)))
  {
    printf("Can't Handle the PID %d\n", PID);
    return 2;
  }
  printf("Handle: %d\n", hProcess);

  BYTE *pBase;
  if (!(pBase = GetModuleBase(PrName, PID)))
  {
    printf("Can not get address of PID %d\n", PID);
    return 3;
  }

  BYTE *pBuffer = pBase + 0x2238;
  BYTE *pfuncPrintMessage = pBase + 0x1080;

  const BYTE MESSAGE_LENGTH = 16; // "default message"
  CHAR localBuffer[MESSAGE_LENGTH];
  ReadProcessMemory(
    hProcess,
    (void *)pBuffer,
    &localBuffer,
    sizeof(CHAR) * MESSAGE_LENGTH,
    0);
  printf("Buffer: %s\n", localBuffer);

  const DWORD64 VictimAddress = (DWORD64)pBase;
  const CHAR Shellcode[] = "SHELLCODE\xAA\xBB\xCC";
  const SIZE_T VictimAddressSize = sizeof(VictimAddress);
  const SIZE_T ShellcodeSize = sizeof(Shellcode);
  const SIZE_T VictimMemorySize = VictimAddressSize + ShellcodeSize;
  LPVOID VictimMemory = VirtualAllocEx(
    hProcess,
    NULL,
    VictimMemorySize,
    MEM_COMMIT,
    PAGE_EXECUTE_READWRITE);
  printf("Allocated %llu bytes at %p\n", VictimMemorySize, VictimMemory);
  const BOOL IsBaseAddressWritten = WriteProcessMemory(
    hProcess,
    VictimMemory,
    &VictimAddress,
    VictimAddressSize,
    NULL);
  if (!IsBaseAddressWritten)
    printf("Cannot write address of victim process!\n");
  const BOOL IsShellCodeWritten = WriteProcessMemory(
    hProcess,
    (DWORD64 *)VictimMemory + 1,
    Shellcode,
    ShellcodeSize,
    NULL);
  if (!IsShellCodeWritten)
    printf("Cannot write shellcode!\n");

  return NULL;
}
