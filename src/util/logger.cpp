#include "util/logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDateTime>

void Logger::init()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString logDirPath = QDir(appDataPath).filePath("log");
    
    QDir logDir(logDirPath);
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }
    
    QString logFilePath = logDir.filePath("debug.log");
    
    static QFile logFile(logFilePath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        static QTextStream logStream(&logFile);
        qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
            QTextStream &stream = logStream;
            QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            
            QString typeStr;
            switch (type) {
            case QtDebugMsg:
                typeStr = "DEBUG";
                break;
            case QtInfoMsg:
                typeStr = "INFO";
                break;
            case QtWarningMsg:
                typeStr = "WARNING";
                break;
            case QtCriticalMsg:
                typeStr = "CRITICAL";
                break;
            case QtFatalMsg:
                typeStr = "FATAL";
                break;
            }
            
            stream << QString("[%1] [%2] %3").arg(time).arg(typeStr).arg(msg) << endl;
            stream.flush();
        });
    }
}

QString Logger::logFilePath()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString logDirPath = QDir(appDataPath).filePath("log");
    return QDir(logDirPath).filePath("debug.log");
}