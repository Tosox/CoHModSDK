#include <Windows.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {
    constexpr char kOriginalGameDllName[] = "WW2Mod.dll";
    constexpr char kDefaultVanillaSdkFolder[] = "WW2\\ModSDK";
    constexpr char kScopedLoaderDllName[] = "CoHModSDKLoader.dll";
    constexpr char kModSdkFolderKey[] = "ModSDKFolder";

    using GetDllInterfaceFn = int(*)();
    using GetDllVersionFn = int(*)();
    using StdMutexCtorFn = void(__thiscall*)(void*, int);
    using StdInitLocksAssignFn = void*(__thiscall*)(void*, const void*);
    using LoaderInitializeFn = bool(*)();
    using LoaderShutdownFn = void(*)();

    SRWLOCK shimStateLock = SRWLOCK_INIT;
    HMODULE originalGameDll = nullptr;
    HMODULE scopedLoaderDll = nullptr;
    GetDllInterfaceFn fnGetDllInterface = nullptr;
    GetDllVersionFn fnGetDllVersion = nullptr;
    StdMutexCtorFn fnStdMutexCtor = nullptr;
    StdInitLocksAssignFn fnStdInitLocksAssign = nullptr;
    LoaderShutdownFn fnLoaderShutdown = nullptr;
    bool initialized = false;

    [[noreturn]] void FailFast(const std::string& message) {
        MessageBoxA(nullptr, message.c_str(), "CoHModSDK Shim Error", MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    std::filesystem::path GetGameDirectory() {
        char modulePath[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, modulePath, MAX_PATH) == 0) {
            FailFast("Failed to resolve the game executable path");
        }

        return std::filesystem::path(modulePath).parent_path();
    }

    std::string Trim(std::string value) {
        const std::size_t first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return {};
        }

        const std::size_t last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }

    bool EqualsIgnoreCase(const std::string& lhs, const char* rhs) {
        if (rhs == nullptr) {
            return false;
        }

        const std::size_t rhsLength = std::char_traits<char>::length(rhs);
        if (lhs.size() != rhsLength) {
            return false;
        }

        return std::equal(lhs.begin(), lhs.end(), rhs, [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
        });
    }

    std::vector<std::string> SplitCommandLine(const char* commandLine) {
        std::vector<std::string> args;
        if (commandLine == nullptr) {
            return args;
        }

        std::string current;
        for (const char* cursor = commandLine; *cursor != '\0'; ++cursor) {
            const char ch = *cursor;
            if ((ch == ' ') || (ch == '\t')) {
                if (!current.empty()) {
                    args.push_back(current);
                    current.clear();
                }
                continue;
            }

            current.push_back(ch);
        }

        if (!current.empty()) {
            args.push_back(current);
        }

        return args;
    }

    std::string GetModArgument() {
        const std::vector<std::string> args = SplitCommandLine(GetCommandLineA());
        for (std::size_t i = 0; i < args.size(); ++i) {
            const std::string& arg = args[i];
            if (EqualsIgnoreCase(arg, "-mod") && ((i + 1) < args.size())) {
                return args[i + 1];
            }
        }

        return {};
    }

    std::filesystem::path GetModuleFilePath(const std::filesystem::path& gameDirectory, const std::string& modArgument) {
        std::filesystem::path moduleFileName(modArgument);
        if (moduleFileName.is_absolute()) {
            FailFast("The -mod value must be a module file name relative to the game directory: " + modArgument);
        }

        if (!moduleFileName.has_extension()) {
            moduleFileName += ".module";
        }

        return gameDirectory / moduleFileName;
    }

    std::string ReadModSdkFolder(const std::filesystem::path& modulePath) {
        std::ifstream moduleFile(modulePath);
        if (!moduleFile.is_open()) {
            FailFast("Failed to open module file: " + modulePath.string());
        }

        bool inGlobalSection = false;
        std::string line;
        while (std::getline(moduleFile, line)) {
            line = Trim(std::move(line));
            if (line.empty() || (line[0] == ';') || (line[0] == '#')) {
                continue;
            }

            if ((line.front() == '[') && (line.back() == ']')) {
                const std::string sectionName = Trim(line.substr(1, line.size() - 2));
                inGlobalSection = EqualsIgnoreCase(sectionName, "global");
                continue;
            }

            if (!inGlobalSection) {
                continue;
            }

            const std::size_t separator = line.find('=');
            if (separator == std::string::npos) {
                continue;
            }

            const std::string key = Trim(line.substr(0, separator));
            if (!EqualsIgnoreCase(key, kModSdkFolderKey)) {
                continue;
            }

            return Trim(line.substr(separator + 1));
        }

        return {};
    }

    std::filesystem::path ResolveSdkRoot() {
        const std::filesystem::path gameDirectory = GetGameDirectory();
        const std::string modArgument = GetModArgument();
        if (modArgument.empty()) {
            return gameDirectory / kDefaultVanillaSdkFolder;
        }

        const std::filesystem::path modulePath = GetModuleFilePath(gameDirectory, modArgument);
        const std::string modSdkFolder = ReadModSdkFolder(modulePath);
        if (modSdkFolder.empty()) {
            FailFast("Missing required [global] ModSDKFolder in module file: " + modulePath.string());
        }

        std::filesystem::path sdkFolderPath(modSdkFolder);
        if (sdkFolderPath.is_absolute()) {
            FailFast("ModSDKFolder must be relative to the game directory: " + modSdkFolder);
        }

        return gameDirectory / sdkFolderPath;
    }

    void LoadOriginalGameDll() {
        const std::filesystem::path originalPath = GetGameDirectory() / kOriginalGameDllName;
        originalGameDll = LoadLibraryA(originalPath.string().c_str());
        if (originalGameDll == nullptr) {
            FailFast("Failed to load original game DLL: " + originalPath.string());
        }

        fnGetDllInterface = reinterpret_cast<GetDllInterfaceFn>(GetProcAddress(originalGameDll, "GetDllInterface"));
        fnGetDllVersion = reinterpret_cast<GetDllVersionFn>(GetProcAddress(originalGameDll, "GetDllVersion"));
        fnStdMutexCtor = reinterpret_cast<StdMutexCtorFn>(GetProcAddress(originalGameDll, "??0_Mutex@std@@QAE@W4_Uninitialized@1@@Z"));
        fnStdInitLocksAssign = reinterpret_cast<StdInitLocksAssignFn>(GetProcAddress(originalGameDll, "??4_Init_locks@std@@QAEAAV01@ABV01@@Z"));
        if ((fnGetDllInterface == nullptr) || (fnGetDllVersion == nullptr) || (fnStdMutexCtor == nullptr) || (fnStdInitLocksAssign == nullptr)) {
            FailFast("Original WW2Mod.dll is missing one or more required exports");
        }
    }

    void LoadScopedLoader() {
        const std::filesystem::path sdkRoot = ResolveSdkRoot();
        const std::filesystem::path loaderPath = sdkRoot / kScopedLoaderDllName;
        scopedLoaderDll = LoadLibraryA(loaderPath.string().c_str());
        if (scopedLoaderDll == nullptr) {
            FailFast("Failed to load scoped CoHModSDK loader: " + loaderPath.string());
        }

        const auto loaderInitialize = reinterpret_cast<LoaderInitializeFn>(GetProcAddress(scopedLoaderDll, "CoHModSDKLoader_Initialize"));
        fnLoaderShutdown = reinterpret_cast<LoaderShutdownFn>(GetProcAddress(scopedLoaderDll, "CoHModSDKLoader_Shutdown"));
        if ((loaderInitialize == nullptr) || (fnLoaderShutdown == nullptr)) {
            FailFast("Scoped CoHModSDKLoader.dll is missing required exports: " + loaderPath.string());
        }

        if (!loaderInitialize()) {
            FailFast("Scoped CoHModSDKLoader.dll failed to initialize: " + loaderPath.string());
        }
    }

    void EnsureInitialized() {
        AcquireSRWLockExclusive(&shimStateLock);
        if (!initialized) {
            LoadOriginalGameDll();
            LoadScopedLoader();
            initialized = true;
        }
        ReleaseSRWLockExclusive(&shimStateLock);
    }

    void Shutdown() {
        AcquireSRWLockExclusive(&shimStateLock);
        if (initialized) {
            initialized = false;
            if (fnLoaderShutdown != nullptr) {
                fnLoaderShutdown();
            }

            fnLoaderShutdown = nullptr;
            fnGetDllInterface = nullptr;
            fnGetDllVersion = nullptr;
            fnStdMutexCtor = nullptr;
            fnStdInitLocksAssign = nullptr;

            if (scopedLoaderDll != nullptr) {
                FreeLibrary(scopedLoaderDll);
                scopedLoaderDll = nullptr;
            }

            if (originalGameDll != nullptr) {
                FreeLibrary(originalGameDll);
                originalGameDll = nullptr;
            }
        }
        ReleaseSRWLockExclusive(&shimStateLock);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, unsigned long attachReason, void* reserved) {
    if (attachReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
    } else if (attachReason == DLL_PROCESS_DETACH) {
        Shutdown();
    }

    return TRUE;
}

extern "C" void __fastcall ForwardStdMutexCtor(void* _this, void* edx, int uninitialized) {
    EnsureInitialized();
    fnStdMutexCtor(_this, uninitialized);
}

extern "C" void* __fastcall ForwardStdInitLocksAssign(void* _this, void* edx, const void* other) {
    EnsureInitialized();
    return fnStdInitLocksAssign(_this, other);
}

extern "C" int GetDllInterface() {
    EnsureInitialized();
    return fnGetDllInterface();
}

extern "C" int GetDllVersion() {
    EnsureInitialized();
    return fnGetDllVersion();
}
