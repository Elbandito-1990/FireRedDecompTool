#ifndef STARTUPWINDOW_H
#include <QPainter>
#define STARTUPWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

class StartupWindow : public QWidget {
    Q_OBJECT

public:
    void paintEvent(QPaintEvent *event) override;
    StartupWindow(QWidget *parent = nullptr);
    QString getPath() const;

signals:
    void pathSelected(const QString &path);

private:
    QLineEdit *pathEdit;
    QPushButton *browseButton;
    QPushButton *openButton;

    void browse();
};

#endif // STARTUPWINDOW_H
