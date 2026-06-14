#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>

class Logger : public QObject
{
    Q_OBJECT
public:
    static void init();
    static QString logFilePath();

private:
    Logger() = default;
    ~Logger() = default;
};

#endif // LOGGER_H