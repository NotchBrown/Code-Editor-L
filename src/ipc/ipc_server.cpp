#include "main.h"
#include "ipc/ipc_server.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

IpcServer::IpcServer(QObject *parent)
    : QObject(parent),
      m_server(nullptr),
      m_port(0),
      m_running(false)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &IpcServer::onNewConnection);
}

IpcServer::~IpcServer()
{
    stop();
}

bool IpcServer::start(int port)
{
    if (m_running) {
        stop();
    }

    if (m_server->listen(QHostAddress::LocalHost, port)) {
        m_port = m_server->serverPort();
        m_running = true;
        qDebug() << "IPC server started on port" << m_port;
        return true;
    }

    qDebug() << "Failed to start IPC server:" << m_server->errorString();
    return false;
}

void IpcServer::stop()
{
    if (m_running && m_server) {
        m_server->close();
        qDeleteAll(m_clientBuffers.keys());
        m_clientBuffers.clear();
        m_running = false;
        m_port = 0;
        qDebug() << "IPC server stopped";
    }
}

bool IpcServer::isRunning() const
{
    return m_running;
}

int IpcServer::port() const
{
    return m_port;
}

void IpcServer::onNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    if (socket) {
        connect(socket, &QTcpSocket::disconnected, this, &IpcServer::onClientDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &IpcServer::onReadyRead);
        m_clientBuffers[socket] = QString();
        emit clientConnected();
        qDebug() << "Client connected";
    }
}

void IpcServer::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_clientBuffers.remove(socket);
        socket->deleteLater();
        emit clientDisconnected();
        qDebug() << "Client disconnected";
    }
}

void IpcServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString &buffer = m_clientBuffers[socket];
    buffer += socket->readAll();

    while (true) {
        int newlinePos = buffer.indexOf('\n');
        if (newlinePos == -1) break;

        QString messageStr = buffer.left(newlinePos);
        buffer = buffer.mid(newlinePos + 1);

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(messageStr.toUtf8(), &error);

        if (error.error != QJsonParseError::NoError) {
            qDebug() << "Invalid JSON message:" << error.errorString();
            continue;
        }

        if (doc.isObject()) {
            QJsonObject message = doc.object();
            processMessage(socket, message);
            emit messageReceived(message);
        }
    }
}

void IpcServer::processMessage(QTcpSocket *socket, const QJsonObject &message)
{
    QString method = message["method"].toString();
    int requestId = message["id"].toInt(-1);

    QJsonObject result;

    if (method == "openFile") {
        QString filePath = message["params"]["filePath"].toString();
        result["filePath"] = filePath;
        result["success"] = true;
    } else if (method == "closeFile") {
        QString filePath = message["params"]["filePath"].toString();
        result["filePath"] = filePath;
        result["success"] = true;
    } else if (method == "newFile") {
        result["success"] = true;
    } else if (method == "saveFile") {
        QString filePath = message["params"]["filePath"].toString();
        QString content = message["params"]["content"].toString();
        result["filePath"] = filePath;
        result["success"] = true;
    } else if (method == "getFileContent") {
        QString filePath = message["params"]["filePath"].toString();
        result["filePath"] = filePath;
        result["content"] = QString();
        result["success"] = true;
    } else if (method == "exit") {
        result["success"] = true;
        sendResponse(socket, requestId, result);
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    } else if (method == "ping") {
        result["message"] = "pong";
        result["success"] = true;
    } else {
        sendError(socket, requestId, QString("Unknown method: %1").arg(method));
        return;
    }

    sendResponse(socket, requestId, result);
}

void IpcServer::sendResponse(QTcpSocket *socket, int requestId, const QJsonObject &result)
{
    QJsonObject response;
    response["id"] = requestId;
    response["result"] = result;

    QJsonDocument doc(response);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void IpcServer::sendError(QTcpSocket *socket, int requestId, const QString &error)
{
    QJsonObject response;
    response["id"] = requestId;
    response["error"] = error;

    QJsonDocument doc(response);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}