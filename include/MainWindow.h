
#pragma once

#include <LogTextProcessor.h>

#include <QAction>
#include <QFileInfo>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QThread>
#include <QVBoxLayout>
#include <set>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void openFile();
    void closeFile();

    void updateLevelCheckBoxes(const std::vector<QCheckBox*>& levelCheckBoxes);
    void updateClassCheckBoxes(const std::vector<QCheckBox*>& classCheckBoxes);

    void updateLogText(const QString& logTextHtml);

   private:
    // creating ui
    void setMainWindowSize();
    void createActions();
    void createMenu();
    void createStatusBar();

    void createCentralWidget();
    QWidget* createSideBar();
    QWidget* createLogView();

    void showHelpDialog();
    void showAboutDialog();
    const QString getHelpText();

    // update ui from file
    void updateLogFileNameFromFile();

    QFileInfo* m_currentLog;
    QLabel* m_logFileName;
    QTextEdit* m_logText;
    LogTextProcessor* m_logTextProcessor;
    QThread* m_logTextProcessorThread;

    QAction* m_helpAction;
    QAction* m_aboutAction;
    QAction* m_openAction;
    QAction* m_closeAction;

    QVBoxLayout* m_levelCheckBoxLayout;
    QVBoxLayout* m_classCheckBoxLayout;
    std::vector<QCheckBox*> m_previousClassCheckBoxes;
};