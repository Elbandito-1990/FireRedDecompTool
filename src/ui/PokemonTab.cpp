#include "PokemonTab.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QPixmap>
#include <QAbstractSpinBox>
#include <QWidget>

PokemonTab::PokemonTab(const QString &projectPath, QWidget *parent)
    : QWidget(parent), path(projectPath)
{
    loadConstants();

    speciesCombo = new QComboBox(this);
    speciesCombo->setFixedWidth(200);
    speciesCombo->addItem("Select a Pokémon…");
    connect(speciesCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PokemonTab::onSpeciesChanged);

    QLabel *pathLabel = new QLabel("Project Path: " + path, this);
    pathLabel->setStyleSheet("padding-left: 10px; padding-top: 5px;");

    auto makeStatSpin = [this]() {
        QSpinBox *s = new QSpinBox(this);
        s->setRange(1, 255);
        s->setButtonSymbols(QAbstractSpinBox::NoButtons);
        s->setReadOnly(true);
        return s;
    };
    hpSpin = makeStatSpin(); atkSpin = makeStatSpin(); defSpin = makeStatSpin();
    spaSpin = makeStatSpin(); spdSpin = makeStatSpin(); speSpin = makeStatSpin();

    type1Combo = new QComboBox(this); type2Combo = new QComboBox(this);
    ability1Combo = new QComboBox(this); ability2Combo = new QComboBox(this); abilityHiddenCombo = new QComboBox(this);
    growthCombo = new QComboBox(this); egg1Combo = new QComboBox(this); egg2Combo = new QComboBox(this);

    for (const QString &p : typeMap.values()) { type1Combo->addItem(p); type2Combo->addItem(p); }
    for (const QString &p : abilityMap.values()) {
        ability1Combo->addItem(p); ability2Combo->addItem(p); abilityHiddenCombo->addItem(p);
    }
    for (const QString &p : growthMap.values()) growthCombo->addItem(p);
    for (const QString &p : eggMap.values()) { egg1Combo->addItem(p); egg2Combo->addItem(p); }

    for (auto *w : {type1Combo, type2Combo, ability1Combo, ability2Combo,
                    abilityHiddenCombo, growthCombo, egg1Combo, egg2Combo})
        w->setEnabled(false);

    genderSlider = new QSlider(Qt::Horizontal, this);
    genderSlider->setRange(0, 100);
    genderSlider->setTickInterval(10);
    genderSlider->setTickPosition(QSlider::TicksBelow);
    genderSlider->setFixedWidth(200);
    genderSlider->setEnabled(false);

    genderDecimalField = new QLineEdit(this);
    genderDecimalField->setFixedWidth(60);
    genderDecimalField->setAlignment(Qt::AlignRight);
    genderDecimalField->setReadOnly(true);
    genderDecimalField->setEnabled(false);

    catchRateSpin  = new QSpinBox(this);
    expYieldSpin   = new QSpinBox(this);
    friendshipSpin = new QSpinBox(this);
    for (auto *s : {catchRateSpin, expYieldSpin, friendshipSpin}) {
        s->setButtonSymbols(QAbstractSpinBox::NoButtons);
        s->setReadOnly(true);
    }
    catchRateSpin ->setRange(0, 255);
    expYieldSpin  ->setRange(0, 1000);
    friendshipSpin->setRange(0, 255);

    spriteLabelBack      = new QLabel(this);
    spriteLabelFront     = new QLabel(this);
    spriteLabelIcon      = new QLabel(this);
    spriteLabelFootprint = new QLabel(this);

    spriteLabelBack     ->setFixedSize(96, 96);
    spriteLabelFront    ->setFixedSize(96, 96);
    spriteLabelIcon     ->setFixedSize(64, 64);
    spriteLabelFootprint->setFixedSize(96, 96);

    spriteLabelBack     ->setAlignment(Qt::AlignCenter);
    spriteLabelFront    ->setAlignment(Qt::AlignCenter);
    spriteLabelIcon     ->setAlignment(Qt::AlignCenter);
    spriteLabelFootprint->setAlignment(Qt::AlignCenter);

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QWidget *leftWidget = new QWidget(this);
    QVBoxLayout *main = new QVBoxLayout(leftWidget);
    main->setContentsMargins(0, 0, 0, 0);
    main->setSpacing(0);
        // ─── Header ───
    QHBoxLayout *header = new QHBoxLayout;
    header->setContentsMargins(10, 10, 0, 0);
    header->addWidget(speciesCombo);
    header->addStretch();
    main->addLayout(header);

    // ─── Summary ───
    QVBoxLayout *summary = new QVBoxLayout;
    summary->setContentsMargins(10, 10, 10, 10);

    QLabel *spritesHeader = new QLabel("Sprites", this);
    spritesHeader->setAlignment(Qt::AlignCenter);
    spritesHeader->setStyleSheet("font-weight: bold; font-size: 14px; padding-top: 10px;");
    summary->addWidget(spritesHeader);

    // ─── Corrected Sprite Grid ───
    QGridLayout *spriteGrid = new QGridLayout;
    spriteGrid->setHorizontalSpacing(15);
    spriteGrid->setVerticalSpacing(5);

    // Row 0: Top sprites
    spriteGrid->addWidget(spriteLabelIcon,      0, 0, Qt::AlignCenter);
    spriteGrid->addWidget(spriteLabelFootprint, 0, 1, Qt::AlignCenter);
    spriteGrid->addWidget(spriteLabelFront,     2, 0, Qt::AlignCenter);
    spriteGrid->addWidget(spriteLabelBack,      2, 1, Qt::AlignCenter);

    // Row 1: Labels directly underneath each sprite
    spriteGrid->addWidget(new QLabel("Icon", this),      1, 0, Qt::AlignCenter);
    spriteGrid->addWidget(new QLabel("Footprint", this), 1, 1, Qt::AlignCenter);
    spriteGrid->addWidget(new QLabel("Front", this),     3, 0, Qt::AlignCenter);
    spriteGrid->addWidget(new QLabel("Back", this),      3, 1, Qt::AlignCenter);

    summary->addLayout(spriteGrid);

    QLabel *statsHeader = new QLabel("Base Stats", this);
    statsHeader->setAlignment(Qt::AlignCenter);
    statsHeader->setStyleSheet("font-weight: bold; font-size: 14px; padding-top: 10px;");
    summary->addWidget(statsHeader);

    QGridLayout *stats = new QGridLayout;
    stats->addWidget(new QLabel("HP", this),        0, 0); stats->addWidget(hpSpin , 0, 1);
    stats->addWidget(new QLabel("Attack", this),    1, 0); stats->addWidget(atkSpin, 1, 1);
    stats->addWidget(new QLabel("Defense", this),   2, 0); stats->addWidget(defSpin, 2, 1);
    stats->addWidget(new QLabel("Sp. Atk", this),   0, 2); stats->addWidget(spaSpin, 0, 3);
    stats->addWidget(new QLabel("Sp. Def", this),   1, 2); stats->addWidget(spdSpin, 1, 3);
    stats->addWidget(new QLabel("Speed", this),     2, 2); stats->addWidget(speSpin, 2, 3);
    stats->setHorizontalSpacing(20);
    summary->addSpacing(8);
    summary->addLayout(stats, Qt::AlignCenter);

    // ─── Typing ───
    QLabel *typingHeader = new QLabel("Typing", this);
    typingHeader->setAlignment(Qt::AlignCenter);
    typingHeader->setStyleSheet("font-weight: bold; font-size: 14px; padding-top: 10px;");
    summary->addWidget(typingHeader);

    QHBoxLayout *typingRow = new QHBoxLayout;
    typingRow->setAlignment(Qt::AlignCenter);
    typingRow->setSpacing(10);
    typingRow->addWidget(new QLabel("Type 1:", this));
    typingRow->addWidget(type1Combo);
    typingRow->addWidget(new QLabel("Type 2:", this));
    typingRow->addWidget(type2Combo);
    summary->addSpacing(12);
    summary->addLayout(typingRow);

    // ─── Abilities ───
    QLabel *abilityHeader = new QLabel("Abilities", this);
    abilityHeader->setAlignment(Qt::AlignCenter);
    abilityHeader->setStyleSheet("font-weight: bold; font-size: 14px; padding-top: 10px;");
    summary->addSpacing(12);
    summary->addWidget(abilityHeader);

    QHBoxLayout *abilityRow1 = new QHBoxLayout;
    abilityRow1->setAlignment(Qt::AlignCenter);
    abilityRow1->setSpacing(20);
    abilityRow1->addWidget(new QLabel("Ability 1:", this));
    abilityRow1->addWidget(ability1Combo);
    abilityRow1->addWidget(new QLabel("Ability 2:", this));
    abilityRow1->addWidget(ability2Combo);

    QHBoxLayout *abilityRow2 = new QHBoxLayout;
    abilityRow2->setAlignment(Qt::AlignCenter);
    abilityRow2->setSpacing(20);
    abilityRow2->addWidget(new QLabel("Hidden Ability:", this));
    abilityRow2->addWidget(abilityHiddenCombo);

    summary->addLayout(abilityRow1);
    summary->addSpacing(6);
    summary->addLayout(abilityRow2);

    // ─── Gender ───
    QLabel *genderHeader = new QLabel("Gender", this);
    genderHeader->setAlignment(Qt::AlignCenter);
    genderHeader->setStyleSheet("font-weight: bold; font-size: 14px; padding-top: 10px;");
    summary->addSpacing(12);
    summary->addWidget(genderHeader);

    QHBoxLayout *genderSliderRow = new QHBoxLayout;
    genderSliderRow->setAlignment(Qt::AlignCenter);
    genderSliderRow->setSpacing(10);
    genderSliderRow->addWidget(new QLabel("♀ Female", this));
    genderSliderRow->addWidget(genderSlider);
    genderSliderRow->addWidget(new QLabel("♂ Male", this));
    summary->addLayout(genderSliderRow);

    QHBoxLayout *genderPercentRow = new QHBoxLayout;
    genderPercentRow->setAlignment(Qt::AlignCenter);
    genderPercentRow->setSpacing(6);
    genderPercentRow->addWidget(new QLabel("Gender Ratio:", this));
    genderPercentRow->addWidget(genderDecimalField);
    genderPercentRow->addWidget(new QLabel("% Female", this));
    summary->addLayout(genderPercentRow);

        main->addLayout(summary);

    // ─── Detail Form ───
    QFormLayout *details = new QFormLayout;
    details->setLabelAlignment(Qt::AlignRight);
    details->addRow("Growth Rate:",    growthCombo);
    details->addRow("Egg Group 1:",    egg1Combo);
    details->addRow("Egg Group 2:",    egg2Combo);
    details->addRow("Catch Rate:",     catchRateSpin);
    details->addRow("Base EXP:",       expYieldSpin);
    details->addRow("Friendship:",     friendshipSpin);
    main->addLayout(details);
    main->addStretch();

    root->addWidget(leftWidget, 0);

    QWidget *rightWidget = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 10, 10, 0);
    rightLayout->addWidget(pathLabel, 0, Qt::AlignTop | Qt::AlignRight);
    rightLayout->addStretch();

    root->addWidget(rightWidget, 1);

    loadSpeciesList();
}

