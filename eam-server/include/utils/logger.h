#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#include <string>
#include <atomic>

namespace eam
{

    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
    };

    class Logger
    {
    public:
        static Logger &getInstance();
        void initialize();
        void setLogFile(const std::string &filename);
        void log(LogLevel level, const std::string &message);
        void flush();
        void setConsoleOutput(bool enable);
        void setMinLevel(LogLevel level);

    private:
        Logger() = default;
        ~Logger();
        std::string levelToString(LogLevel level);

        std::ofstream logFile_;
        std::mutex mutex_;
        std::atomic<bool> consoleOutput_{true};
        std::atomic<LogLevel> minLevel_{LogLevel::DEBUG};
        bool createLogDir(const std::string &path);
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;
    };

}
// 带代码定位的日志宏
#define EAM_LOG_DEBUG(msg)                         \
    eam::Logger::getInstance().log(eam::LogLevel::DEBUG, \
                              std::string(__FILE__) + ":" + std::to_string(__LINE__) + " (" + __FUNCTION__ + ") " + msg)

#define EAM_LOG_INFO(msg)                         \
    eam::Logger::getInstance().log(eam::LogLevel::INFO, \
                              std::string(__FILE__) + ":" + std::to_string(__LINE__) + " (" + __FUNCTION__ + ") " + msg)

#define EAM_LOG_WARNING(msg)                         \
    eam::Logger::getInstance().log(eam::LogLevel::WARNING, \
                              std::string(__FILE__) + ":" + std::to_string(__LINE__) + " (" + __FUNCTION__ + ") " + msg)

#define EAM_LOG_ERROR(msg)                         \
    eam::Logger::getInstance().log(eam::LogLevel::ERROR, \
                              std::string(__FILE__) + ":" + std::to_string(__LINE__) + " (" + __FUNCTION__ + ") " + msg)
