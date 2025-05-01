#include <Windows.h>
#include <iostream>
#include <fstream>

#include "Logger.hpp"

// --- Logger ---
Logger::Logger() {
    CreateDirectoryA("mods", nullptr);
    CreateDirectoryA("mods/logs", nullptr);

    SYSTEMTIME st;
    GetLocalTime(&st);

    char filename[128];
    sprintf_s(filename, "mods/logs/%04d-%02d-%02d_%02d-%02d-%02d.log",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    logFileName = filename;
    logFile.open(logFileName, std::ios::out | std::ios::trunc);

    if (logFile.is_open()) {
        LogInfo(std::string("Log file created: " + logFileName));
    }
}

Logger::~Logger() {
	if (logFile.is_open()) {
		LogInfo("Closing log file");
		logFile.close();
	}
}

// Helper: Get current date/time string
std::string Logger::GetCurrentTimestamp() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    char buffer[64];
    sprintf_s(buffer, "[%04d-%02d-%02d %02d:%02d:%02d]",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    return std::string(buffer);
}

// Helper: Log message
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
