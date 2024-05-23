
#include "MainWindow.h"

#include <Logger.h>

#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
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
    createActions();
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
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_closeAction);
    menuBar()->addAction(m_helpAction);
    menuBar()->addAction(m_aboutAction);
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
    m_logFileName = new QLabel("No file opened.", this);
    m_logFileName->setFrameStyle(QFrame::Box | QFrame::Plain);
    logViewLayout->addWidget(m_logFileName);

    m_logText = new QTextEdit(this);
    m_logText->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    logViewLayout->addWidget(m_logText);
    m_logText->setReadOnly(true);
    m_logText->setLineWrapMode(QTextEdit::NoWrap);
    m_logText->setWordWrapMode(QTextOption::NoWrap);
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
    m_openAction = new QAction(tr("&Open"), this);
    m_openAction->setShortcuts(QKeySequence::Open);
    m_openAction->setStatusTip(tr("Open a file"));
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);

    m_closeAction = new QAction(tr("&Close"), this);
    m_closeAction->setShortcuts(QKeySequence::Close);
    m_closeAction->setStatusTip(tr("Close the file"));
    connect(m_closeAction, &QAction::triggered, this, &MainWindow::closeFile);

    m_helpAction = new QAction(tr("&Help"), this);
    m_helpAction->setShortcuts(QKeySequence::HelpContents);
    m_helpAction->setStatusTip(tr("Show the application's help"));
    connect(m_helpAction, &QAction::triggered, this,
            &MainWindow::showHelpDialog);

    m_aboutAction = new QAction(tr("&About"), this);
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    m_aboutAction->setShortcuts(QKeySequence::WhatsThis);
    connect(m_aboutAction, &QAction::triggered, this,
            &MainWindow::showAboutDialog);
}

void MainWindow::openFile() {
    Logger::info("Open file");
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), "",
                                                 tr("Log Files (*.log)"));
    if (fileName.isEmpty()) {
        return;
    }
    Logger::info("File name: {}", fileName.toStdString());
    m_currentLog = new QFileInfo(fileName);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::error("Failed to open file: {}", fileName.toStdString());
        return;
    }
    QTextStream in(&file);
    m_logText->setPlainText(in.readAll());
    file.close();
    updateLogFileName();
}

void MainWindow::updateLogFileName() {
    if (m_currentLog == nullptr) {
        m_logFileName->setText("No file opened.");
    } else {
        m_logFileName->setText(
            QString("File: %1").arg(m_currentLog->fileName()));
    }
}

void MainWindow::closeFile() {
    Logger::info("Close file");
    m_logText->clear();
    m_currentLog = nullptr;
    updateLogFileName();
}

void MainWindow::showHelpDialog() {
    Logger::info("Open help dialog");
    auto helpDialog = new QDialog(this);
    auto helpLayout = new QVBoxLayout();
    auto helpText = new QTextEdit(this);
    helpText->setReadOnly(true);
    helpText->setLineWrapMode(QTextEdit::NoWrap);
    helpText->setWordWrapMode(QTextOption::NoWrap);
    helpText->setText(getHelpText());
    helpLayout->addWidget(helpText);
    helpDialog->setLayout(helpLayout);
    helpDialog->setWindowFlags(Qt::WindowType::Popup);
    helpDialog->setWindowTitle("Help");

    helpDialog->setGeometry(0, 0, width() * 0.7, height() * 0.7);
    helpDialog->move(geometry().center() - helpDialog->geometry().center());
    helpDialog->exec();
}

const QString MainWindow::getHelpText() {
    auto textTitle = QString("<h1>%1</h1>").arg("Feature");
    auto openFileSubtitle = QString("<h2>%1</h2>").arg("Open file");
    auto openFileList = std::vector<QString>{"Open a file", "Close a file"};
    auto openFileItems = QString("<ul>");
    for (const auto &item : openFileList) {
        openFileItems.append(QString("<li>%1</li>").arg(item));
    }
    openFileItems.append("</ul>");

    auto filterSubtitle = QString("<h2>%1</h2>").arg("Filter");
    auto filterList =
        std::vector<QString>{"Filter by log level", "Filter by log class"};
    auto filterItems = QString("<ul>");
    for (const auto &item : filterList) {
        filterItems.append(QString("<li>%1</li>").arg(item));
    }
    filterItems.append("</ul>");
    auto helpSubtitle = QString("<h2>%1</h2>").arg("Help");
    auto aboutSubtitle = QString("<h2>%1</h2>").arg("About");
    return textTitle + openFileSubtitle + openFileItems + filterSubtitle +
           filterItems + helpSubtitle + aboutSubtitle;
}

void MainWindow::showAboutDialog() {
    Logger::info("Open about dialog");
    auto aboutDialog = new QDialog(this);
    auto aboutLayout = new QVBoxLayout();
    auto styleSheet = QString("QLabel { font-size: 20px; }");
    auto showTextList =
        std::vector<QString>{"Author: Kexin Wei", "Version: 1.0"};
    for (const auto &text : showTextList) {
        auto label = new QLabel(text, this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(styleSheet);
        aboutLayout->addWidget(label);
    }
    aboutDialog->setLayout(aboutLayout);
    aboutDialog->setWindowFlags(Qt::WindowType::Popup);
    aboutDialog->setWindowTitle("About");
    aboutDialog->exec();
}