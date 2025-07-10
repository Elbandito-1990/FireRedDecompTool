#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setProjectPath(const QString &path);
    void initializeTabs();  // <-- Moved to public

private:
    QTabWidget *tabWidget;
    QString projectPath;
};

#endif // MAINWINDOW_H