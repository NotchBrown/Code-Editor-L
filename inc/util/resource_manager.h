#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <QString>
#include <QIcon>

class ResourceManager
{
public:
    static ResourceManager& instance();
    
    void init(const QString& appPath);
    
    QString iconPath(const QString& iconName) const;
    QString fontPath(const QString& fontName) const;
    
    QIcon loadIcon(const QString& iconName) const;
    
private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    QString m_resourcesPath;
};

#endif // RESOURCE_MANAGER_H