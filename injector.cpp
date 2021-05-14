/*
 * Injector for test program from
 * https://xakep.ru/2015/08/12/code-injection/
 */

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

DWORD getPID(const CHAR *ProcessName)
{
  PROCESSENTRY32W entry;
  entry.dwSize = sizeof(PROCESSENTRY32W);
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (Process32First(snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
  {
    while (Process32Next(snapshot, (LPPROCESSENTRY32)&entry) == TRUE)
    {
      if (strcmp((CHAR *)entry.szExeFile, ProcessName) == 0)
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

LPVOID Inject(HANDLE hProcess)
{
  const CHAR Shellcode[] = "\x48\xB8\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x48\x89\xC1\x48\xB8\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x48\x89\xC2\x48\xB8\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xD0\xC3";
  const SIZE_T ShellcodeSize = sizeof(Shellcode);

  LPVOID pVictimMemory = VirtualAllocEx(
    hProcess,
    NULL,
    ShellcodeSize,
    MEM_COMMIT,
    PAGE_EXECUTE_READWRITE);
  
  printf("Allocated %llu bytes at %p\n", ShellcodeSize, pVictimMemory);
  
  return WriteProcessMemory(
    hProcess,
    pVictimMemory,
    Shellcode,
    ShellcodeSize,
    NULL)
  ? pVictimMemory : NULL;
}

INT main()
{
  const CHAR *ProcessName = "test.exe";

  const DWORD PID = getPID(ProcessName);
  if (!PID)
  {
    printf("Process not found: %s\n", ProcessName);
    return 1;
  }
  printf("Process %s\nPID: %d\n", ProcessName, PID);

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
  if (!hProcess)
  {
    printf("Can't Handle the PID %d\n", PID);
    return 2;
  }
  printf("Handle: %d\n", hProcess);

  BYTE *pProcess = GetModuleBase(ProcessName, PID);
  if (!pProcess)
  {
    printf("Can not get address of PID %d\n", PID);
    return 3;
  }

  BYTE *pBuffer = pProcess + 0x2238;
  BYTE *pfuncPrintMessage = pProcess + 0x1080;

  const BYTE MESSAGE_LENGTH = 16; // "default message"
  CHAR localBuffer[MESSAGE_LENGTH];
  ReadProcessMemory(
    hProcess,
    (void *)pBuffer,
    &localBuffer,
    sizeof(CHAR) * MESSAGE_LENGTH,
    0);
  printf("Buffer: %s\n", localBuffer);

  LPVOID pVictimMemory = Inject(hProcess);
  if (!pVictimMemory)
    return 4;
  
  /*TODO
  const BOOL IsMessageWritten = WriteMessage(hProcess, pVictimMemory);
  if (!IsMessageWritten)
    return 5;
  */

  return NULL;
}
