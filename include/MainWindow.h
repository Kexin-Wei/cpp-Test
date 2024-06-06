
#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColor>
#include <QFileInfo>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>
#include <set>

enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL };

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void closeFile();

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
    void fileChanged();
    void updateLogFileNameFromFile();
    void updateSideBarFiltersFromFile();
    void updateLogViewFromFile();

    void updateLevelFilterFromFile();
    void updateClassFilterFromFile();

    // general functions
    void filterLogText(QString& logTextHtml);
    void filterLogText(QString& logTextHtml, const std::set<QString>& classes,
                       const std::set<LogLevel>& levels);
    void showLevelInDifferentColor(QString & logTextHtml);

    // utils
    bool getLogLevelFromString(const QString& level, LogLevel& logLevel);
    QString capitalize(const QString& str);

    QFileInfo* m_currentLog;
    QLabel* m_logFileName;
    QTextEdit* m_logText;
    QString* m_logTextHtmlBackup;

    QAction* m_helpAction;
    QAction* m_aboutAction;
    QAction* m_openAction;
    QAction* m_closeAction;

    QVBoxLayout* m_levelCheckBoxLayout;
    QVBoxLayout* m_classCheckBoxLayout;
    QButtonGroup* m_levelChoiceGroup;
    QButtonGroup* m_classChoiceGroup;

    std::set<QString> m_classes;
    std::set<QString> m_classesFromLog;
    std::set<QString> m_levelsFromLog;

    const QString m_levelReg = "\\[(\\w+)\\]";
    const QString m_classReg = "\\s*(\\w+)\\s*-";

    std::map<QString, QCheckBox*> m_classCheckBoxes;
    std::map<LogLevel, QCheckBox*> m_levelCheckBoxes;
    std::map<LogLevel, QString> m_levels;
    std::map<LogLevel, QColor> m_levelColors;
};