#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QFont>
#include <QDir>
#include <QIcon>
#include <QTimer>
#include "widget/main_window/main_window.h"
#include "util/logger.h"
#include "util/resource_manager.h"
#include "util/font_manager.h"
#include "util/settings_manager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Code Editor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("WaveIn");

    // Initialize logger
    Logger::init();

    // Set Fusion style first to ensure font is applied correctly
    app.setStyle(QStyleFactory::create("Fusion"));

    // Get application path
    QString appPath = QCoreApplication::applicationDirPath();

    // Initialize resource manager with external resources
    ResourceManager::instance().init(appPath);

    // ── Font Setup (必须在 FontManager 之前加载，使其进入 QFontDatabase) ──
    QString fontDir = QDir(appPath).filePath("../resource/font/14_SourceHanSansHWSC/OTF/SimplifiedChineseHW");
    if (!QDir(fontDir).exists()) {
        fontDir = QDir(appPath).filePath("resources/font/14_SourceHanSansHWSC/OTF/SimplifiedChineseHW");
    }

    // Regular
    QString regularFontPath = QDir(fontDir).filePath("SourceHanSansHWSC-Regular.otf");
    if (!QFile::exists(regularFontPath))
        regularFontPath = ":/font/SourceHanSansHWSC-Regular.otf";
    int fontId = QFontDatabase::addApplicationFont(regularFontPath);
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty())
            qDebug() << "Default font loaded from:" << regularFontPath;
    } else {
        qDebug() << "Failed to load default font from:" << regularFontPath;
    }

    // Bold
    QString boldFontPath = QDir(fontDir).filePath("SourceHanSansHWSC-Bold.otf");
    if (!QFile::exists(boldFontPath))
        boldFontPath = ":/font/SourceHanSansHWSC-Bold.otf";
    int boldFontId = QFontDatabase::addApplicationFont(boldFontPath);
    if (boldFontId != -1) {
        qDebug() << "Bold font loaded from:" << boldFontPath;
    } else {
        qDebug() << "Failed to load bold font from:" << boldFontPath;
    }

    // 现在初始化 FontManager（QFontDatabase 中已有字体，能正确识别）
    FontManager::instance().init(appPath);

    // Initialize SettingsManager
    SettingsManager::instance().init(appPath);

    // Apply saved app font, or fallback
    QFont savedFont = SettingsManager::instance().appFont();
    int savedSize = SettingsManager::instance().appFontSize();
    if (savedSize > 0) {
        savedFont.setPointSize(savedSize);
        app.setFont(savedFont);
        qDebug() << "Applied saved app font:" << savedFont.family() << savedSize;
    } else {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId >= 0 ? fontId : 0);
        QString fallbackFamily = families.isEmpty() ? "Microsoft YaHei" : families.first();
        QFont defaultFont(fallbackFamily, 10);
        app.setFont(defaultFont);
        qDebug() << "Using fallback font:" << fallbackFamily;
    }

    // ── Command Line ──
    QCommandLineParser parser;
    parser.setApplicationDescription("A lightweight code editor with IPC support");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption portOption("mp", "Management port for IPC communication", "port");
    parser.addOption(portOption);

    QCommandLineOption fileOption("f", "File to open on startup", "file");
    parser.addOption(fileOption);

    QCommandLineOption debugOption("debug", "Enable debug mode");
    parser.addOption(debugOption);

    parser.addPositionalArgument("file", "File to open");

    parser.process(app);

    QString fileToOpen;
    if (!parser.positionalArguments().isEmpty()) {
        fileToOpen = parser.positionalArguments().first();
    } else if (parser.isSet(fileOption)) {
        fileToOpen = parser.value(fileOption);
    }

    MainWindow window;

    int port = parser.value(portOption).toInt();
    if (port > 0) {
        window.initIpcServer(port);
        qDebug() << "IPC server initialized on port" << port;
    }

    if (!fileToOpen.isEmpty()) {
        qDebug() << "Opening file:" << fileToOpen;
        QTimer::singleShot(200, [&]() {
            window.createNewEditor(fileToOpen);
        });
    }

    if (parser.isSet(debugOption))
        qDebug() << "Debug mode enabled";

    window.show();
    return app.exec();
}