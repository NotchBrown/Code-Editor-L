#include "main.h"
#include "project/project_manager.h"
#include <QDebug>

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent),
      m_projectOpen(false)
{
}

ProjectManager::~ProjectManager()
{
    closeProject();
}

bool ProjectManager::createProject(const QString &projectPath, const QString &projectName)
{
    QDir dir(projectPath);
    if (!dir.exists()) {
        if (!dir.mkpath(projectPath)) {
            qDebug() << "Failed to create project directory:" << projectPath;
            return false;
        }
    }

    m_projectName = projectName;
    m_projectPath = projectPath;
    m_projectFilePath = QString("%1/%2.celproj").arg(projectPath).arg(projectName);
    m_files.clear();
    m_projectOpen = true;

    emit projectOpened(projectPath);
    return writeProjectFile(m_projectFilePath);
}

bool ProjectManager::openProject(const QString &projectFilePath)
{
    if (!QFile::exists(projectFilePath)) {
        qDebug() << "Project file does not exist:" << projectFilePath;
        return false;
    }

    closeProject();

    if (!readProjectFile(projectFilePath)) {
        return false;
    }

    m_projectFilePath = projectFilePath;
    QFileInfo info(projectFilePath);
    m_projectPath = info.dir().absolutePath();
    m_projectOpen = true;

    emit projectOpened(m_projectPath);
    return true;
}

void ProjectManager::closeProject()
{
    if (m_projectOpen) {
        m_projectOpen = false;
        m_projectName.clear();
        m_projectPath.clear();
        m_projectFilePath.clear();
        m_files.clear();
        emit projectClosed();
    }
}

bool ProjectManager::saveProject()
{
    if (!m_projectOpen) {
        qDebug() << "No project open";
        return false;
    }

    return writeProjectFile(m_projectFilePath);
}

bool ProjectManager::addFile(const QString &filePath)
{
    if (!m_projectOpen) {
        qDebug() << "No project open";
        return false;
    }

    QFileInfo info(filePath);
    QString absPath = info.absoluteFilePath();
    
    if (m_files.contains(absPath)) {
        qDebug() << "File already in project:" << filePath;
        return false;
    }

    m_files.append(absPath);
    emit fileAdded(absPath);
    return true;
}

bool ProjectManager::removeFile(const QString &filePath)
{
    if (!m_projectOpen) {
        qDebug() << "No project open";
        return false;
    }

    QFileInfo info(filePath);
    QString absPath = info.absoluteFilePath();
    
    if (!m_files.contains(absPath)) {
        qDebug() << "File not in project:" << filePath;
        return false;
    }

    m_files.removeOne(absPath);
    emit fileRemoved(absPath);
    return true;
}

bool ProjectManager::renameFile(const QString &oldPath, const QString &newPath)
{
    if (!m_projectOpen) {
        qDebug() << "No project open";
        return false;
    }

    QFileInfo oldInfo(oldPath);
    QString oldAbsPath = oldInfo.absoluteFilePath();
    
    QFileInfo newInfo(newPath);
    QString newAbsPath = newInfo.absoluteFilePath();
    
    int index = m_files.indexOf(oldAbsPath);
    if (index == -1) {
        qDebug() << "File not in project:" << oldPath;
        return false;
    }

    m_files[index] = newAbsPath;
    emit fileRenamed(oldAbsPath, newAbsPath);
    return true;
}

QString ProjectManager::projectName() const
{
    return m_projectName;
}

QString ProjectManager::projectPath() const
{
    return m_projectPath;
}

QString ProjectManager::projectFilePath() const
{
    return m_projectFilePath;
}

QStringList ProjectManager::files() const
{
    return m_files;
}

bool ProjectManager::isProjectOpen() const
{
    return m_projectOpen;
}

bool ProjectManager::isFileInProject(const QString &filePath) const
{
    QFileInfo info(filePath);
    return m_files.contains(info.absoluteFilePath());
}

bool ProjectManager::readProjectFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open project file:" << filePath;
        return false;
    }

    QTextStream stream(&file);
    QString line;
    
    while (!stream.atEnd()) {
        line = stream.readLine().trimmed();
        
        if (line.startsWith("[Project]")) {
            while (!stream.atEnd()) {
                line = stream.readLine().trimmed();
                if (line.isEmpty()) break;
                
                QStringList parts = line.split("=", QString::SkipEmptyParts);
                if (parts.size() == 2) {
                    QString key = parts[0].trimmed();
                    QString value = parts[1].trimmed();
                    
                    if (key == "Name") {
                        m_projectName = value;
                    }
                }
            }
        } else if (line.startsWith("[Files]")) {
            while (!stream.atEnd()) {
                line = stream.readLine().trimmed();
                if (line.isEmpty() || line.startsWith("[")) break;
                
                if (!line.isEmpty()) {
                    m_files.append(line);
                }
            }
        }
    }

    file.close();
    return true;
}

bool ProjectManager::writeProjectFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open project file for writing:" << filePath;
        return false;
    }

    QTextStream stream(&file);
    
    stream << "[Project]\n";
    stream << QString("Name=%1\n").arg(m_projectName);
    stream << "\n";
    
    stream << "[Files]\n";
    foreach (const QString &filePath, m_files) {
        stream << filePath << "\n";
    }

    file.close();
    return true;
}