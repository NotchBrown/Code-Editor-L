#include "main.h"
#include "widget/properties/properties.h"
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>

Properties::Properties(QWidget *parent)
    : QWidget(parent),
      m_propertiesTable(nullptr),
      m_filePath("")
{
    setupUI();
    setupConnections();
}

Properties::~Properties()
{
}

void Properties::setupUI()
{
    ui = new Ui::Properties();
    ui->setupUi(this);
    
    m_propertiesTable = ui->propertiesTable;
    
    // Setup table properties
    m_propertiesTable->setColumnWidth(0, 150);
    m_propertiesTable->setColumnWidth(1, 300);
    m_propertiesTable->setColumnWidth(2, 80);
    m_propertiesTable->horizontalHeader()->setStretchLastSection(true);
    m_propertiesTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    m_propertiesTable->setAlternatingRowColors(true);
    
    // Set table headers
    QStringList headers;
    headers << "Property" << "Value" << "Editable";
    m_propertiesTable->setHorizontalHeaderLabels(headers);
}

void Properties::setupConnections()
{
    connect(m_propertiesTable, &QTableWidget::cellChanged, this, &Properties::onCellChanged);
    connect(ui->refreshButton, &QPushButton::clicked, this, &Properties::onRefreshButtonClicked);
    connect(ui->applyButton, &QPushButton::clicked, this, &Properties::onApplyButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
}

void Properties::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
    m_fileInfo = QFileInfo(filePath);
    
    ui->filePathLabel->setText("File Path: " + filePath);
    refreshProperties();
}

QString Properties::filePath() const
{
    return m_filePath;
}

void Properties::refreshProperties()
{
    loadProperties();
}

