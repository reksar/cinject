/*
 * Injector for test program from
 * https://xakep.ru/2015/08/12/code-injection/
 */

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

DWORD get_PID(const char *PrName)
{
  PROCESSENTRY32 entry;
  entry.dwSize = sizeof(PROCESSENTRY32);
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (Process32First(snapshot, &entry) == TRUE)
  {
    while (Process32Next(snapshot, &entry) == TRUE)
    {
      if (strcmp((char *)entry.szExeFile, PrName) == 0)
      {
        CloseHandle(snapshot);
        return entry.th32ProcessID;
      }
    }
  }
  CloseHandle(snapshot);
  return NULL;
}

DWORD GetModuleBase(const char *lpModuleName, DWORD dwProcessId)
{
  MODULEENTRY32 lpModuleEntry = { 0 };
  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

  if (!hSnapShot)
    return NULL;
  
  lpModuleEntry.dwSize = sizeof(lpModuleEntry);
  BOOL bModule = Module32First(hSnapShot, &lpModuleEntry);
  while (bModule)
  {
    if (!strcmp((char *)lpModuleEntry.szModule, lpModuleName))
    {
      CloseHandle(hSnapShot);
      return (DWORD)lpModuleEntry.modBaseAddr;
    }
    bModule = Module32Next(hSnapShot, &lpModuleEntry);
  }
  CloseHandle(hSnapShot);
  return NULL;
}

int main()
{
  const char *PrName = "test.exe";
  DWORD PID;
  if (!(PID = get_PID(PrName)))
  {
    printf("Process not found: %s\n", PrName);
    return 1;
  }
  printf("Process %s has PID %d\n", PrName, PID);

  HANDLE hProcess;
  if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID)))
  {
    printf("Can't Handle the PID %d\n", PID);
    return 2;
  }
  printf("Handle: %d\n", hProcess);

  DWORD BaseAddress;
  if (!(BaseAddress = GetModuleBase(PrName, PID)))
  {
    printf("Can not get BaseAddress of PID %d\n", PID);
    return 3;
  }
  printf("BaseAddress: %x\n", BaseAddress);
  
  return 0;
}