void PokemonTab::onSpeciesChanged(int index)
{
    if (index <= 0 || index > speciesMacroList.size()) {
        populateUI(PokemonInfo{});
        spriteLabelFront->clear();
        spriteLabelBack->clear();
        spriteLabelIcon->clear();
        spriteLabelFootprint->clear();
        return;
    }

    const QString macro = speciesMacroList.at(index - 1);
    PokemonInfo info;
    if (loadSpeciesInfo(macro, info)) {
        populateUI(info);
        loadSprites(macro);
    }
}

void PokemonTab::loadSprites(const QString &speciesMacro)
{
    const QString name   = speciesMacro.toLower();
    const QString folder = path + "/graphics/pokemon/" + name + "/";

    const QString frontPath     = folder + "front.png";
    const QString backPath      = folder + "back.png";
    const QString iconPath      = folder + "icon.png";
    const QString footprintPath = folder + "footprint.png";

    if (QFile::exists(frontPath)) {
        QPixmap pix(frontPath);
        spriteLabelFront->setPixmap(pix.scaled(spriteLabelFront->size(),
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));
    } else {
        spriteLabelFront->clear();
    }

    if (QFile::exists(backPath)) {
        QPixmap pix(backPath);
        spriteLabelBack->setPixmap(pix.scaled(spriteLabelBack->size(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation));
    } else {
        spriteLabelBack->clear();
    }

    if (QFile::exists(iconPath)) {
        QPixmap pix(iconPath);
        spriteLabelIcon->setPixmap(pix.scaled(spriteLabelIcon->size(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation));
    } else {
        spriteLabelIcon->clear();
    }

    if (QFile::exists(footprintPath)) {
        QPixmap pix(footprintPath);
        spriteLabelFootprint->setPixmap(pix.scaled(spriteLabelFootprint->size(),
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));
    } else {
        spriteLabelFootprint->clear();
    }
}

void PokemonTab::populateUI(const PokemonInfo &i)
{
    hpSpin ->setValue(i.baseHP);
    atkSpin->setValue(i.baseAttack);
    defSpin->setValue(i.baseDefense);
    spaSpin->setValue(i.baseSpAttack);
    spdSpin->setValue(i.baseSpDefense);
    speSpin->setValue(i.baseSpeed);

    type1Combo->setCurrentText(i.type1);
    type2Combo->setCurrentText(i.type2);

    ability1Combo     ->setCurrentText(i.ability1);
    ability2Combo     ->setCurrentText(i.ability2);
    abilityHiddenCombo->setCurrentText(i.abilityHidden);

    growthCombo->setCurrentText(i.growthRate);

    bool ok = false;
    double percent = i.genderRatio.split('%').first().toDouble(&ok);
    if (ok) {
        genderSlider->setValue(static_cast<int>(percent));
        genderDecimalField->setText(QString::number(percent, 'f', 1));
        genderSlider->setToolTip(QString::number(percent, 'f', 1) + "% Female");
    } else {
        genderSlider->setValue(0);
        genderDecimalField->setText("-");
        genderSlider->setToolTip("-");
    }

    egg1Combo->setCurrentText(i.eggGroup1);
    egg2Combo->setCurrentText(i.eggGroup2);

    catchRateSpin ->setValue(i.catchRate);
    expYieldSpin  ->setValue(i.expYield);
    friendshipSpin->setValue(i.friendship);
}

QString PokemonTab::prettify(const QString &raw)
{
    QString out = raw.toLower().replace('_', ' ');
    bool capNext = true;
    for (QChar &c : out) {
        if (capNext && c.isLetter()) {
            c = c.toUpper();
            capNext = false;
        }
        if (c == ' ') capNext = true;
    }
    return out;
}

void PokemonTab::loadConstants()
{
    const QString constFile = path + "/include/constants/pokemon.h";
    QFile f(constFile);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f);
        QRegularExpression rxType  ("#define\\s+TYPE_([A-Z_]+)\\s+\\d+");
        QRegularExpression rxGrowth("#define\\s+GROWTH_([A-Z_]+)\\s+\\d+");
        QRegularExpression rxEgg   ("#define\\s+EGG_GROUP_([A-Z_]+)\\s+\\d+");
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (auto m = rxType.match(line); m.hasMatch())
                typeMap.insert("TYPE_" + m.captured(1), prettify(m.captured(1)));
            if (auto m = rxGrowth.match(line); m.hasMatch())
                growthMap.insert("GROWTH_" + m.captured(1), prettify(m.captured(1)));
            if (auto m = rxEgg.match(line); m.hasMatch())
                eggMap.insert("EGG_GROUP_" + m.captured(1), prettify(m.captured(1)));
        }
        f.close();
    }

    const QString abilFile = path + "/include/constants/abilities.h";
    QFile f2(abilFile);
    if (f2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f2);
        QRegularExpression rx("#define\\s+ABILITY_([A-Z_]+)\\s+\\d+");
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (auto m = rx.match(line); m.hasMatch())
                abilityMap.insert("ABILITY_" + m.captured(1), prettify(m.captured(1)));
        }
        f2.close();
    }
}

