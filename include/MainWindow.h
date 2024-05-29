
#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFileInfo>
#include <QLabel>
#include <QMainWindow>
#include <QTextEdit>
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
    void updateSideBarFilters();

    QTextEdit* m_logText;
    QFileInfo* m_currentLog;
    QLabel* m_logFileName;

    QAction* m_helpAction;
    QAction* m_aboutAction;
    QAction* m_openAction;
    QAction* m_closeAction;

    QVBoxLayout* m_levelCheckBoxLayout;
    QVBoxLayout* m_classCheckBoxLayout;
    QButtonGroup* m_levelChoiceGroup;
    QButtonGroup* m_classChoiceGroup;

    std::map<QString, QCheckBox*> m_levelCheckBoxes;
    std::set<QString> m_levels;
    std::set<QString> m_levelsFromLog;

    std::map<QString, QCheckBox*> m_classCheckBoxes;
    std::set<QString> m_classes;
    std::set<QString> m_classesFromLog;
};