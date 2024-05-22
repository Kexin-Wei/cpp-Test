
#include <QMainWindow>

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
 void createMenu();
 void createCentralWidget();
 void createStatusBar();
 void createActions();
 void setMainWindowSize();
 QWidget* createSideBar();
 QWidget* createLogView();
};