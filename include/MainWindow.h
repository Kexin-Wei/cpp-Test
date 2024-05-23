
#include <QMainWindow>

class QAction;
class QTextEdit;
class QFileInfo;
class QLabel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
 MainWindow(QWidget* parent = nullptr);
 ~MainWindow();

private slots:
 void openFile();
 void closeFile();

private:
 void setMainWindowSize();
 void createActions();
 void createMenu();
 void createStatusBar();

 void createCentralWidget();
 QWidget* createSideBar();
 QWidget* createLogView();
 void updateLogFileName();

 void showHelpDialog();
 void showAboutDialog();
 const QString getHelpText();

 QTextEdit* m_logText;
 QFileInfo* m_currentLog;
 QLabel* m_logFileName;
 QAction* m_helpAction;
 QAction* m_aboutAction;
 QAction* m_openAction;
 QAction* m_closeAction;
};