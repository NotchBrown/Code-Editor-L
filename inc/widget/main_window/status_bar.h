#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include <QFrame>

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar();

    // 设置文件位置（自动显示在左侧）
    void setFilePath(const QString &path);
    
    // 设置文件类型
    void setFileType(const QString &type);
    
    // 设置编码
    void setEncoding(const QString &encoding);
    
    // 设置光标位置
    void setCursorPosition(int line, int column);
    
    // 设置临时消息（会覆盖文件位置显示）
    void showMessage(const QString &message, int duration = 0);
    
    // 清除临时消息，恢复显示文件位置
    void clearMessage();

private slots:
    void onMessageTimeout();

private:
    QLabel *m_filePathLabel;      // 左侧：文件路径或消息
    QLabel *m_fileTypeLabel;      // 右侧：文件类型
    QLabel *m_encodingLabel;      // 右侧：编码
    QLabel *m_cursorLabel;        // 右侧：光标位置
    
    QString m_currentFilePath;    // 当前文件路径（用于恢复显示）
    QTimer *m_messageTimer;       // 消息超时定时器
    bool m_showingMessage;        // 是否正在显示消息
};

#endif // STATUS_BAR_H