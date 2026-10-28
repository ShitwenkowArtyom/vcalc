#pragma once

#include <fstream>
#include <string>

class Logger {
public:
    Logger() = delete;

    Logger(const std::string& fileName);

    void logError(const std::string& msg);

    void logWarning(const std::string& msg);

    void logInfo(const std::string& msg);

private:
    std::ofstream file;

    void write(const std::string& level, const std::string& msg);
};