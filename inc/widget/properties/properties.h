#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "main.h"
#include "ui_properties.h"

class Properties : public QWidget
{
    Q_OBJECT

public:
    explicit Properties(QWidget *parent = nullptr);
    ~Properties();

    void setFilePath(const QString &filePath);
    QString filePath() const;

    void refreshProperties();

private:
    void setupUI();
    void setupConnections();
    void loadProperties();
    void saveProperties();
    void addProperty(const QString &name, const QString &value, bool editable);
    bool isPropertyEditable(int row) const;
    QString formatFileSize(qint64 bytes) const;
    QString formatPermissions(QFile::Permissions permissions) const;

    enum PropertyRole {
        PropertyName = Qt::UserRole,
        PropertyValue = Qt::UserRole + 1,
        PropertyEditable = Qt::UserRole + 2
    };

    Ui::Properties *ui;
    QTableWidget *m_propertiesTable;
    QString m_filePath;
    QFileInfo m_fileInfo;

private slots:
    void onCellChanged(int row, int column);
    void onApplyButtonClicked();
    void onRefreshButtonClicked();
};

#endif // PROPERTIES_H