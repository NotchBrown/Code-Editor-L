#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDir>

class ProjectManager : public QObject
{
    Q_OBJECT

public:
    explicit ProjectManager(QObject *parent = nullptr);
    ~ProjectManager();

    bool createProject(const QString &projectPath, const QString &projectName);
    bool openProject(const QString &projectFilePath);
    void closeProject();
    bool saveProject();
    
    bool addFile(const QString &filePath);
    bool removeFile(const QString &filePath);
    bool renameFile(const QString &oldPath, const QString &newPath);
    
    QString projectName() const;
    QString projectPath() const;
    QString projectFilePath() const;
    QStringList files() const;
    
    bool isProjectOpen() const;
    bool isFileInProject(const QString &filePath) const;

signals:
    void projectOpened(const QString &projectPath);
    void projectClosed();
    void fileAdded(const QString &filePath);
    void fileRemoved(const QString &filePath);
    void fileRenamed(const QString &oldPath, const QString &newPath);

private:
    bool readProjectFile(const QString &filePath);
    bool writeProjectFile(const QString &filePath);
    
    QString m_projectName;
    QString m_projectPath;
    QString m_projectFilePath;
    QStringList m_files;
    bool m_projectOpen;
};

#endif // PROJECT_MANAGER_H