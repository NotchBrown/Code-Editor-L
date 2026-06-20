#include "widget/addons/addons_dialog.h"
#include "component/addon_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

AddonsDialog::AddonsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    populateTable();
}

AddonsDialog::~AddonsDialog() {}

void AddonsDialog::setupUi()
{
    setWindowTitle(tr("Add-Ons Manager"));
    setMinimumSize(750, 450);
    resize(850, 520);

    auto *mainLayout = new QVBoxLayout(this);

    // Title
    auto *titleLabel = new QLabel(tr("Add-Ons Manager"));
    QFont f = titleLabel->font();
    f.setPointSize(f.pointSize() + 4);
    f.setBold(true);
    titleLabel->setFont(f);
    mainLayout->addWidget(titleLabel);

    auto *desc = new QLabel(tr(
        "Add-ons extend the editor with advanced language analysis. "
        "Place addon folders in the addons/ directory, or import a .zip package."));
    desc->setWordWrap(true);
    mainLayout->addWidget(desc);
    mainLayout->addSpacing(8);

    // Table
    m_table = new QTableWidget(0, 6, this);
    m_table->setHorizontalHeaderLabels({
        tr("Name"), tr("Display Name"), tr("Publisher"),
        tr("Version"), tr("Status"), tr("Capabilities")
    });
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool hasSel = !selectedAddon().isEmpty();
        m_enableBtn->setEnabled(hasSel);
        m_exportBtn->setEnabled(hasSel);
        m_removeBtn->setEnabled(hasSel);
        // Update enable/disable button text
        QString name = selectedAddon();
        if (!name.isEmpty()) {
            bool en = AddonManager::instance()->isEnabled(name);
            m_enableBtn->setText(en ? tr("Disable") : tr("Enable"));
        }
    });
    mainLayout->addWidget(m_table);

    // Status
    m_statusLabel = new QLabel();
    mainLayout->addWidget(m_statusLabel);

    // Buttons row 1: actions on selected addon
    auto *actionLayout = new QHBoxLayout();
    m_enableBtn = new QPushButton(tr("Enable"));
    m_enableBtn->setEnabled(false);
    m_importBtn = new QPushButton(tr("Import..."));
    m_exportBtn = new QPushButton(tr("Export..."));
    m_exportBtn->setEnabled(false);
    m_removeBtn = new QPushButton(tr("Remove"));
    m_removeBtn->setEnabled(false);

    actionLayout->addWidget(m_enableBtn);
    actionLayout->addWidget(m_importBtn);
    actionLayout->addWidget(m_exportBtn);
    actionLayout->addWidget(m_removeBtn);
    mainLayout->addLayout(actionLayout);

    // Buttons row 2: general
    auto *btnLayout = new QHBoxLayout();
    m_reloadBtn = new QPushButton(tr("Reload"));
    m_openFolderBtn = new QPushButton(tr("Open Add-Ons Folder"));
    m_closeBtn = new QPushButton(tr("Close"));

    btnLayout->addWidget(m_reloadBtn);
    btnLayout->addWidget(m_openFolderBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_closeBtn);
    mainLayout->addLayout(btnLayout);

    // Connections
    connect(m_enableBtn, &QPushButton::clicked, this, &AddonsDialog::onEnableDisable);
    connect(m_importBtn, &QPushButton::clicked, this, &AddonsDialog::onImport);
    connect(m_exportBtn, &QPushButton::clicked, this, &AddonsDialog::onExport);
    connect(m_removeBtn, &QPushButton::clicked, this, &AddonsDialog::onRemove);
    connect(m_reloadBtn, &QPushButton::clicked, this, &AddonsDialog::onReload);
    connect(m_openFolderBtn, &QPushButton::clicked, this, &AddonsDialog::onOpenAddonsFolder);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void AddonsDialog::populateTable()
{
    m_table->setRowCount(0);
    auto *mgr = AddonManager::instance();
    QList<AddonDescriptor> all = mgr->allAddons();
    int loaded = 0, total = all.size();

    for (const AddonDescriptor &desc : all) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(desc.name));
        m_table->setItem(row, 1, new QTableWidgetItem(
            desc.displayName.isEmpty() ? desc.name : desc.displayName));
        m_table->setItem(row, 2, new QTableWidgetItem(desc.vendor));
        m_table->setItem(row, 3, new QTableWidgetItem(desc.version));

        bool en = mgr->isEnabled(desc.name);
        bool ok = mgr->component(desc.name) != nullptr;
        QString status;
        QColor color;
        if (!en) { status = tr("Disabled"); color = QColor(160, 160, 0); }
        else if (ok) { status = tr("Loaded"); color = QColor(0, 140, 0); loaded++; }
        else { status = tr("Failed"); color = QColor(200, 0, 0); }

        auto *stItem = new QTableWidgetItem(status);
        stItem->setForeground(color);
        m_table->setItem(row, 4, stItem);
        m_table->setItem(row, 5, new QTableWidgetItem(desc.capabilities.join(", ")));
    }

    m_statusLabel->setText(tr("Found %1 addon(s), %2 loaded").arg(total).arg(loaded));
}

QString AddonsDialog::selectedAddon() const
{
    int row = m_table->currentRow();
    if (row < 0) return QString();
    return m_table->item(row, 0)->text();
}

void AddonsDialog::onEnableDisable()
{
    QString name = selectedAddon();
    if (name.isEmpty()) return;
    bool en = AddonManager::instance()->isEnabled(name);
    AddonManager::instance()->setEnabled(name, !en);
    populateTable();
}

void AddonsDialog::onImport()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Import Add-On"), QString(),
                                                tr("Add-On Package (*.zip)"));
    if (path.isEmpty()) return;

    QString err = AddonManager::instance()->importAddon(path);
    if (err.isEmpty()) {
        QMessageBox::information(this, tr("Success"), tr("Add-On imported successfully."));
    } else {
        QMessageBox::warning(this, tr("Import Failed"), err);
    }
    populateTable();
}

void AddonsDialog::onExport()
{
    QString name = selectedAddon();
    if (name.isEmpty()) return;

    QString dir = QFileDialog::getExistingDirectory(this, tr("Export Add-On To"));
    if (dir.isEmpty()) return;

    QString result = AddonManager::instance()->exportAddon(name, dir);
    if (result.isEmpty()) {
        QMessageBox::warning(this, tr("Export Failed"), tr("Failed to create archive."));
    } else {
        QMessageBox::information(this, tr("Success"),
                                 tr("Add-On exported to:\n%1").arg(result));
    }
}

void AddonsDialog::onRemove()
{
    QString name = selectedAddon();
    if (name.isEmpty()) return;

    auto ret = QMessageBox::question(this, tr("Confirm Remove"),
        tr("Remove add-on '%1'?\nThis will delete the add-on files.").arg(name));
    if (ret != QMessageBox::Yes) return;

    if (AddonManager::instance()->removeAddon(name))
        populateTable();
}

void AddonsDialog::onReload()
{
    AddonManager::instance()->reload();
    populateTable();
}

void AddonsDialog::onOpenAddonsFolder()
{
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    QString path = appDir.absoluteFilePath("addons");
    QDir().mkpath(path);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
