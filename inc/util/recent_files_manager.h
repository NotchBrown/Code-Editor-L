#ifndef RECENT_FILES_MANAGER_H
#define RECENT_FILES_MANAGER_H

#include <QObject>
#include <QStringList>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class RecentFilesManager : public QObject
{
    Q_OBJECT

public:
    static RecentFilesManager* instance();

    QStringList recentFiles() const;
    void addFile(const QString &filePath);
    void removeFile(const QString &filePath);
    void clear();
    int maxFiles() const;
    void setMaxFiles(int max);

signals:
    void recentFilesChanged(const QStringList &files);

private:
    RecentFilesManager(QObject *parent = nullptr);
    ~RecentFilesManager();

    void loadRecentFiles();
    void saveRecentFiles();
    QString getConfigFilePath() const;

    QStringList m_recentFiles;
    int m_maxFiles;
    static RecentFilesManager* m_instance;
};

#endif // RECENT_FILES_MANAGER_H