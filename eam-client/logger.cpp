#include "logger.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QStringConverter>
#include <QDate>
#include <QCoreApplication>
#include <QDir>

Logger::~Logger()
{
    if(m_logFile.isOpen())
    {
        m_out.flush();
        m_logFile.close();
    }
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::initialize()
{

    QString logPath = QCoreApplication::applicationDirPath()+"/"+"logs";
    QString logFileName=QDate::currentDate().toString("yyyy-MM-dd");
    QDir dir;
    if(!dir.exists(logPath))
    {
        if(!dir.mkpath(logPath))
            qDebug()<<"Fialed to create log directory"<<logPath;
    }
    logFileName = logPath+"/"+logFileName+".log";
    qDebug()<<logFileName;
    setLogFile(logFileName);
}

void Logger::setLogFile(const QString &filename)
{
    QMutexLocker locker{&m_mutex};
    if(m_logFile.isOpen())
        m_logFile.close();
    m_logFile.setFileName(filename);
    if(m_logFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        m_out.setDevice(&m_logFile);
        m_out.setEncoding(QStringConverter::Utf8);
    }else{
        qDebug()<<"Open log file failed:"<<filename;
    }
}

void Logger::log(LogLevel level, const QString &message)
{
    if(level>m_minLevel)
        return;

    QString logLine;
    QTextStream stream(&logLine);

    QDate currentDate = QDate::currentDate();
    QTime now = QTime::currentTime();
    stream<<currentDate.toString("yyyy-MM-dd ")
           << now.toString("hh:mm:ss.zzz")
           << " [" << levelToString(level) << "]"
           << " [Thread:" << QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()), 16) << "]"
           << " " << message;

    // logLine = QString("%1 [%2] [Thread:%3] %4")
    //               .arg(now.toString("hh:mm:ss.zzz")
    //                    ,levelToString(level)
    //                    ,QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()),16)
    //                    ,message);

    QMutexLocker locker{&m_mutex};

    if(m_logFile.isOpen()){
        m_out<<logLine<<Qt::endl;
       // m_out.flush();
    }
    if(m_consoleOutput)
    {
        if(level==LogLevel::ERROR){
            qDebug().noquote()<<"\033[31m"<<logLine<<"\033[0m";
        } else if(level==LogLevel::WARNING){
            qDebug().noquote()<<"\033[33m"<<logLine<<"\033[0m";
        }else if(level==LogLevel::INFO){
            qDebug().noquote()<<"\033[32m"<<logLine<<"\033[0m";
        }else{
            qDebug().noquote()<<logLine;
        }
    }
}

void Logger::setMinLevel(LogLevel level)
{
    QMutexLocker locker{&m_mutex};
    m_minLevel = level;
}

void Logger::flush()
{
    QMutexLocker locker{&m_mutex};
    if(m_logFile.isOpen())
    {
        m_out.flush();
    }
}

QString Logger::levelToString(LogLevel level)
{
    switch (level) {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARNING";
    default:
        return "UNKNOWN";
    }
}
