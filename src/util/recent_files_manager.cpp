#include "util/recent_files_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDateTime>

RecentFilesManager* RecentFilesManager::m_instance = nullptr;

RecentFilesManager::RecentFilesManager(QObject *parent)
    : QObject(parent),
      m_maxFiles(10)
{
    loadRecentFiles();
}

RecentFilesManager::~RecentFilesManager()
{
    saveRecentFiles();
}

RecentFilesManager* RecentFilesManager::instance()
{
    if (!m_instance) {
        m_instance = new RecentFilesManager();
    }
    return m_instance;
}

QString RecentFilesManager::getConfigFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(dataPath);
    }
    return dataPath + "/recent_files.xml";
}

void RecentFilesManager::loadRecentFiles()
{
    QString filePath = getConfigFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QXmlStreamReader reader(&file);
    m_recentFiles.clear();

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "recentFiles") {
                continue;
            }

            if (reader.name() == "file") {
                QString path = reader.readElementText();
                if (!path.isEmpty() && QFile::exists(path)) {
                    m_recentFiles.append(path);
                }
            }
        }
    }

    file.close();
}

void RecentFilesManager::saveRecentFiles()
{
    QString filePath = getConfigFilePath();
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("recentFiles");

    for (const QString &path : m_recentFiles) {
        writer.writeStartElement("file");
        writer.writeAttribute("timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
        writer.writeCharacters(path);
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();

    file.close();
}

QStringList RecentFilesManager::recentFiles() const
{
    return m_recentFiles;
}

void RecentFilesManager::addFile(const QString &filePath)
{
    // Remove if already exists
    m_recentFiles.removeAll(filePath);

    // Add to beginning
    m_recentFiles.prepend(filePath);

    // Trim to max files
    while (m_recentFiles.size() > m_maxFiles) {
        m_recentFiles.removeLast();
    }

    saveRecentFiles();
    emit recentFilesChanged(m_recentFiles);
}

void RecentFilesManager::removeFile(const QString &filePath)
{
    m_recentFiles.removeAll(filePath);
    saveRecentFiles();
    emit recentFilesChanged(m_recentFiles);
}

void RecentFilesManager::clear()
{
    m_recentFiles.clear();
    saveRecentFiles();
    emit recentFilesChanged(m_recentFiles);
}

int RecentFilesManager::maxFiles() const
{
    return m_maxFiles;
}

void RecentFilesManager::setMaxFiles(int max)
{
    m_maxFiles = max;
    while (m_recentFiles.size() > m_maxFiles) {
        m_recentFiles.removeLast();
    }
    saveRecentFiles();
}