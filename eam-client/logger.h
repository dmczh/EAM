#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMutex>
#include <QTextStream>
#include <QFile>

enum class LogLevel
{
    ERROR,
    WARNING,
    INFO,
    DEBUG,
};

class Logger
{
private:
    Logger()=default;
    ~Logger();

    QFile m_logFile;
    QMutex m_mutex;
    QTextStream m_out;
    bool m_consoleOutput{true};
    std::atomic<LogLevel> m_minLevel{LogLevel::DEBUG};
    QString levelToString(LogLevel level);
    void setLogFile(const QString& filename);

    Logger(const Logger&)=delete;
    Logger& operator=(const Logger)=delete;

public:
    static Logger& getInstance();
    void initialize();
    void log(LogLevel level,const QString& message);
    void setMinLevel(LogLevel level);
    void setLogDir(QString& dir);
    void flush();
};

#define LOG_ERROR(msg)\
Logger::getInstance().log(LogLevel::ERROR,\
                                              QString(__FILE__)+":"+QString::number(__LINE__)+"("+__FUNCTION__+") "+msg )
#define LOG_WARNING(msg)\
    Logger::getInstance().log(LogLevel::WARNING,\
                                   QString(__FILE__)+":"+QString::number(__LINE__)+"("+__FUNCTION__+") "+msg )
#define LOG_INFO(msg)\
    Logger::getInstance().log(LogLevel::INFO,\
                                QString(__FILE__)+":"+QString::number(__LINE__)+"("+__FUNCTION__+") "+msg )
#define LOG_DEBUG(msg)\
    Logger::getInstance().log(LogLevel::DEBUG,\
                                 QString(__FILE__)+":"+QString::number(__LINE__)+"("+__FUNCTION__+") "+msg )


#endif // LOGGER_H
