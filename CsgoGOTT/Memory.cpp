#include "Memory.h"

Manager::Memory::Memory(PEPROCESS Process) { m_Process = Process; }

uintptr_t Manager::Memory::ReadMemoryPtr(uintptr_t Address) {
  if (m_Process == NULL) {
    return NULL;
  }

  uintptr_t Value = NULL;
  NTSTATUS MemoryStatus =
      KeReadProcessMemory((PVOID)Address, &Value, sizeof(uintptr_t));
  if (NT_SUCCESS(MemoryStatus)) {
    return Value;
  } else {
    return NULL;
  }
}

uintptr_t Manager::Memory::ReadMemoryPtr32(uintptr_t Address) {
  if (m_Process == NULL) {
    return NULL;
  }

  uintptr_t Value = NULL;
  NTSTATUS MemoryStatus =
      KeReadProcessMemory((PVOID)Address, &Value, sizeof(uintptr_t));
  if (NT_SUCCESS(MemoryStatus)) {
    return (uintptr_t)PtrToPtr32((PVOID)Value);
  } else {
    return NULL;
  }
}

INT Manager::Memory::ReadMemoryInt(uintptr_t Address) {
  if (m_Process == NULL) {
    return NULL;
  }

  INT Value = NULL;
  NTSTATUS MemoryStatus =
      KeReadProcessMemory((PVOID)Address, &Value, sizeof(INT));
  if (NT_SUCCESS(MemoryStatus)) {
    return Value;
  } else {
    return NULL;
  }
}

float Manager::Memory::ReadMemoryFloat(uintptr_t Address) {
  if (m_Process == NULL) {
    return NULL;
  }

  float Value = NULL;
  NTSTATUS MemoryStatus =
      KeReadProcessMemory((PVOID)Address, &Value, sizeof(float));
  if (NT_SUCCESS(MemoryStatus)) {
    return Value;
  } else {
    return NULL;
  }
}

BOOLEAN Manager::Memory::ReadMemoryBool(uintptr_t Address) {
  if (m_Process == NULL) {
    return NULL;
  }

  uintptr_t Value = NULL;
  NTSTATUS MemoryStatus =
      KeReadProcessMemory((PVOID)Address, &Value, sizeof(BOOLEAN));
  if (NT_SUCCESS(MemoryStatus)) {
    return Value;
  } else {
    return NULL;
  }
}

NTSTATUS Manager::Memory::WriteMemoryPtr(uintptr_t Address, uintptr_t Value) {
  if (m_Process == NULL) {
    return STATUS_NOT_FOUND;
  }

  return KeWriteProcessMemory(&Value, (PVOID)Address, sizeof(uintptr_t));
}

NTSTATUS Manager::Memory::WriteMemoryInt(uintptr_t Address, INT Value) {
  if (m_Process == NULL) {
    return STATUS_NOT_FOUND;
  }

  return KeWriteProcessMemory(&Value, (PVOID)Address, sizeof(INT));
}

NTSTATUS Manager::Memory::WriteMemoryFloat(uintptr_t Address, float Value) {
  if (m_Process == NULL) {
    return STATUS_NOT_FOUND;
  }

  return KeWriteProcessMemory(&Value, (PVOID)Address, sizeof(float));
}

NTSTATUS Manager::Memory::WriteMemoryBool(uintptr_t Address, BOOLEAN Value) {
  if (m_Process == NULL) {
    return STATUS_NOT_FOUND;
  }

  return KeWriteProcessMemory(&Value, (PVOID)Address, sizeof(BOOLEAN));
}

NTSTATUS Manager::Memory::KeReadProcessMemory(PVOID SourceAddress,
                                              PVOID TargetAddress,
                                              SIZE_T Size) {
  PEPROCESS SourceProcess = m_Process;
  PEPROCESS TargetProcess = IoGetCurrentProcess();
  SIZE_T Result;
  if (NT_SUCCESS(MmCopyVirtualMemory(SourceProcess, SourceAddress,
                                     TargetProcess, TargetAddress, Size,
                                     KernelMode, &Result))) {
    return STATUS_SUCCESS;
  } else {
    return STATUS_ACCESS_DENIED;
  }
}

NTSTATUS Manager::Memory::KeWriteProcessMemory(PVOID SourceAddress,
                                               PVOID TargetAddress,
                                               SIZE_T Size) {
  PEPROCESS SourceProcess = IoGetCurrentProcess();
  PEPROCESS TargetProcess = m_Process;
  SIZE_T Result;

  if (NT_SUCCESS(MmCopyVirtualMemory(SourceProcess, SourceAddress,
                                     TargetProcess, TargetAddress, Size,
                                     KernelMode, &Result))) {
    return STATUS_SUCCESS;
  } else {
    return STATUS_ACCESS_DENIED;
  }
}