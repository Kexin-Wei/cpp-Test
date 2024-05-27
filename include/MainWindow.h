
#include <QMainWindow>
#include <QVBoxLayout>
#include <QAction>
#include <QTextEdit>
#include <QFileInfo>
#include <QLabel>
#include <QCheckBox>
#include <QButtonGroup>

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

 void updateLevelFilter();
 void updateClassFilter();
 std::vector<std::string> getAvailaleLevelsFromLog();
 std::vector<std::string> getClassesFromLog();

 QTextEdit* m_logText;
 QFileInfo* m_currentLog;
 QLabel* m_logFileName;

 QAction* m_helpAction;
 QAction* m_aboutAction;
 QAction* m_openAction;
 QAction* m_closeAction;

 QVBoxLayout* m_levelChoiceLayout;
 QVBoxLayout* m_classChoiceLayout;
 QButtonGroup* m_levelChoiceGroup;
 QButtonGroup* m_classChoiceGroup;

 std::map<std::string, QCheckBox*> m_levelCheckBoxes;
 std::vector<std::string> m_levels;

 std::map<std::string, QCheckBox*> m_classCheckBoxes;
 std::vector<std::string> m_classes;
};