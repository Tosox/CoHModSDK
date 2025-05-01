#include <Windows.h>
#include <intrin.h>
#include <Psapi.h>
#include <format>
#include <vector>
#include <iostream>

#include "MinHook.h"
#include "../include/CoHModSDK.hpp"

#pragma comment(lib, "psapi.lib")

namespace ModSDK {
	bool g_HooksInitialized = false;

    namespace Memory {
        std::uintptr_t FindPattern(const char* moduleName, const char* signature, bool reportError) {
            HMODULE moduleHandle = GetModuleHandleA(moduleName);
            if (!moduleHandle) {
                if (reportError) {
                    std::string errorMessage = std::format("Unable to get a handle for module '{}'", moduleName);
                    MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_ICONERROR);
                    throw std::runtime_error(errorMessage);
                }
                return 0;
            }

            static auto patternToBytes = [](const char* pattern) {
                std::vector<int> bytes;
                char* start = const_cast<char*>(pattern);
                char* end = const_cast<char*>(pattern) + std::strlen(pattern);

                for (char* current = start; current < end; ++current) {
                    if (*current == '?') {
                        ++current;
                        if (*current == '?') {
                            ++current;
                        }
                        bytes.push_back(-1);
                    }
                    else {
                        bytes.push_back(std::strtoul(current, &current, 16));
                    }
                }
                return bytes;
            };

            PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)(moduleHandle);
            PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)(moduleHandle)+dosHeader->e_lfanew);

            std::size_t sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
            std::vector<int> patternBytes = patternToBytes(signature);
            std::uint8_t* scanBytes = (std::uint8_t*)(moduleHandle);

            std::size_t patternBytesSize = patternBytes.size();
            int* data = patternBytes.data();

            for (std::size_t i = 0; i < sizeOfImage - patternBytesSize; ++i) {
                bool found = true;

                for (std::size_t j = 0; j < patternBytesSize; ++j) {
                    if ((scanBytes[i + j] != data[j]) && (data[j] != -1)) {
                        found = false;
                        break;
                    }
                }

                if (found) {
                    return (std::uintptr_t)(&scanBytes[i]);
                }
            }

            if (reportError) {
                std::string errorMessage = std::format("Unknown signature: {}", signature);
                MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_ICONERROR);
                throw std::runtime_error(errorMessage);
            }
            return 0;
        }

        void PatchMemory(void* destination, const void* source, std::size_t size) {
            DWORD oldProtect;
            VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &oldProtect);
            memcpy(destination, source, size);
            VirtualProtect(destination, size, oldProtect, &oldProtect);
        }
    }

    namespace Hooks {
        bool InitializeHooks() {
            if ((!g_HooksInitialized) && (MH_Initialize() == MH_OK)) {
                g_HooksInitialized = true;
            }
            return g_HooksInitialized;
        }

        bool CreateHook(void* targetFunction, void* detourFunction, void** originalFunction) {
            if (!InitializeHooks()) {
                return false;
            }

            if (MH_CreateHook(targetFunction, detourFunction, originalFunction) != MH_OK) {
                return false;
            }

            return true;
        }

		bool EnableHook(void* targetFunction) {
            if (!InitializeHooks()) {
                return false;
            }

			return (MH_EnableHook(targetFunction) == MH_OK);
		}

        bool EnableAllHooks() {
			return EnableHook(MH_ALL_HOOKS);
        }

		bool DisableHook(void* targetFunction) {
            if (!g_HooksInitialized) {
                return false;
            }

			return (MH_DisableHook(targetFunction) == MH_OK);
		}

        bool DisableAllHooks() {
            return DisableHook(MH_ALL_HOOKS);
        }
    }
}
