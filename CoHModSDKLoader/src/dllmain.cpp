#include <Windows.h>

#include "core/Loader.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, unsigned long attachReason, void* reserved) {
    switch (attachReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        Loader::SetModuleHandle(hModule);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
