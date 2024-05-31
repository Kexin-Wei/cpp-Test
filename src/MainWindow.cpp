
#include "MainWindow.h"

#include <Logger.h>

#include <QDialog>
#include <QFileDialog>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QRegularExpression>
#include <QScreen>
#include <QScrollArea>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_classesFromLog({}),
      m_classes({}),
      m_levelsFromLog({}),
      m_levels({{LogLevel::TRACE, "Trace"},
                {LogLevel::DEBUG, "Debug"},
                {LogLevel::INFO, "Info"},
                {LogLevel::WARNING, "Warning"},
                {LogLevel::ERROR, "Error"},
                {LogLevel::CRITICAL, "Critical"}}),
      m_levelColors({{LogLevel::TRACE, Qt::gray},
                     {LogLevel::DEBUG, Qt::blue},
                     {LogLevel::INFO, Qt::green},
                     {LogLevel::WARNING, Qt::darkYellow},
                     {LogLevel::ERROR, Qt::red},
                     {LogLevel::CRITICAL, Qt::darkRed}}) {
    for (auto i = 0; i < 20; i++)
        m_classesFromLog.insert(QString("Class%1").arg(i));
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
    Logger::debug("MainWindow created");
}

void MainWindow::createMenu() {
    auto fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_closeAction);
    menuBar()->addAction(m_helpAction);
    menuBar()->addAction(m_aboutAction);
    Logger::debug("Menu created");
}

void MainWindow::updateLevelFilter() {
    for (const auto &[levelEnum, levelString] : m_levels) {
        auto checkBox = m_levelCheckBoxes[levelEnum];
        if (auto search = m_levelsFromLog.find(levelString.toLower());
            search == m_levelsFromLog.end()) {
            checkBox->setEnabled(true);
            Logger::trace("Level checkbox enabled: {}",
                          levelString.toStdString());
        } else {
            checkBox->setEnabled(false);
            Logger::trace("Level checkbox disabled: {}",
                          levelString.toStdString());
        }
    }
    Logger::trace("Level checkboxes updated");
}

void MainWindow::updateClassFilter() {
    // remove all existing class checkboxes
    for (const auto &className : m_classes) {
        auto button = m_classCheckBoxes[className];
        m_classChoiceGroup->removeButton(button);
        m_classCheckBoxLayout->removeWidget(button);
        delete button;
    }
    m_classCheckBoxes.clear();
    m_classes.clear();
    Logger::trace("Class checkboxes removed");

    m_classes = m_classesFromLog;
    for (const auto &className : m_classes) {
        auto button = new QCheckBox(className, this);
        m_classChoiceGroup->addButton(button);
        m_classCheckBoxLayout->addWidget(button);
        m_classCheckBoxes[className] = button;
        Logger::trace("Class checkbox created: {}", className.toStdString());
    }
    Logger::trace("Class checkboxes updated");
}

QWidget *MainWindow::createSideBar() {
    auto sideBarWidget = new QWidget(this);
    auto sideBarLayout = new QHBoxLayout(sideBarWidget);
    sideBarLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_levelChoiceGroup = new QButtonGroup(this);
    m_levelChoiceGroup->setExclusive(false);
    {
        m_levelCheckBoxLayout = new QVBoxLayout();
        m_levelCheckBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        auto levelChoiceTitle = new QLabel("Log Level", this);
        auto allLevelButton = new QCheckBox("All", this);
        m_levelCheckBoxLayout->addWidget(levelChoiceTitle);
        m_levelCheckBoxLayout->addWidget(allLevelButton);

        {
            m_levelChoiceGroup->addButton(allLevelButton);
            for (const auto &[levelEnum, levelString] : m_levels) {
                auto button = new QCheckBox(levelString, this);
                m_levelChoiceGroup->addButton(button);
                m_levelCheckBoxLayout->addWidget(button);
                button->setDisabled(true);
                m_levelCheckBoxes[levelEnum] = button;
            }
        }
        sideBarLayout->addLayout(m_levelCheckBoxLayout);
    }

    updateLevelFilter();
    Logger::debug("Level checkboxes created");

    {
        auto verticalLine = new QFrame(this);
        verticalLine->setFrameShape(QFrame::VLine);
        verticalLine->setFrameShadow(QFrame::Sunken);
        sideBarLayout->addWidget(verticalLine);
    }

    m_classChoiceGroup = new QButtonGroup(this);
    m_classChoiceGroup->setExclusive(false);

    {
        auto classChoiceLayout = new QVBoxLayout();
        classChoiceLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        auto classChoiceTitle = new QLabel("Log Class", this);
        auto allClassButton = new QCheckBox("All", this);
        m_classChoiceGroup->addButton(allClassButton);

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
        classChoiceLayout->addWidget(allClassButton);  // not scrollable
        classChoiceLayout->addWidget(scrollArea);

        sideBarLayout->addLayout(classChoiceLayout);
    }

    Logger::debug("Scroll area created");
    updateClassFilter();
    Logger::debug("Class checkboxes created");
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

    Logger::debug("Log view created");
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

void MainWindow::createStatusBar() { Logger::debug("Status bar created"); }

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
    Logger::debug("Actions created");
}

void MainWindow::openFile() {
    Logger::debug("Open file");
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
    m_logTextContent = in.readAll();
    m_logText->setPlainText(m_logTextContent);
    file.close();
    updateLogFileName();
    showLevelInDifferentColor();
    updateSideBarFilters();
}

void MainWindow::showLevelInDifferentColor() {
    for (auto &[levelEnum, levelString] : m_levels) {
        auto levelShown = levelString.toLower();
        auto color = m_levelColors[levelEnum].name();
        auto format = QString("<span style=\"color: %1;\">%2</span>")
                          .arg(color)
                          .arg(levelShown);
        m_logText->moveCursor(QTextCursor::Start);
        m_logText->find(levelShown);  // TODO: not working
        m_logText->insertHtml(format);
    }
    Logger::debug("Show level in different color");
}

void MainWindow::updateLogFileName() {
    if (m_currentLog == nullptr) {
        m_logFileName->setText("No file opened.");
    } else {
        m_logFileName->setText(
            QString("File: %1").arg(m_currentLog->fileName()));
    }
    m_levelsFromLog.clear();
    m_classesFromLog.clear();
    updateSideBarFilters();
}

void MainWindow::updateLogView() { showLevelInDifferentColor(); }

void MainWindow::updateSideBarFilters() {
    QRegularExpression levelClassRegex(m_levelReg + m_classReg);
    m_levelsFromLog.clear();
    m_classesFromLog.clear();

    auto text = m_logText->toPlainText();
    auto matches = levelClassRegex.globalMatch(text);
    while (matches.hasNext()) {
        auto match = matches.next();
        auto level = match.captured(1);
        auto className = match.captured(2);
        m_levelsFromLog.insert(capitalize(level.toLower()));
        m_classesFromLog.insert(className.toLower());
        Logger::debug("Level: {}, Class: {}", level.toStdString(),
                      className.toStdString());
    }
    updateLevelFilter();
    updateClassFilter();
    updateLogView();
    Logger::debug("Update sidebar filters");
}

void MainWindow::closeFile() {
    Logger::debug("Close file");
    m_logText->clear();
    m_currentLog = nullptr;
    updateLogFileName();
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

QString MainWindow::capitalize(const QString &str) {
    return str.at(0).toUpper() + str.mid(1);
}