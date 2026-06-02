#include "Loader.hpp"

#include <Windows.h>

#include <cstdlib>
#include <filesystem>

#include "mods/ModLoader.hpp"
#include "RuntimeBridge.hpp"

namespace {
    HMODULE loaderModule = nullptr;
    SRWLOCK loaderStateLock = SRWLOCK_INIT;
    bool loaderInitialized = false;
}

namespace Loader {
    void SetModuleHandle(HMODULE moduleHandle) {
        loaderModule = moduleHandle;
    }

    void EnsureInitialized() {
        AcquireSRWLockExclusive(&loaderStateLock);
        if (!loaderInitialized) {
            LoadRuntime();
            LoadConfiguredMods();
            EnableAllHooks();
            loaderInitialized = true;
        }
        ReleaseSRWLockExclusive(&loaderStateLock);
    }

    void Shutdown() {
        AcquireSRWLockExclusive(&loaderStateLock);
        if (loaderInitialized) {
            loaderInitialized = false;
            NotifyModsShutdown();
            ShutdownRuntime();
        }
        ReleaseSRWLockExclusive(&loaderStateLock);
    }

    [[noreturn]] void FailFast(const std::string& message) {
        MessageBoxA(nullptr, message.c_str(), "CoHModSDK Loader Error", MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    std::filesystem::path GetDirectory() {
        char modulePath[MAX_PATH] = {};
        if ((loaderModule == nullptr) || (GetModuleFileNameA(loaderModule, modulePath, MAX_PATH) == 0)) {
            FailFast("Failed to resolve the loader path");
        }

        return std::filesystem::path(modulePath).parent_path();
    }

    std::filesystem::path GetRelativePath(const char* fileName) {
        return GetDirectory() / fileName;
    }
}

extern "C" __declspec(dllexport) bool CoHModSDKLoader_Initialize() {
    Loader::EnsureInitialized();
    return true;
}

extern "C" __declspec(dllexport) void CoHModSDKLoader_Shutdown() {
    Loader::Shutdown();
}
