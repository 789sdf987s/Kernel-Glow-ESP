#pragma once
#include <ntifs.h>
#include <ntddk.h>

#include "Structure.h"

namespace Manager {
typedef PVOID (*fnPsGetProcessWow64Process)(PEPROCESS Process);

class Process {
 public:
  Process(CHAR* ProcessName);
  uintptr_t GetModuleBaseAddress(WCHAR* ModuleName);
  NTSTATUS GetProcess(PEPROCESS* pProcess);

 private:
  NTSTATUS FindProcessByName(CHAR* ProcessName);

  PEPROCESS m_Process = NULL;
};
}  // namespace Manager
