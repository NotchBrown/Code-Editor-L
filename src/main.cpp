#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QStyleFactory>
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
    
    // Set Fusion style
    app.setStyle(QStyleFactory::create("Fusion"));

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