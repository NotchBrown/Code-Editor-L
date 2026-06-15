#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QFont>
#include "widget/main_window/main_window.h"
#include "util/logger.h"

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
    
    // Load default font from resource
    int fontId = QFontDatabase::addApplicationFont(":/font/SourceHanSansHWSC-Regular.otf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            QFont defaultFont(fontFamily, 10);
            app.setFont(defaultFont);
            qDebug() << "Default font loaded:" << fontFamily;
        }
    } else {
        qDebug() << "Failed to load default font from resource";
    }
    
    // Load bold font as well
    int boldFontId = QFontDatabase::addApplicationFont(":/font/SourceHanSansHWSC-Bold.otf");
    if (boldFontId != -1) {
        QStringList boldFontFamilies = QFontDatabase::applicationFontFamilies(boldFontId);
        if (!boldFontFamilies.isEmpty()) {
            qDebug() << "Bold font loaded:" << boldFontFamilies.at(0);
        }
    } else {
        qDebug() << "Failed to load bold font from resource";
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

    parser.process(app);

    MainWindow window;

    int port = parser.value(portOption).toInt();
    if (port > 0) {
        window.initIpcServer(port);
        qDebug() << "IPC server initialized on port" << port;
    }

    QString fileToOpen = parser.value(fileOption);
    if (!fileToOpen.isEmpty()) {
        qDebug() << "Opening file:" << fileToOpen;
    }

    if (parser.isSet(debugOption)) {
        qDebug() << "Debug mode enabled";
    }

    window.show();

    return app.exec();
}