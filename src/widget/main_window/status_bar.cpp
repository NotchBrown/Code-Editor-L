#include "status_bar.h"
#include <QSpacerItem>

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent),
      m_showingMessage(false)
{
    // 创建消息超时定时器
    m_messageTimer = new QTimer(this);
    m_messageTimer->setSingleShot(true);
    connect(m_messageTimer, &QTimer::timeout, this, &StatusBar::onMessageTimeout);
    
    // 创建右侧标签
    m_fileTypeLabel = new QLabel(this);
    m_fileTypeLabel->setAlignment(Qt::AlignCenter);
    m_fileTypeLabel->setMinimumWidth(80);
    m_fileTypeLabel->setStyleSheet("QLabel { padding: 0 8px; border-right: 1px solid #ccc; }");
    
    m_encodingLabel = new QLabel(this);
    m_encodingLabel->setAlignment(Qt::AlignCenter);
    m_encodingLabel->setMinimumWidth(80);
    m_encodingLabel->setStyleSheet("QLabel { padding: 0 8px; border-right: 1px solid #ccc; }");
    
    m_cursorLabel = new QLabel(this);
    m_cursorLabel->setAlignment(Qt::AlignCenter);
    m_cursorLabel->setMinimumWidth(100);
    m_cursorLabel->setStyleSheet("QLabel { padding: 0 8px; }");
    
    // 创建左侧标签（文件路径/消息）
    m_filePathLabel = new QLabel(this);
    m_filePathLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_filePathLabel->setStyleSheet("QLabel { padding: 0 8px; }");
    m_filePathLabel->setText(tr("Ready"));
    
    // 创建右侧布局容器
    QWidget *rightWidget = new QWidget(this);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_fileTypeLabel);
    rightLayout->addWidget(m_encodingLabel);
    rightLayout->addWidget(m_cursorLabel);
    rightWidget->setLayout(rightLayout);
    
    // 添加到状态栏
    addWidget(m_filePathLabel, 1);  // 左侧占剩余空间
    addPermanentWidget(rightWidget); // 右侧固定
    
    // 初始化默认值
    setFileType("Plain Text");
    setEncoding("UTF-8");
    setCursorPosition(1, 1);
}

StatusBar::~StatusBar()
{
    if (m_messageTimer->isActive()) {
        m_messageTimer->stop();
    }
}

void StatusBar::setFilePath(const QString &path)
{
    m_currentFilePath = path;
    
    if (!m_showingMessage) {
        m_filePathLabel->setText(path);
    }
}

void StatusBar::setFileType(const QString &type)
{
    m_fileTypeLabel->setText(type);
}

void StatusBar::setEncoding(const QString &encoding)
{
    m_encodingLabel->setText(encoding);
}

void StatusBar::setCursorPosition(int line, int column)
{
    m_cursorLabel->setText(QString("%1,%2").arg(line).arg(column));
}

void StatusBar::showMessage(const QString &message, int duration)
{
    m_showingMessage = true;
    m_filePathLabel->setText(message);
    
    if (duration > 0) {
        m_messageTimer->start(duration);
    }
}

void StatusBar::clearMessage()
{
    if (m_messageTimer->isActive()) {
        m_messageTimer->stop();
    }
    
    m_showingMessage = false;
    m_filePathLabel->setText(m_currentFilePath.isEmpty() ? tr("Ready") : m_currentFilePath);
}

void StatusBar::onMessageTimeout()
{
    clearMessage();
}