void Properties::loadProperties()
{
    if (m_filePath.isEmpty() || !m_fileInfo.exists()) {
        m_propertiesTable->setRowCount(0);
        return;
    }
    
    m_propertiesTable->setRowCount(0);
    m_propertiesTable->blockSignals(true);
    
    // Read-only properties
    addProperty("File Name", m_fileInfo.fileName(), false);
    addProperty("File Path", m_fileInfo.absoluteFilePath(), false);
    addProperty("File Size", formatFileSize(m_fileInfo.size()), false);
    addProperty("File Type", m_fileInfo.suffix().toUpper() + " File", false);
    addProperty("Created", m_fileInfo.created().toString("yyyy-MM-dd hh:mm:ss"), false);
    addProperty("Last Modified", m_fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"), false);
    addProperty("Last Read", m_fileInfo.lastRead().toString("yyyy-MM-dd hh:mm:ss"), false);
    addProperty("Owner", m_fileInfo.owner(), false);
    addProperty("Group", m_fileInfo.group(), false);
    addProperty("Permissions", formatPermissions(m_fileInfo.permissions()), false);
    addProperty("Absolute Path", m_fileInfo.absolutePath(), false);
    addProperty("Canonical Path", m_fileInfo.canonicalFilePath(), false);
    addProperty("Is Hidden", m_fileInfo.isHidden() ? "Yes" : "No", false);
    addProperty("Is Readable", m_fileInfo.isReadable() ? "Yes" : "No", false);
    addProperty("Is Writable", m_fileInfo.isWritable() ? "Yes" : "No", false);
    addProperty("Is Executable", m_fileInfo.isExecutable() ? "Yes" : "No", false);
    addProperty("Is SymLink", m_fileInfo.isSymLink() ? "Yes" : "No", false);
    
    // Editable properties
    addProperty("New File Name", m_fileInfo.fileName(), true);
    
    m_propertiesTable->blockSignals(false);
}

void Properties::addProperty(const QString &name, const QString &value, bool editable)
{
    int row = m_propertiesTable->rowCount();
    m_propertiesTable->insertRow(row);
    
    QTableWidgetItem *nameItem = new QTableWidgetItem(name);
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    nameItem->setData(PropertyName, name);
    
    QTableWidgetItem *valueItem = new QTableWidgetItem(value);
    if (!editable) {
        valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
    }
    valueItem->setData(PropertyValue, value);
    valueItem->setData(PropertyEditable, editable);
    
    QTableWidgetItem *editableItem = new QTableWidgetItem(editable ? "Yes" : "No");
    editableItem->setFlags(editableItem->flags() & ~Qt::ItemIsEditable);
    editableItem->setTextAlignment(Qt::AlignCenter);
    
    m_propertiesTable->setItem(row, 0, nameItem);
    m_propertiesTable->setItem(row, 1, valueItem);
    m_propertiesTable->setItem(row, 2, editableItem);
}

bool Properties::isPropertyEditable(int row) const
{
    if (row < 0 || row >= m_propertiesTable->rowCount()) {
        return false;
    }
    
    QTableWidgetItem *item = m_propertiesTable->item(row, 2);
    return item && item->text() == "Yes";
}

void Properties::onCellChanged(int row, int column)
{
    if (column != 1 || !isPropertyEditable(row)) {
        return;
    }
    
    // Highlight changed rows
    QTableWidgetItem *nameItem = m_propertiesTable->item(row, 0);
    if (nameItem) {
        nameItem->setBackground(QColor(255, 255, 200));
    }
    
    QTableWidgetItem *valueItem = m_propertiesTable->item(row, 1);
    if (valueItem) {
        valueItem->setBackground(QColor(255, 255, 200));
    }
}

void Properties::onApplyButtonClicked()
{
    if (m_filePath.isEmpty() || !m_fileInfo.exists()) {
        QMessageBox::warning(this, "Error", "No valid file selected.");
        return;
    }
    
    bool hasChanges = false;
    QString newFileName;
    
    for (int row = 0; row < m_propertiesTable->rowCount(); ++row) {
        if (!isPropertyEditable(row)) {
            continue;
        }
        
        QTableWidgetItem *nameItem = m_propertiesTable->item(row, 0);
        QTableWidgetItem *valueItem = m_propertiesTable->item(row, 1);
        
        if (!nameItem || !valueItem) {
            continue;
        }
        
        QString propertyName = nameItem->data(PropertyName).toString();
        QString newValue = valueItem->text();
        QString oldValue = valueItem->data(PropertyValue).toString();
        
        if (newValue != oldValue) {
            hasChanges = true;
            
            if (propertyName == "New File Name") {
                newFileName = newValue;
            }
        }
    }
    
    if (!hasChanges) {
        QMessageBox::information(this, "Info", "No changes to apply.");
        return;
    }
    
    // Apply file name change
    if (!newFileName.isEmpty()) {
        QDir dir = m_fileInfo.absoluteDir();
        QString newPath = dir.filePath(newFileName);
        
        if (QFile::rename(m_filePath, newPath)) {
            m_filePath = newPath;
            m_fileInfo = QFileInfo(newPath);
            ui->filePathLabel->setText("File Path: " + newPath);
            QMessageBox::information(this, "Success", "File renamed successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to rename file.");
            return;
        }
    }
    
    // Reload properties
    loadProperties();
}

void Properties::onRefreshButtonClicked()
{
    if (!m_filePath.isEmpty()) {
        m_fileInfo = QFileInfo(m_filePath);
        loadProperties();
    }
}

QString Properties::formatFileSize(qint64 bytes) const
{
    const qint64 KB = 1024;
    const qint64 MB = 1024 * KB;
    const qint64 GB = 1024 * MB;
    
    if (bytes >= GB) {
        return QString::number(bytes / (double)GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(bytes / (double)MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(bytes / (double)KB, 'f', 2) + " KB";
    } else {
        return QString::number(bytes) + " bytes";
    }
}

QString Properties::formatPermissions(QFile::Permissions permissions) const
{
    QString result;
    
    // Owner permissions
    result += (permissions & QFile::ReadOwner) ? "r" : "-";
    result += (permissions & QFile::WriteOwner) ? "w" : "-";
    result += (permissions & QFile::ExeOwner) ? "x" : "-";
    
    // Group permissions
    result += (permissions & QFile::ReadGroup) ? "r" : "-";
    result += (permissions & QFile::WriteGroup) ? "w" : "-";
    result += (permissions & QFile::ExeGroup) ? "x" : "-";
    
    // Other permissions
    result += (permissions & QFile::ReadOther) ? "r" : "-";
    result += (permissions & QFile::WriteOther) ? "w" : "-";
    result += (permissions & QFile::ExeOther) ? "x" : "-";
    
    return result;
}