#include <LogTextProcessor.h>
#include <Logger.h>

#include <QCheckBox>
#include <QColor>
#include <QRegularExpression>

LogTextProcessor::LogTextProcessor(QWidget *parent)
    : QObject(parent),
      m_parent(parent),
      m_classesFromLog({}),
      m_classes({}),
      m_levelsFromLog({}),
      m_levels({{LogLevel::TRACE, "trace"},
                {LogLevel::DEBUG, "debug"},
                {LogLevel::INFO, "info"},
                {LogLevel::WARNING, "warning"},
                {LogLevel::ERROR, "error"},
                {LogLevel::CRITICAL, "critical"}}),
      m_levelColors({{LogLevel::TRACE, Qt::gray},
                     {LogLevel::DEBUG, Qt::blue},
                     {LogLevel::INFO, Qt::darkGreen},
                     {LogLevel::WARNING, Qt::darkYellow},
                     {LogLevel::ERROR, Qt::red},
                     {LogLevel::CRITICAL, Qt::darkRed}}),
      m_levelChoiceGroup(new QButtonGroup(this)),
      m_classChoiceGroup(new QButtonGroup(this)) {
    m_levelChoiceGroup->setExclusive(false);
    m_classChoiceGroup->setExclusive(false);

    auto allLevelButton = new QCheckBox("All", m_parent);
    m_levelChoiceGroup->addButton(allLevelButton);
    m_levelChoiceGroup->setId(allLevelButton, 0);

    auto allClassButton = new QCheckBox("All", m_parent);
    m_classChoiceGroup->addButton(allClassButton);
    m_classChoiceGroup->setId(allClassButton, 0);

    connect(this, &LogTextProcessor::logTextLoaded, this,
            &LogTextProcessor::setLogText);
}

LogTextProcessor::~LogTextProcessor() {
    for (const auto &[levelEnum, checkBox] : m_levelCheckBoxes) {
        delete checkBox;
    }
    for (const auto &[className, checkBox] : m_classCheckBoxes) {
        delete checkBox;
    }
    delete m_levelChoiceGroup;
    delete m_classChoiceGroup;
}

void LogTextProcessor::setLogText(const QString &logTextHtml) {
    m_logTextHtml = logTextHtml;
    m_logTextHtmlBackup = logTextHtml;
    // updateSideBarFiltersFromFile();
    // updateLogViewFromFile();
}

void LogTextProcessor::updateLevelFilterFromFile() {
    Logger::debug("Level checkboxes updating");
    int countEnabled = 0;
    for (const auto &level : m_levelsFromLog) {
        Logger::debug("Level from log: {}", level.toStdString());
    }
    for (const auto &[levelEnum, levelString] : m_levels) {
        auto checkBox = m_levelCheckBoxes[levelEnum];
        if (auto search = m_levelsFromLog.find(levelString);
            search != m_levelsFromLog.end()) {
            checkBox->setEnabled(true);
            checkBox->setChecked(true);
            countEnabled++;
            Logger::debug("Level checkbox enabled: {}",
                          levelString.toStdString());

        } else {
            checkBox->setEnabled(false);
            checkBox->setChecked(false);
            Logger::trace("Level checkbox disabled: {}",
                          levelString.toStdString());
        }
    }
    if (countEnabled == m_levelsFromLog.size()) {
        m_levelChoiceGroup->button(0)->setChecked(true);
    }
    Logger::trace("Level checkboxes updated");
}

void LogTextProcessor::updateClassFilterFromFile() {
    Logger::debug("Class checkboxes updating");
    // remove all existing class checkboxes
    for (const auto &className : m_classes) {
        auto button = m_classCheckBoxes[className];
        m_classChoiceGroup->removeButton(button);
        delete button;
    }
    m_classCheckBoxes.clear();
    m_classes.clear();
    Logger::trace("Class checkboxes removed");

    m_classes = m_classesFromLog;
    for (const auto &className : m_classes) {
        auto button = new QCheckBox(className, m_parent);
        m_classChoiceGroup->addButton(button);
        button->setEnabled(true);
        button->setChecked(true);
        m_classCheckBoxes[className] = button;
        Logger::trace("Class checkbox created: {}", className.toStdString());
    }
    m_classChoiceGroup->button(0)->setChecked(true);  // default set class all
    Logger::trace("Class checkboxes updated");
}

