#pragma once
#include <QWidget>
#include <QString>

class PokedexTab : public QWidget {
    Q_OBJECT
public:
    explicit PokedexTab(const QString &projectPath, QWidget *parent = nullptr);

private:
    QString projectPath;
};