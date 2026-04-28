#include "logger.h"
#include <sys/stat.h>
#include <errno.h>
#include <cstring>

namespace eam
{

    Logger::~Logger()
    {
        if (logFile_.is_open())
        {
            logFile_.flush();
            logFile_.close();
        }
    }

    Logger &Logger::getInstance()
    {
        static Logger instance;
        return instance;
    }

    // 初始化，主要目的是创建LOG文件夹
    void Logger::initialize()
    {
        std::string logdir = "/var/log/EAM/";
        if (!createLogDir(logdir))
        {
            std::cerr << "Failed to create log directory: " << logdir << std::endl;
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;

        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d.log");

        logdir = logdir + ss.str();

        setLogFile(logdir);
    }

    bool Logger::createLogDir(const std::string &path)
    {
        if (path.empty())
        {
            return false;
        }

        struct stat info;

        if (stat(path.c_str(), &info) == 0)
        {
            if (S_ISDIR(info.st_mode))
            {
                return true;
            }
            std::cerr << "Path is exists but is not a directory: " << path << std::endl;
            return false;
        }

        std::string currentPath;
        size_t pos = 0;
        std::string tempPath = path;

        if (tempPath[0] != '/')
        {
            tempPath = '/' + tempPath;
        }

        while ((pos = tempPath.find('/', pos + 1)) != std::string::npos)
        {
            currentPath = tempPath.substr(0, pos);
            if (currentPath.empty() || currentPath == "/")
            {
                continue;
            }
            if (mkdir(currentPath.c_str(), 0755) != 0)
            {
                if (errno != EEXIST)
                {
                    std::cerr << "Failed to create directory" << currentPath
                              << "-" << strerror(errno) << std::endl;
                    return false;
                }
            }
        }
        if (mkdir(tempPath.c_str(), 0755) != 0)
        {
            if (errno != EEXIST)
            {
                std::cerr << "Failed to create directory" << tempPath
                          << "-" << strerror(errno) << std::endl;
                return false;
            }
        }
        return true;
    }

    void Logger::setLogFile(const std::string &filename)
    {

        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open())
            logFile_.close();
        logFile_.open(filename, std::ios::app);
        if (!logFile_.is_open())
            std::cerr << "Failed to open log file:" << filename << std::endl;
    }

    void Logger::log(LogLevel level, const std::string &message)
    {
        // 过滤级别
        if (level < minLevel_)
            return;

        std::stringstream ss;

        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        ss << " [" << levelToString(level) << "]";
        ss << " [Thread:" << std::this_thread::get_id() << "]";
        ss << " " << message;

        std::string formattedLog = ss.str();

        if (consoleOutput_)
        {
            if (level == LogLevel::ERROR)
                std::cout << "\033[31m"; // 红色
            else if (level == LogLevel::WARNING)
                std::cout << "\033[33m";
            else if (level == LogLevel::INFO)
                std::cout << "\033[32m";

            std::cout << formattedLog << std::endl;
            std::cout << "\033[0m";
        }
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open())
        {
            logFile_ << ss.str() << std::endl;
            if (level == LogLevel::ERROR)
                logFile_.flush();
        }
    }

    void Logger::flush()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open())
        {
            logFile_.flush();
        }
    }

    void Logger::setConsoleOutput(bool enable)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        consoleOutput_ = enable;
    }

    void Logger::setMinLevel(LogLevel level)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        minLevel_ = level;
    }

    std::string Logger::levelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }
}