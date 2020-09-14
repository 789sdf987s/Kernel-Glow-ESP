#include "Process.h"

Manager::Process::Process(CHAR* ProcessName) {
  FindProcessByName(ProcessName);
}

uintptr_t Manager::Process::GetModuleBaseAddress(WCHAR* ModuleName) {
  fnPsGetProcessWow64Process PsGetProcessWow64Process;
  UNICODE_STRING routineName;
  RtlInitUnicodeString(&routineName, L"PsGetProcessWow64Process");
  PsGetProcessWow64Process =
      (fnPsGetProcessWow64Process)MmGetSystemRoutineAddress(&routineName);

  KAPC_STATE AttachState;
  KeStackAttachProcess(m_Process, &AttachState);

  PPEB32 pPeb32 = (PPEB32)PsGetProcessWow64Process(m_Process);
  if (!pPeb32 || !pPeb32->Ldr) {
    KeUnstackDetachProcess(&AttachState);
    return 0;
  }

  for (PLIST_ENTRY32 pListEntry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)pPeb32->Ldr)
                                      ->InLoadOrderModuleList.Flink;
       pListEntry != &((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList;
       pListEntry = (PLIST_ENTRY32)pListEntry->Flink) {
    PLDR_DATA_TABLE_ENTRY32 pEntry =
        CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

    if (wcscmp((PWCH)pEntry->BaseDllName.Buffer, ModuleName) == 0) {
      uintptr_t ModuleAddress = pEntry->DllBase;
      KeUnstackDetachProcess(&AttachState);
      return ModuleAddress;
    }
  }

  KeUnstackDetachProcess(&AttachState);
  return 0;
}

NTSTATUS Manager::Process::GetProcess(PEPROCESS* pProcess) { 
  if (m_Process != NULL) {
    *pProcess = m_Process;
    return STATUS_SUCCESS;
  } else {
    return STATUS_NOT_FOUND;
  }
}

NTSTATUS Manager::Process::FindProcessByName(CHAR* ProcessName) {
  PEPROCESS SystemProcess = PsInitialSystemProcess;
  PEPROCESS CurrentEntry = SystemProcess;

  CHAR ImageName[15];

  do {
    RtlCopyMemory((PVOID)(&ImageName), (PVOID)((uintptr_t)CurrentEntry + 0x450),
                  sizeof(ImageName));

    if (strstr(ImageName, ProcessName)) {
      DWORD32 ActiveThreads;
      RtlCopyMemory((PVOID)&ActiveThreads,
                    (PVOID)((uintptr_t)CurrentEntry + 0x498),
                    sizeof(ActiveThreads));
      if (ActiveThreads) {
        m_Process = CurrentEntry;
        return STATUS_SUCCESS;
      }
    }

    PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(CurrentEntry) + 0x2F0);
    CurrentEntry = (PEPROCESS)((uintptr_t)list->Flink - 0x2F0);

  } while (CurrentEntry != SystemProcess);

  return STATUS_NOT_FOUND;
}