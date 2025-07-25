#ifndef POKEMONTAB_H
#define POKEMONTAB_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QList>
#include <QSlider>
#include <QLineEdit>
#include <QRadioButton>
#include <QByteArray>
#include <QColor>
#include <QImage>

class QComboBox;
class QSpinBox;
class QLabel;

struct PokemonInfo
{
    int     baseHP        = 0;
    int     baseAttack    = 0;
    int     baseDefense   = 0;
    int     baseSpAttack  = 0;
    int     baseSpDefense = 0;
    int     baseSpeed     = 0;

    QString type1;
    QString type2;

    int     catchRate     = 0;
    int     expYield      = 0;
    int     friendship    = 0;

    QString growthRate;
    QString genderRatio;

    QString eggGroup1;
    QString eggGroup2;

    QString ability1;
    QString ability2;
    QString abilityHidden;

    int     eggCycles     = 0;

    QString displayName;    // actual in-game species name
};

class PokemonTab : public QWidget
{
    Q_OBJECT

public:
    explicit PokemonTab(const QString &projectPath,
                        QWidget *parent = nullptr);

private slots:
    void onSpeciesChanged(int index);
    void onNameEdited();
    void updateSpriteVariant();

private:
    bool saveSpeciesName(const QString &speciesMacro,
                         const QString &newName);

    void loadConstants();
    void loadSpeciesList();
    bool loadSpeciesInfo(const QString &speciesMacro,
                         PokemonInfo &outInfo);
    QString prettify(const QString &rawMacro);
    void populateUI(const PokemonInfo &info);
    void loadSprites(const QString &speciesMacro);

    // helpers for .4bpp + .pal rendering
    QByteArray      readFile(const QString &path);
    QVector<QColor> parsePalFile(const QString &palPath);
    QImage          render4bppSprite(const QByteArray &data,
                                     const QVector<QColor> &palette,
                                     const QString &pngPath);

    // project data
    QString path;
    QString currentSpecies;

    QMap<QString, QString> typeMap;
    QMap<QString, QString> abilityMap;
    QMap<QString, QString> growthMap;
    QMap<QString, QString> eggMap;

    QList<QString> speciesMacroList;

    // UI controls
    QComboBox   *speciesCombo;
    QLineEdit   *nameField;

    QLabel      *spriteLabelFront;
    QLabel      *spriteLabelBack;
    QLabel      *spriteLabelIcon;
    QLabel      *spriteLabelFootprint;

    QSpinBox    *hpSpin;
    QSpinBox    *atkSpin;
    QSpinBox    *defSpin;
    QSpinBox    *spaSpin;
    QSpinBox    *spdSpin;
    QSpinBox    *speSpin;

    QComboBox   *type1Combo;
    QComboBox   *type2Combo;

    QComboBox   *ability1Combo;
    QComboBox   *ability2Combo;
    QComboBox   *abilityHiddenCombo;

    QComboBox   *growthCombo;

    QSlider     *genderSlider;
    QLineEdit   *genderDecimalField;

    QComboBox   *egg1Combo;
    QComboBox   *egg2Combo;

    QSpinBox    *catchRateSpin;
    QSpinBox    *expYieldSpin;
    QSpinBox    *friendshipSpin;

    QSpinBox    *eggCycleSpin;
    QLineEdit   *stepsField;

    // shiny toggle
    QRadioButton *normalRadio;
    QRadioButton *shinyRadio;
};

#endif // POKEMONTAB_H