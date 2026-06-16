#include "util/resource_manager.h"
#include <QDir>
#include <QDebug>

ResourceManager& ResourceManager::instance()
{
    static ResourceManager instance;
    return instance;
}

void ResourceManager::init(const QString& appPath)
{
    QDir appDir(appPath);
    m_resourcesPath = appDir.filePath("resources");
    
    // Create resources directory if not exists
    QDir resourcesDir(m_resourcesPath);
    if (!resourcesDir.exists()) {
        resourcesDir.mkpath(".");
        qDebug() << "Created resources directory:" << m_resourcesPath;
    }
}

QString ResourceManager::iconPath(const QString& iconName) const
{
    return QDir(m_resourcesPath).filePath("icon/" + iconName);
}

QString ResourceManager::fontPath(const QString& fontName) const
{
    return QDir(m_resourcesPath).filePath("font/" + fontName);
}

QIcon ResourceManager::loadIcon(const QString& iconName) const
{
    QString path = iconPath(iconName);
    if (QFile::exists(path)) {
        return QIcon(path);
    }
    
    // Fallback to embedded resource
    QString resourcePath = ":/icon/" + iconName;
    return QIcon(resourcePath);
}