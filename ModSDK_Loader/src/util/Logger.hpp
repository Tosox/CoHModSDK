#pragma once

#include <string>

class Logger {
public:
	Logger();
	~Logger();

	void LogInfo(const std::string& message);
	void LogWarning(const std::string& message);
	void LogError(const std::string& message);
private:
	std::string GetCurrentTimestamp();
	void LogMessage(const std::string& level, const std::string& message);

private:
	std::ofstream logFile;
	std::string logFileName;
};