void LogTextProcessor::filterLogText(QString &logTextHtml) {
    Logger::debug("Log text filtering");
    filterLogText(logTextHtml, m_classes, m_levelsFromLog);

    Logger::debug("Log text filtered");
}

void LogTextProcessor::filterLogText(QString &logTextHtml,
                                     const std::set<QString> &classes,
                                     const std::set<QString> &levels) {
    auto levelRegex = QRegularExpression(m_levelReg);
    bool previousLineMatched = false;
    QString emptyHtml;
    for (auto line : logTextHtml.split("\n")) {
        if (line.isEmpty()) {
            continue;
        }
        auto match = levelRegex.match(line);
        if (match.hasMatch()) {
            auto level = match.captured(1);

            if (levels.find(level) == levels.end()) {
                Logger::trace("Level not matched: {}", level.toStdString());
                continue;
            }

            LogLevel levelEnum;
            if (!getLogLevelFromString(level, levelEnum)) {
                Logger::error("Unknown log level: {}", level.toStdString());
                continue;
            }

            auto checkBox = m_levelCheckBoxes[levelEnum];
            if (checkBox->isChecked()) {
                previousLineMatched = true;
                emptyHtml.append(line);
                Logger::trace("Append line: {}", line.toStdString());
            } else {
                previousLineMatched = false;
            }
        } else {
            if (previousLineMatched) {
                emptyHtml.append(line);
                Logger::trace("Append line: {}", line.toStdString());
            }
        }
    }
    logTextHtml = emptyHtml;

    // class
}

void LogTextProcessor::showLevelInDifferentColor(QString &logTextHtml) {
    Logger::debug("Showing level in different color");
    auto levelClassRegex = QRegularExpression(QString("%1").arg(m_levelReg));
    auto matches = levelClassRegex.globalMatch(logTextHtml);
    std::vector<QRegularExpressionMatch> revertedMatches;

    while (matches.hasNext()) {
        auto match = matches.next();
        revertedMatches.push_back(match);
    }
    for (int i = revertedMatches.size() - 1; i >= 0; i--) {
        auto match = revertedMatches[i];
        auto level = match.captured(1);
        for (const auto &[levelEnum, levelString] : m_levels) {
            if (levelString.toLower() == level.toLower()) {
                auto color = m_levelColors[levelEnum].name();
                auto original = match.capturedTexts().at(0);
                auto replace = QString("<span style=\"color: %1;\">%2</span>")
                                   .arg(color)
                                   .arg(original);
                logTextHtml.replace(match.capturedStart(),
                                    match.capturedLength(), replace);
                Logger::trace("match: {}, replace: {}", original.toStdString(),
                              replace.toStdString());
            }
        }
    }
    Logger::debug("Showed level in different color");
}

 //void LogTextProcessor::updateSideBarFiltersFromFile() {
 //    QRegularExpression levelClassRegex(m_levelReg + m_classReg);
 //    m_levelsFromLog.clear();
 //    m_classesFromLog.clear();

 //    auto text = m_logText->toPlainText();
 //    auto matches = levelClassRegex.globalMatch(text);
 //    while (matches.hasNext()) {
 //        auto match = matches.next();
 //        auto level = match.captured(1);
 //        auto className = match.captured(2);
 //        m_levelsFromLog.insert(capitalize(level.toLower()));
 //        m_classesFromLog.insert(className.toLower());
 //        Logger::trace("Level: {}, Class: {}", level.toStdString(),
 //                      className.toStdString());
 //    }
 //    updateLevelFilterFromFile();
 //    updateClassFilterFromFile();
 //    Logger::debug("Update sidebar filters");
 //}

QString LogTextProcessor::capitalize(const QString &str) {
    return str.at(0).toUpper() + str.mid(1);
}

bool LogTextProcessor::getLogLevelFromString(const QString &level,
                                             LogLevel &logLevel) {
    for (const auto &[levelEnum, levelString] : m_levels) {
        if (levelString.toLower() == level.toLower()) {
            logLevel = levelEnum;
            return true;
        }
    }
    return false;
}