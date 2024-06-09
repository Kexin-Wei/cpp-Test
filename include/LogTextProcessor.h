#pragma once
#include <QButtonGroup>
#include <QCheckBox>
#include <QObject>
#include <QWidget>
#include <set>
enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL };

class LogTextProcessor : public QObject {
    Q_OBJECT
   public:
    LogTextProcessor(QWidget* parent = nullptr);
    ~LogTextProcessor();

    QString getLogText() const { return m_logTextHtml; }
    std::map<LogLevel, QString> getLevels() const { return m_levels; }
    QButtonGroup* getLevelChoiceGroup() const { return m_levelChoiceGroup; }

   signals:
    void updateClassCheckBoxes(const std::vector<QCheckBox*>& classCheckBoxes);
    void updateLevelCheckBoxes(const std::vector<QCheckBox*>& levelCheckBoxes);
    void logTextLoaded(const QString& logTextHtml);
    void logTextProcessed(const QString& logTextHtml);

   public slots:
    void setLogText(const QString& logTextHtml);

   private:
    void createLevelButtons();

    // void updateSideBarFiltersFromFile();
    void updateLogViewFromFile();

    void updateLevelFilterFromFile();
    void updateClassFilterFromFile();

    // general functions
    void filterLogText(QString& logTextHtml);
    void filterLogText(QString& logTextHtml, const std::set<QString>& classes,
                       const std::set<QString>& levels);
    void showLevelInDifferentColor(QString& logTextHtml);

    // utils
    bool getLogLevelFromString(const QString& level, LogLevel& logLevel);
    QString capitalize(const QString& str);

    QWidget* m_parent;
    QString m_logTextHtml;
    QString m_logTextHtmlBackup;
    const QString m_levelReg = "\\[(\\w+)\\]";
    const QString m_classReg = "\\s*(\\w+)\\s*-";

    std::set<QString> m_classes;
    std::set<QString> m_classesFromLog;
    std::set<QString> m_levelsFromLog;
    std::map<LogLevel, QString> m_levels;
    std::map<LogLevel, QColor> m_levelColors;

    QButtonGroup* m_levelChoiceGroup;
    QButtonGroup* m_classChoiceGroup;
    std::map<QString, QCheckBox*> m_classCheckBoxes;
    std::map<LogLevel, QCheckBox*> m_levelCheckBoxes;
};