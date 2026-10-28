#include "Logger.h"
#include "errors.h"

#include <ctime>
#include <string>

Logger::Logger(const std::string& fileName)
    : file(fileName, std::ios::app)
{
    if (!file.is_open()) {
        throw FileError("Cannot open log file: " + fileName);
    }
}

static std::string timeStamp() {
    std::time_t t = std::time(nullptr);
    std::tm tm;

    localtime_r(&t, &tm);

    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);

    return std::string(buf);
}

void Logger::write(const std::string& level, const std::string& msg) {
    file << "[" << timeStamp() << "] " << level << " " << msg << "\n";
    file.flush();
}

void Logger::logError(const std::string& msg) {
    write("[CRITICAL]", msg);
}

void Logger::logWarning(const std::string& msg) {
    write("[NOT CRITICAL]", msg);
}

void Logger::logInfo(const std::string& msg) {
    write("[INFO]", msg);
}