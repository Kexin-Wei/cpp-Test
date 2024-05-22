#include <Logger.h>
#include <MainWindow.h>

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    logger::init_logger();

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("LogReader");
    MainWindow mainWindow;
    mainWindow.show();
    logger::info("Application started");
    return app.exec();
}