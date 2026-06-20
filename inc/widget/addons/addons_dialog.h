#ifndef ADDONS_DIALOG_H
#define ADDONS_DIALOG_H

#include <QDialog>
#include "component/addon_info.h"

class QTableWidget;
class QPushButton;
class QLabel;
class QPlainTextEdit;

class AddonsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddonsDialog(QWidget *parent = nullptr);
    ~AddonsDialog() override;

private slots:
    void onEnableDisable();
    void onImport();
    void onExport();
    void onRemove();
    void onReload();
    void onOpenAddonsFolder();

private:
    void setupUi();
    void populateTable();
    void showMessage(const QString &msg, bool isError = false);
    QString selectedAddon() const;

    QTableWidget    *m_table;
    QPushButton     *m_enableBtn;
    QPushButton     *m_importBtn;
    QPushButton     *m_exportBtn;
    QPushButton     *m_removeBtn;
    QPushButton     *m_reloadBtn;
    QPushButton     *m_openFolderBtn;
    QPushButton     *m_closeBtn;
    QLabel          *m_statusLabel;
};

#endif // ADDONS_DIALOG_H
