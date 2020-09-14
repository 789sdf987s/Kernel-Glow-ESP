#pragma once
#include <ntifs.h>
#include <ntddk.h>

#include "Ntapi.h"

namespace Manager {
class Memory {
 public:
  Memory(PEPROCESS Process);

  uintptr_t ReadMemoryPtr(uintptr_t Address);
  uintptr_t ReadMemoryPtr32(uintptr_t Address);
  INT ReadMemoryInt(uintptr_t Address);
  float ReadMemoryFloat(uintptr_t Address);
  BOOLEAN ReadMemoryBool(uintptr_t Address);

  NTSTATUS WriteMemoryPtr(uintptr_t Address, uintptr_t Value);
  NTSTATUS WriteMemoryInt(uintptr_t Address, INT Value);
  NTSTATUS WriteMemoryFloat(uintptr_t Address, float Value);
  NTSTATUS WriteMemoryBool(uintptr_t Address, BOOLEAN Value);

 private:
  NTSTATUS KeReadProcessMemory(PVOID SourceAddress, PVOID TargetAddress,
                               SIZE_T Size);
  NTSTATUS KeWriteProcessMemory(PVOID SourceAddress, PVOID TargetAddress,
                                SIZE_T Size);

  PEPROCESS m_Process = NULL;
};
}  // namespace Manager