
#include "MainWindow.h"

#include <Logger.h>

#include <QDialog>
#include <QFileDialog>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QObject>
#include <QScreen>
#include <QScrollArea>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_logTextProcessor(new LogTextProcessor(this)) {
    setMainWindowSize();
    createActions();
    createMenu();
    createCentralWidget();
    createStatusBar();

    connect(m_logTextProcessor, &LogTextProcessor::updateLevelCheckBoxes, this,
            &MainWindow::updateLevelCheckBoxes);
    connect(m_logTextProcessor, &LogTextProcessor::updateClassCheckBoxes, this,
            &MainWindow::updateClassCheckBoxes);
    connect(m_logTextProcessor, &LogTextProcessor::logTextProcessed, this,
            &MainWindow::updateLogText);

    m_logTextProcessorThread = new QThread(this);
    m_logTextProcessor->moveToThread(m_logTextProcessorThread);
    connect(m_logTextProcessorThread, &QThread::finished, m_logTextProcessor,
            &QObject::deleteLater);
    m_logTextProcessorThread->start();
}

MainWindow::~MainWindow() {
    m_logTextProcessorThread->quit();
    m_logTextProcessorThread->wait();
}

void MainWindow::setMainWindowSize() {
    Logger::debug("MainWindow creating");
    auto desktop = QGuiApplication::primaryScreen();
    auto screenRect = desktop->availableGeometry();
    auto width = screenRect.width() * 0.8;
    auto height = screenRect.height() * 0.8;
    resize(width, height);
    setMinimumSize(width * 0.5, height * 0.5);
    setMaximumSize(width, height);
    setWindowTitle("LogReader");
    move(screenRect.center() - rect().center());
    Logger::debug("MainWindow created");
}

void MainWindow::createMenu() {
    Logger::debug("Menu creating");
    auto fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_closeAction);
    menuBar()->addAction(m_helpAction);
    menuBar()->addAction(m_aboutAction);
    Logger::debug("Menu created");
}

QWidget *MainWindow::createSideBar() {
    Logger::debug("Side bar creating");
    auto sideBarWidget = new QWidget(this);
    auto sideBarLayout = new QHBoxLayout(sideBarWidget);
    sideBarLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    {
        m_levelCheckBoxLayout = new QVBoxLayout();
        m_levelCheckBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        auto levelChoiceTitle = new QLabel("Log Level", this);
        m_levelCheckBoxLayout->addWidget(levelChoiceTitle);
        sideBarLayout->addLayout(m_levelCheckBoxLayout);
    }
    Logger::debug("Level checkboxes created");

    {
        auto verticalLine = new QFrame(this);
        verticalLine->setFrameShape(QFrame::VLine);
        verticalLine->setFrameShadow(QFrame::Sunken);
        sideBarLayout->addWidget(verticalLine);
    }

    {
        auto classChoiceLayout = new QVBoxLayout();
        classChoiceLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        auto classChoiceTitle = new QLabel("Log Class", this);
        QScrollArea *scrollArea = new QScrollArea(this);
        {
            scrollArea->setWidgetResizable(true);
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            scrollArea->setFrameShape(QFrame::NoFrame);
            scrollArea->setContentsMargins(0, 0, 0, 0);

            auto scrollWidget = new QWidget(this);
            scrollArea->setWidget(scrollWidget);
            m_classCheckBoxLayout = new QVBoxLayout(scrollWidget);
            m_classCheckBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            scrollWidget->setLayout(m_classCheckBoxLayout);
        }
        classChoiceLayout->addWidget(classChoiceTitle);
        classChoiceLayout->addWidget(scrollArea);

        sideBarLayout->addLayout(classChoiceLayout);
    }
    Logger::debug("Class checkboxes created");
    return sideBarWidget;
}

QWidget *MainWindow::createLogView() {
    Logger::debug("Log view creating");
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

    Logger::debug("Log view created");
    return logViewWidget;
}

void MainWindow::createCentralWidget() {
    Logger::debug("Central widget creating");
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
    auto sidebarSize = (int)round(width() * 0.3);
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
    Logger::debug("Central widget created");
}

void MainWindow::createStatusBar() {
    Logger::debug("Status bar creating");
    Logger::debug("Status bar created");
}

void MainWindow::createActions() {
    Logger::debug("Actions creating");
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
    Logger::debug("Actions created");
}

void MainWindow::showHelpDialog() {
    Logger::debug("Open help dialog");
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
    Logger::debug("Open about dialog");
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

void MainWindow::openFile() {
    Logger::debug("File opening");
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), "",
                                                 tr("Log Files (*.log)"));
    if (fileName.isEmpty()) {
        return;
    }
    Logger::debug("File name: {}", fileName.toStdString());
    m_currentLog = new QFileInfo(fileName);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::error("Failed to open file: {}", fileName.toStdString());
        return;
    }
    QTextStream in(&file);
    m_logText->setPlainText(in.readAll());
    file.close();
    updateLogFileNameFromFile();
    emit m_logTextProcessor->logTextLoaded(m_logText->toHtml());
    Logger::debug("File opened");
}

void MainWindow::updateLogText(const QString &logTextHtml) {
    if (m_currentLog == nullptr) {  // no file opened
        return;
    }
    Logger::debug("Log view updating");
    m_logText->setHtml(logTextHtml);
    Logger::debug("Log view updated");
}

void MainWindow::closeFile() {
    Logger::debug("File closing");
    m_currentLog = nullptr;
    m_logText->clear();
    emit m_logTextProcessor->logTextLoaded("");
    Logger::debug("File closed");
}

void MainWindow::updateLevelCheckBoxes(
    const std::vector<QCheckBox *> &levelCheckBoxes) {
    for (auto checkBox : levelCheckBoxes) {
        m_levelCheckBoxLayout->addWidget(checkBox);
    }
}

void MainWindow::updateClassCheckBoxes(
    const std::vector<QCheckBox *> &classCheckBoxes) {
    for (auto checkBox : m_previousClassCheckBoxes) {
        m_classCheckBoxLayout->removeWidget(checkBox);
        delete checkBox;
    }
    m_previousClassCheckBoxes = classCheckBoxes;
    for (auto checkBox : classCheckBoxes) {
        m_classCheckBoxLayout->addWidget(checkBox);
    }
}

void MainWindow::updateLogFileNameFromFile() {
    Logger::trace("Update log file name");
    if (m_currentLog == nullptr) {
        m_logFileName->setText("No file opened.");
    } else {
        m_logFileName->setText(
            QString("File: %1").arg(m_currentLog->fileName()));
    }
    Logger::trace("Update log file name");
}
