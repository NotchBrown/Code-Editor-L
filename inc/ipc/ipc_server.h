#ifndef IPC_SERVER_H
#define IPC_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QMap>

class IpcServer : public QObject
{
    Q_OBJECT

public:
    explicit IpcServer(QObject *parent = nullptr);
    ~IpcServer();

    bool start(int port);
    void stop();
    bool isRunning() const;
    int port() const;

signals:
    void clientConnected();
    void clientDisconnected();
    void messageReceived(const QJsonObject &message);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();

private:
    void processMessage(QTcpSocket *socket, const QJsonObject &message);
    void sendResponse(QTcpSocket *socket, int requestId, const QJsonObject &result);
    void sendError(QTcpSocket *socket, int requestId, const QString &error);

    QTcpServer *m_server;
    QMap<QTcpSocket*, QString> m_clientBuffers;
    int m_port;
    bool m_running;
};

#endif // IPC_SERVER_H