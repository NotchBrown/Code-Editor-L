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
    
    // Load fonts from external resources first, fallback to embedded
    QString fontDir = QDir(appPath).filePath("resources/font/14_SourceHanSansHWSC/OTF/SimplifiedChineseHW");
    
    // Load regular font
    QString regularFontPath = QDir(fontDir).filePath("SourceHanSansHWSC-Regular.otf");
    if (!QFile::exists(regularFontPath)) {
        regularFontPath = ":/font/SourceHanSansHWSC-Regular.otf";
    }
    
    int fontId = QFontDatabase::addApplicationFont(regularFontPath);
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            QFont defaultFont(fontFamily, 10);
            app.setFont(defaultFont);
            qDebug() << "Default font loaded from:" << regularFontPath;
        }
    } else {
        qDebug() << "Failed to load default font from:" << regularFontPath;
        // Fallback to system font
        QFont defaultFont("Microsoft YaHei", 10);
        app.setFont(defaultFont);
        qDebug() << "Falling back to system font: Microsoft YaHei";
    }
    
    // Load bold font
    QString boldFontPath = QDir(fontDir).filePath("SourceHanSansHWSC-Bold.otf");
    if (!QFile::exists(boldFontPath)) {
        boldFontPath = ":/font/SourceHanSansHWSC-Bold.otf";
    }
    
    int boldFontId = QFontDatabase::addApplicationFont(boldFontPath);
    if (boldFontId != -1) {
        QStringList boldFontFamilies = QFontDatabase::applicationFontFamilies(boldFontId);
        if (!boldFontFamilies.isEmpty()) {
            qDebug() << "Bold font loaded from:" << boldFontPath;
        }
    } else {
        qDebug() << "Failed to load bold font from:" << boldFontPath;
    }

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

    // Positional argument: file path (standard Windows: drop file onto .exe)
    parser.addPositionalArgument("file", "File to open");

    parser.process(app);

    // Determine which file to open (positional arg takes priority over -f)
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

    if (parser.isSet(debugOption)) {
        qDebug() << "Debug mode enabled";
    }

    window.show();

    return app.exec();
}