#pragma once

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QMap>

struct DexInfo {
    QString category;
    double heightM = 0.0;
    double weightKg = 0.0;
    QString descriptionKey;
};

class PokedexTab : public QWidget {
    Q_OBJECT

public:
    explicit PokedexTab(const QString &projectPath, QWidget *parent = nullptr);

private slots:
    void onSpeciesSelected(int index);

private:
    void setupUI();
    void loadSpeciesMetadata();
    void updateDexDisplay(const QString &speciesName);
    QString parseSingleDexEntry(const QString &filePath, const QString &descKey);
    QString formatSpeciesName(const QString &raw); // ✅ Declared to fix linker error

    QString projectPath;
    QMap<QString, DexInfo> dexData;

    QComboBox *speciesDropdown;
    QLabel *categoryLabel;
    QLabel *heightLabel;
    QLabel *weightLabel;
    QTextEdit *fireRedTextEdit;
    QTextEdit *leafGreenTextEdit;
};