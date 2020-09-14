#include "Memory.h"
#include "Offset.h"
#include "Process.h"

#define DebuggerPrint(...) \
  DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__);

typedef float FLOAT;
EXTERN_C INT _fltused = 0x9875;

BOOLEAN Exit = FALSE;

VOID DriverUnload(IN PDRIVER_OBJECT pDriver);
VOID Sleep(INT MilliSeconds);
VOID GlowWorker(IN PVOID pVoid);

EXTERN_C NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                              IN PUNICODE_STRING RegistryPath) {
  UNREFERENCED_PARAMETER(RegistryPath);

  NTSTATUS ReturnStatus = STATUS_SUCCESS;

  DriverObject->DriverUnload = DriverUnload;

  HANDLE hThread;
  PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL,
                       GlowWorker, NULL);

  return ReturnStatus;
}

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  Exit = TRUE;
  Sleep(1000);
}

VOID Sleep(INT MilliSeconds) {
  LARGE_INTEGER Time;
  Time.QuadPart = -10000;
  for (INT i = 0; i < MilliSeconds; i++) {
    KeDelayExecutionThread(KernelMode, FALSE, &Time);
  }
}

VOID GlowWorker(IN PVOID pVoid) {
  Manager::Process ProcessManager("csgo.exe");
  PEPROCESS GameProcess;
  NTSTATUS ReturnStatus = ProcessManager.GetProcess(&GameProcess);
  if (NT_SUCCESS(ReturnStatus)) {
    Manager::Memory MemoryManager(GameProcess);

    uintptr_t ClientModuleAddress =
        ProcessManager.GetModuleBaseAddress(L"client.dll");
    uintptr_t pLocalPlayer = ClientModuleAddress + Offset::dwLocalPlayer;
    uintptr_t pGlowObjectManager =
        ClientModuleAddress + Offset::dwGlowObjectManager;
    uintptr_t pEntityList = ClientModuleAddress + Offset::dwEntityList;

    while (TRUE) {
      if (Exit) {
        break;
      }

      uintptr_t LocalPlayer = MemoryManager.ReadMemoryPtr32(pLocalPlayer);
      uintptr_t GlowObjectManager =
          MemoryManager.ReadMemoryPtr32(pGlowObjectManager);
      uintptr_t EntityList = MemoryManager.ReadMemoryPtr32(pEntityList);

      if (LocalPlayer != NULL && GlowObjectManager != NULL &&
          EntityList != NULL) {
        INT MyTeamNumber =
            MemoryManager.ReadMemoryInt(LocalPlayer + Offset::m_iTeamNum);
        for (SHORT i = 0; i < 64; i++) {
          uintptr_t Entity =
              MemoryManager.ReadMemoryPtr32(pEntityList + i * 0x10);
          if (Entity == NULL) {
            continue;
          }
          if (MemoryManager.ReadMemoryBool(Entity + Offset::m_bDormant)) {
            continue;
          }

          INT GlowIndex =
              MemoryManager.ReadMemoryInt(Entity + Offset::m_iGlowIndex);
          INT EntityTeamNumber =
              MemoryManager.ReadMemoryInt(Entity + Offset::m_iTeamNum);
          INT EntityHealth =
              MemoryManager.ReadMemoryInt(Entity + Offset::m_iHealth);

          if (MyTeamNumber == EntityTeamNumber) {
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0x4), 0.f);
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0x8), 0.f);
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0xC), 1.f);
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0x10), 1.f);
          } else {
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0x4),
                (255 - 2.55f * EntityHealth) / 255.0f);
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0x8),
                (2.55f * EntityHealth) / 255.0f);
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0xC), 0.f);
            MemoryManager.WriteMemoryFloat(
                GlowObjectManager + (GlowIndex * 0x38 + 0x10), 1.f);
          }
          MemoryManager.WriteMemoryBool(
              GlowObjectManager + (GlowIndex * 0x38 + 0x24), TRUE);
          MemoryManager.WriteMemoryBool(
              GlowObjectManager + (GlowIndex * 0x38 + 0x25), FALSE);
        }
      }
      Sleep(1);
    }
  }
}