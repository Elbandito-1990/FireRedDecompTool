#ifndef POKEMONTAB_H
#define POKEMONTAB_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QList>
#include <QSlider>
#include <QLineEdit>

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

    int     eggCycles     = 0;  // new

    // you may also want to store steps in the data model, but here we compute on-the-fly
};

class PokemonTab : public QWidget
{
    Q_OBJECT
public:
    explicit PokemonTab(const QString &projectPath, QWidget *parent = nullptr);

private slots:
    void onSpeciesChanged(int index);

private:
    void loadConstants();
    void loadSpeciesList();
    bool loadSpeciesInfo(const QString &speciesMacro, PokemonInfo &outInfo);
    QString prettify(const QString &rawMacro);
    void populateUI(const PokemonInfo &info);
    void loadSprites(const QString &speciesMacro);

    QString path;

    QMap<QString, QString> typeMap;
    QMap<QString, QString> abilityMap;
    QMap<QString, QString> growthMap;
    QMap<QString, QString> eggMap;

    QList<QString> speciesMacroList;

    QComboBox *speciesCombo;

    QLabel *spriteLabelFront;
    QLabel *spriteLabelBack;
    QLabel *spriteLabelIcon;
    QLabel *spriteLabelFootprint;

    QSpinBox *hpSpin;
    QSpinBox *atkSpin;
    QSpinBox *defSpin;
    QSpinBox *spaSpin;
    QSpinBox *spdSpin;
    QSpinBox *speSpin;

    QComboBox *type1Combo;
    QComboBox *type2Combo;

    QComboBox *ability1Combo;
    QComboBox *ability2Combo;
    QComboBox *abilityHiddenCombo;

    QComboBox *growthCombo;

    QSlider   *genderSlider;
    QLineEdit *genderDecimalField;

    QComboBox *egg1Combo;
    QComboBox *egg2Combo;

    QSpinBox  *catchRateSpin;
    QSpinBox  *expYieldSpin;
    QSpinBox  *friendshipSpin;

    QSpinBox  *eggCycleSpin;   // existing
    QLineEdit *stepsField;     // ← declare it here

};

#endif // POKEMONTAB_H