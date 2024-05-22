
#include "MainWindow.h"

#include <spdlog/spdlog.h>

#include <QButtonGroup>
#include <QCheckBox>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QScreen>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setMainWindowSize();
    createMenu();
    createCentralWidget();
    createStatusBar();
}

MainWindow::~MainWindow() {}

void MainWindow::setMainWindowSize() {
    auto desktop = QGuiApplication::primaryScreen();
    auto screenRect = desktop->availableGeometry();
    auto width = screenRect.width() * 0.8;
    auto height = screenRect.height() * 0.8;
    resize(width, height);
    setMinimumSize(width * 0.5, height * 0.5);
    setMaximumSize(width, height);
    setWindowTitle("LogReader");
    move(screenRect.center() - rect().center());
}

void MainWindow::createMenu() {
    auto fileMenu = menuBar()->addMenu(tr("&File"));
    auto helpMenu = menuBar()->addMenu(tr("&Help"));
    auto aboutMenu = menuBar()->addMenu(tr("&About"));
}

QWidget *MainWindow::createSideBar() {
    auto sideBarWidget = new QWidget(this);
    auto sideBarLayout = new QVBoxLayout(sideBarWidget);
    sideBarLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    auto levelLayout = new QVBoxLayout();
    auto levelChoiceTitle = new QLabel("Log Level", this);
    levelLayout->addWidget(levelChoiceTitle);
    auto levelChoiceGroup = new QButtonGroup(this);
    levelChoiceGroup->setExclusive(false);
    auto allLevelButton = new QCheckBox("All", this);
    levelChoiceGroup->addButton(allLevelButton);
    levelLayout->addWidget(allLevelButton);
    auto levelList = std::vector<std::string>{"Trace",   "Debug", "Info",
                                              "Warning", "Error", "Critical"};
    for (const auto &level : levelList) {
        auto button = new QCheckBox(QString::fromStdString(level), this);
        levelChoiceGroup->addButton(button);
        levelLayout->addWidget(button);
    }
    sideBarLayout->addLayout(levelLayout);

    auto horizontalLine = new QFrame(this);
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Sunken);
    sideBarLayout->addWidget(horizontalLine);

    auto classChoiceLayout = new QVBoxLayout();
    auto classChoiceTitle = new QLabel("Log Class", this);
    classChoiceLayout->addWidget(classChoiceTitle);
    auto classChoiceGroup = new QButtonGroup(this);
    classChoiceGroup->setExclusive(false);
    auto allClassButton = new QCheckBox("All", this);
    classChoiceGroup->addButton(allClassButton);
    classChoiceLayout->addWidget(allClassButton);
    auto classList = std::vector<std::string>{"Class 1", "Class 2", "Class 3"};
    for (const auto &className : classList) {
        auto button = new QCheckBox(QString::fromStdString(className), this);
        classChoiceGroup->addButton(button);
        classChoiceLayout->addWidget(button);
    }
    sideBarLayout->addLayout(classChoiceLayout);
    return sideBarWidget;
}

QWidget *MainWindow::createLogView() {
    auto logViewWidget = new QWidget(this);
    auto logViewLayout = new QVBoxLayout(logViewWidget);
    auto logFileName = new QLabel("File: log123.log", this);
    logFileName->setFrameStyle(QFrame::Box | QFrame::Plain);
    logViewLayout->addWidget(logFileName);
    auto logText = new QTextEdit(this);
    logText->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    logViewLayout->addWidget(logText);
    logText->setReadOnly(true);
    logText->setLineWrapMode(QTextEdit::NoWrap);
    logText->setWordWrapMode(QTextOption::NoWrap);
    return logViewWidget;
}

void MainWindow::createCentralWidget() {
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    splitter->setChildrenCollapsible(false);
    splitter->setOpaqueResize(true);
    auto sideBarWidget = createSideBar();
    auto logViewWidget = createLogView();
    splitter->addWidget(sideBarWidget);
    splitter->addWidget(logViewWidget);
    auto sidebarSize = (int)round(width() * 0.1);
    splitter->setSizes({sidebarSize, width() - sidebarSize});
    auto splitterHandle = splitter->handle(1);
    auto splitterHandleLayout = new QVBoxLayout(splitterHandle);
    splitterHandleLayout->setAlignment(Qt::AlignCenter);
    splitterHandleLayout->setContentsMargins(0, 0, 0, 0);
    splitterHandleLayout->setSpacing(0);
    auto splitterLine = new QFrame(splitterHandle);
    splitterLine->setFrameShape(QFrame::HLine);
    splitterLine->setFrameShadow(QFrame::Sunken);
    splitterHandleLayout->addWidget(splitterLine);

    auto mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);
}

void MainWindow::createStatusBar() {}

void MainWindow::createActions() {
    auto openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a file"));
}

void MainWindow::openFile() {}

void MainWindow::closeFile() {}