void PokemonTab::loadSpeciesList()
{
    const QString file = path + "/include/constants/species.h";
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&f);
    QRegularExpression rx("#define\\s+SPECIES_([A-Z0-9_]+)\\s+\\d+");
    while (!in.atEnd()) {
        QString line = in.readLine();
        auto m = rx.match(line);
        if (!m.hasMatch()) continue;

        QString macro = m.captured(1);
        if (macro == "NONE") continue;

        speciesMacroList << macro;
        speciesCombo->addItem(prettify(macro));
    }
}

bool PokemonTab::loadSpeciesInfo(const QString &speciesMacro, PokemonInfo &out)
{
    const QString file = path + "/src/data/pokemon/species_info.h";
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&f);
    QString header = "[SPECIES_" + speciesMacro + "]";
    bool grab = false;
    int  braces = 0;
    QString block;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!grab) { if (line.contains(header)) grab = true; continue; }
        block += line + '\n';
        if (line.contains('{')) ++braces;
        if (line.contains('}')) { --braces; if (!braces) break; }
    }
    f.close();
    if (block.isEmpty()) return false;

    auto getInt = [&](const QString &k, int &dst) {
        QRegularExpression rx("\\." + k + "\\s*=\\s*(\\d+)");
        if (auto m = rx.match(block); m.hasMatch()) dst = m.captured(1).toInt();
    };

    getInt("baseHP",       out.baseHP);
    getInt("baseAttack",   out.baseAttack);
    getInt("baseDefense",  out.baseDefense);
    getInt("baseSpAttack", out.baseSpAttack);
    getInt("baseSpDefense",out.baseSpDefense);
    getInt("baseSpeed",    out.baseSpeed);
    getInt("catchRate",    out.catchRate);
    getInt("expYield",     out.expYield);
    getInt("friendship",   out.friendship);

    if (auto m = QRegularExpression(R"(.types\s*=\s*\{TYPE_([A-Z_]+)\s*,\s*TYPE_([A-Z_]+)\})").match(block); m.hasMatch()) {
        out.type1 = typeMap.value("TYPE_" + m.captured(1), prettify(m.captured(1)));
        out.type2 = typeMap.value("TYPE_" + m.captured(2), prettify(m.captured(2)));
    }

    if (auto m = QRegularExpression(R"(.abilities\s*=\s*\{ABILITY_([A-Z_]+)\s*,\s*ABILITY_([A-Z_]+)(?:\s*,\s*ABILITY_([A-Z_]+))?)").match(block); m.hasMatch()) {
        out.ability1 = abilityMap.value("ABILITY_" + m.captured(1), prettify(m.captured(1)));
        out.ability2 = abilityMap.value("ABILITY_" + m.captured(2), prettify(m.captured(2)));
        out.abilityHidden = m.captured(3).isEmpty() ? "-" : abilityMap.value("ABILITY_" + m.captured(3), prettify(m.captured(3)));
    }

    if (auto m = QRegularExpression(R"(.eggGroups\s*=\s*\{EGG_GROUP_([A-Z_]+)\s*,\s*EGG_GROUP_([A-Z_]+)\})").match(block); m.hasMatch()) {
        out.eggGroup1 = eggMap.value("EGG_GROUP_" + m.captured(1), prettify(m.captured(1)));
        out.eggGroup2 = eggMap.value("EGG_GROUP_" + m.captured(2), prettify(m.captured(2)));
    }

    if (auto m = QRegularExpression(R"(.growthRate\s*=\s*GROWTH_([A-Z_]+))").match(block); m.hasMatch()) {
        out.growthRate = growthMap.value("GROWTH_" + m.captured(1), prettify(m.captured(1)));
    }

    if (auto m = QRegularExpression(R"(.genderRatio\s*=\s*([A-Z_()%\.0-9]+))").match(block); m.hasMatch()) {
        QString raw = m.captured(1).trimmed();
        if (raw.startsWith("PERCENT_FEMALE")) {
            int l = raw.indexOf('('), r = raw.indexOf(')');
            QString v = raw.mid(l + 1, r - l - 1);
            out.genderRatio = v + "% female";
        } else {
            out.genderRatio = prettify(raw);
        }
    }

    return true;
}