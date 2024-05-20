#include "CustomLogger.h"

namespace {
    const char* logLevelToString(crow::LogLevel level) {
        switch (level) {
            case crow::LogLevel::Debug: return "DEBUG";
            case crow::LogLevel::Info: return "INFO";
            case crow::LogLevel::Warning: return "WARNING";
            case crow::LogLevel::Error: return "ERROR";
            case crow::LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
}

void CustomLogger::log(std::string message, crow::LogLevel level)
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    struct tm buf;
    localtime_r(&in_time_t, &buf);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &buf);

    std::cout << "(" << buffer << ") [" << logLevelToString(level) << "] " << message << std::endl;
}
