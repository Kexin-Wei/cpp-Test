
#include "MainWindow.h"

#include <spdlog/spdlog.h>

#include <QFrame>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createCentralWidget();
    createMenu();
    createStatusBar();
}

MainWindow::~MainWindow() {}

void MainWindow::createMenu() {
    auto fileMenu = menuBar()->addMenu(tr("&File"));
    auto helpMenu = menuBar()->addMenu(tr("&Help"));
    auto aboutMenu = menuBar()->addMenu(tr("&About"));
}

void MainWindow::createSideBar() {}

void MainWindow::createCentralWidget() {
    QFrame *centralWidget = new QFrame(this);
    setCentralWidget(centralWidget);
    createSideBar();
}

void MainWindow::createStatusBar() {}

void MainWindow::createActions() {
    auto openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a file"));
}

void MainWindow::openFile() {}

void MainWindow::closeFile() {}