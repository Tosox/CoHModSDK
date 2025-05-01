#include <Windows.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

#include "../util/Logger.hpp"

// --- Globals ---
Logger logger;
HMODULE ww2ModDll = nullptr;
std::vector<HMODULE> loadedMods;

// Typedefs for mod exports
typedef void(*OnSDKLoadFunc)();
typedef void(*OnGameStartFunc)();
typedef void(*OnGameShutdownFunc)();

void LoadOriginalWW2Mod() {
    ww2ModDll = LoadLibraryA("WW2Mod.dll");
    if (!ww2ModDll) {
        MessageBoxA(nullptr, "Failed to load the original WW2Mod.dll", "Error", MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }
}

// --- Mod Loader ---
void LoadMods() {
    CreateDirectoryA("mods", nullptr);
    CreateDirectoryA("mods/logs", nullptr);

    std::ifstream config("CoHModSDKLoader.ini");
    if (!config.is_open()) {
        logger.LogError("CoHModSDK.ini not found");
        return;
    }

    logger.LogInfo("CoHModSDK.ini found, loading listed mods");

    std::string line;
    while (std::getline(config, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip comments and empty lines
        if ((line.empty()) || (line[0] == '#')) {
            continue;
        }

        std::string path = "mods/" + line;
        HMODULE modHandle = LoadLibraryA(path.c_str());
        if (!modHandle) {
            logger.LogError("Failed to load mod: " + path);
            continue;
        }

        loadedMods.push_back(modHandle);
        logger.LogInfo("Loaded mod: " + path);

        // Notify SDK Load
        if (auto onLoad = (OnSDKLoadFunc)GetProcAddress(modHandle, "OnSDKLoad")) {
            onLoad();
            logger.LogInfo("Called OnSDKLoad for " + path);
        }
    }
}

// --- Game Lifecycle ---
void NotifyGameStart() {
    for (auto mod : loadedMods) {
        if (auto onStart = (OnGameStartFunc)GetProcAddress(mod, "OnGameStart")) {
            onStart();
            logger.LogInfo("Called OnGameStart for a mod");
        }
    }
}

void NotifyGameShutdown() {
    for (auto mod : loadedMods) {
        if (auto onShutdown = (OnGameShutdownFunc)GetProcAddress(mod, "OnGameShutdown")) {
            onShutdown();
            logger.LogInfo("Called OnGameShutdown for a mod");
        }
    }
}

void OnProcessAttach() {
    LoadOriginalWW2Mod();
    LoadMods();
    NotifyGameStart();
}

void OnProcessDetach() {
    NotifyGameShutdown();
}

BOOL APIENTRY DllMain(HMODULE hModule, unsigned long attachReason, void* reserved) {
	DisableThreadLibraryCalls(hModule);

	switch (attachReason) {
	case DLL_PROCESS_ATTACH:
		OnProcessAttach();
		break;
	case DLL_PROCESS_DETACH:
        OnProcessDetach();
		break;
	}

	return TRUE;
}

// --- Proxy WW2Mod.dll Exports ---
extern "C" __declspec(dllexport) int GetDllInterface() {
    using GetDllInterface = int(*)();
    static GetDllInterface fnGetDllInterface = (GetDllInterface)GetProcAddress(ww2ModDll, "GetDllInterface");
    return fnGetDllInterface();
}

extern "C" __declspec(dllexport) int GetDllVersion() {
    using GetDllVersion = int(*)();
    static GetDllVersion fnGetDllVersion = (GetDllVersion)GetProcAddress(ww2ModDll, "GetDllVersion");
    return fnGetDllVersion();
}
