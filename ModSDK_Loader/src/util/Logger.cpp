#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "Logger.hpp"

// --- Logger ---
Logger::Logger() {
    CreateDirectoryA("mods", nullptr);
    CreateDirectoryA("mods/logs", nullptr);

	const char* mainLog = "mods/logs/sdk-loader.log";
	const char* oldLog = "mods/logs/sdk-loader.log.bak";

	// Remove old log file
    if (std::filesystem::exists(oldLog)) {
        std::filesystem::remove(oldLog);
	}

	// Rename the current log file to backup
    if (std::filesystem::exists(mainLog)) {
        std::filesystem::rename(mainLog, oldLog);
    }

	// Create a new main log file
	logFile.open(mainLog, std::ios::out | std::ios::trunc);

    if (logFile.is_open()) {
        LogInfo(std::string("Log file created: " + std::string(mainLog)));
    }
}

Logger::~Logger() {
	if (logFile.is_open()) {
		LogInfo("Closing log file");
		logFile.close();
	}
}

std::string Logger::GetCurrentTimestamp() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    char buffer[64];
    sprintf_s(buffer, "[%04d-%02d-%02d %02d:%02d:%02d]",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    return std::string(buffer);
}

void Logger::LogMessage(const std::string& level, const std::string& message) {
    logFile << GetCurrentTimestamp() << "[" << level << "] " << message << std::endl;
    logFile.flush();
}

// --- Public Logging Functions ---
void Logger::LogInfo(const std::string& message) {
    LogMessage("INFO", message);
}

void Logger::LogWarning(const std::string& message) {
    LogMessage("WARN", message);
}

void Logger::LogError(const std::string& message) {
    LogMessage("ERROR", message);
}
