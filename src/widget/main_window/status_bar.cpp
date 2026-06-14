#include "status_bar.h"
#include <QSpacerItem>
#include <QFrame>
#include <QPushButton>

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent),
      m_showingMessage(false),
      m_readOnly(false)
{
    // 创建消息超时定时器
    m_messageTimer = new QTimer(this);
    m_messageTimer->setSingleShot(true);
    connect(m_messageTimer, &QTimer::timeout, this, &StatusBar::onMessageTimeout);
    
    // 创建分隔符函数
    auto createSeparator = [this]() -> QFrame* {
        QFrame *separator = new QFrame(this);
        separator->setFrameShape(QFrame::VLine);
        separator->setFrameShadow(QFrame::Plain);
        separator->setLineWidth(1);
        separator->setStyleSheet("QFrame { color: #888; }");
        return separator;
    };
    
    // 创建右侧标签
    m_fileTypeLabel = new QLabel(this);
    m_fileTypeLabel->setAlignment(Qt::AlignCenter);
    m_fileTypeLabel->setMinimumWidth(80);
    
    m_encodingLabel = new QLabel(this);
    m_encodingLabel->setAlignment(Qt::AlignCenter);
    m_encodingLabel->setMinimumWidth(80);
    
    // 创建只读按钮
    m_readOnlyButton = new QPushButton(this);
    m_readOnlyButton->setCheckable(false);
    m_readOnlyButton->setMinimumWidth(80);
    m_readOnlyButton->setStyleSheet(
        "QPushButton { border: none; padding: 0 8px; }"
        "QPushButton:hover { background-color: rgba(0,0,0,0.1); }"
    );
    connect(m_readOnlyButton, &QPushButton::clicked, this, &StatusBar::onReadOnlyButtonClicked);
    
    m_cursorLabel = new QLabel(this);
    m_cursorLabel->setAlignment(Qt::AlignCenter);
    m_cursorLabel->setMinimumWidth(80);
    
    // 创建分隔符
    QFrame *sep1 = createSeparator();
    QFrame *sep2 = createSeparator();
    QFrame *sep3 = createSeparator();
    QFrame *sep4 = createSeparator();
    
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
    rightLayout->addWidget(sep1);
    rightLayout->addWidget(m_fileTypeLabel);
    rightLayout->addWidget(sep2);
    rightLayout->addWidget(m_encodingLabel);
    rightLayout->addWidget(sep3);
    rightLayout->addWidget(m_readOnlyButton);
    rightLayout->addWidget(sep4);
    rightLayout->addWidget(m_cursorLabel);
    rightWidget->setLayout(rightLayout);
    
    // 添加到状态栏
    addWidget(m_filePathLabel, 1);  // 左侧占剩余空间
    addPermanentWidget(rightWidget); // 右侧固定
    
    // 初始化默认值
    setFileType(tr("Plain Text"));
    setEncoding(tr("UTF-8"));
    setReadOnly(false);
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

void StatusBar::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
    if (readOnly) {
        m_readOnlyButton->setText(tr("Read Only"));
        m_readOnlyButton->setStyleSheet(
            "QPushButton { border: none; padding: 0 8px; color: red; }"
            "QPushButton:hover { background-color: rgba(0,0,0,0.1); }"
        );
    } else {
        m_readOnlyButton->setText(tr("Read Write"));
        m_readOnlyButton->setStyleSheet(
            "QPushButton { border: none; padding: 0 8px; color: green; }"
            "QPushButton:hover { background-color: rgba(0,0,0,0.1); }"
        );
    }
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

void StatusBar::onReadOnlyButtonClicked()
{
    m_readOnly = !m_readOnly;
    setReadOnly(m_readOnly);
    emit readOnlyToggled(m_readOnly);
}