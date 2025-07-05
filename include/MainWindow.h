#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setProjectPath(const QString &path);
    void addPokemonTab();

private:
    QTabWidget *tabWidget;
    QString projectPath;
};

#endif // MAINWINDOW_H
