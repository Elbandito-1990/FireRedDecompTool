#include "PokemonTab.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include <QRadioButton>
#include <QPainter>
#include <QImage>
#include <QColor>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
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

    // ─── Left Column (Summary) ───────────────────────────────
    auto makeStatSpin = [this]() {
        QSpinBox *s = new QSpinBox(this);
        s->setRange(1, 255);
        s->setButtonSymbols(QAbstractSpinBox::NoButtons);
        s->setReadOnly(true);
        return s;
    };
    hpSpin  = makeStatSpin();
    atkSpin = makeStatSpin();
    defSpin = makeStatSpin();
    spaSpin = makeStatSpin();
    spdSpin = makeStatSpin();
    speSpin = makeStatSpin();

    type1Combo         = new QComboBox(this);
    type2Combo         = new QComboBox(this);
    ability1Combo      = new QComboBox(this);
    ability2Combo      = new QComboBox(this);
    abilityHiddenCombo = new QComboBox(this);
    growthCombo        = new QComboBox(this);
    egg1Combo          = new QComboBox(this);
    egg2Combo          = new QComboBox(this);

    for (const QString &p : typeMap.values()) {
        type1Combo->addItem(p);
        type2Combo->addItem(p);
    }
    for (const QString &p : abilityMap.values()) {
        ability1Combo->addItem(p);
        ability2Combo->addItem(p);
        abilityHiddenCombo->addItem(p);
    }
    for (const QString &p : growthMap.values()) {
        growthCombo->addItem(p);
    }
    for (const QString &p : eggMap.values()) {
        egg1Combo->addItem(p);
        egg2Combo->addItem(p);
    }
    for (auto *w : { type1Combo, type2Combo,
                     ability1Combo, ability2Combo, abilityHiddenCombo,
                     growthCombo, egg1Combo, egg2Combo })
    {
        w->setEnabled(false);
    }

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
    for (auto *s : { catchRateSpin, expYieldSpin, friendshipSpin }) {
        s->setButtonSymbols(QAbstractSpinBox::NoButtons);
        s->setReadOnly(true);
    }
    catchRateSpin ->setRange(0, 255);
    expYieldSpin  ->setRange(0, 1000);
    friendshipSpin->setRange(0, 255);

    eggCycleSpin = new QSpinBox(this);
    eggCycleSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    eggCycleSpin->setReadOnly(true);
    eggCycleSpin->setRange(0, 65535);

    stepsField = new QLineEdit(this);
    stepsField->setFixedWidth(60);
    stepsField->setReadOnly(true);
    stepsField->setEnabled(false);

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

    // Left widget layout
    QWidget *leftWidget = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(10, 10, 10, 0);

    leftLayout->addWidget(speciesCombo);
    leftLayout->addSpacing(8);

    // Sprites section
    {
        QLabel *h = new QLabel("Sprites", this);
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("font-weight:bold; font-size:14px; padding-top:10px;");
        leftLayout->addWidget(h);
        leftLayout->addSpacing(8);

        QGridLayout *spriteGrid = new QGridLayout;
        spriteGrid->setHorizontalSpacing(15);
        spriteGrid->setVerticalSpacing(5);
        spriteGrid->addWidget(spriteLabelIcon,      0, 0, Qt::AlignCenter);
        spriteGrid->addWidget(spriteLabelFootprint, 0, 1, Qt::AlignCenter);
        spriteGrid->addWidget(spriteLabelFront,     2, 0, Qt::AlignCenter);
        spriteGrid->addWidget(spriteLabelBack,      2, 1, Qt::AlignCenter);
        spriteGrid->addWidget(new QLabel("Icon", this),      1, 0, Qt::AlignCenter);
        spriteGrid->addWidget(new QLabel("Footprint", this), 1, 1, Qt::AlignCenter);
        spriteGrid->addWidget(new QLabel("Front", this),     3, 0, Qt::AlignCenter);
        spriteGrid->addWidget(new QLabel("Back", this),      3, 1, Qt::AlignCenter);

        leftLayout->addLayout(spriteGrid);

        // Shiny toggle radio buttons
        normalRadio = new QRadioButton("Normal", this);
        shinyRadio  = new QRadioButton("Shiny", this);
        normalRadio->setChecked(true);

        QHBoxLayout *variantToggle = new QHBoxLayout;
        variantToggle->setAlignment(Qt::AlignCenter);
        variantToggle->addWidget(normalRadio);
        variantToggle->addWidget(shinyRadio);
        leftLayout->addLayout(variantToggle);

        connect(normalRadio, &QRadioButton::toggled, this, &PokemonTab::updateSpriteVariant);
        connect(shinyRadio,  &QRadioButton::toggled, this, &PokemonTab::updateSpriteVariant);
    }

    // Base Stats
    {
        QLabel *h = new QLabel("Base Stats", this);
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("font-weight:bold; font-size:14px; padding-top:10px;");
        leftLayout->addWidget(h);
        leftLayout->addSpacing(8);

        QGridLayout *statsGrid = new QGridLayout;
        statsGrid->setHorizontalSpacing(20);
        statsGrid->addWidget(new QLabel("HP", this),       0, 0);
        statsGrid->addWidget(hpSpin,                       0, 1);
        statsGrid->addWidget(new QLabel("Attack", this),   1, 0);
        statsGrid->addWidget(atkSpin,                      1, 1);
        statsGrid->addWidget(new QLabel("Defense", this),  2, 0);
        statsGrid->addWidget(defSpin,                      2, 1);
        statsGrid->addWidget(new QLabel("Sp. Atk", this),  0, 2);
        statsGrid->addWidget(spaSpin,                      0, 3);
        statsGrid->addWidget(new QLabel("Sp. Def", this),  1, 2);
        statsGrid->addWidget(spdSpin,                      1, 3);
        statsGrid->addWidget(new QLabel("Speed", this),    2, 2);
        statsGrid->addWidget(speSpin,                      2, 3);

        leftLayout->addLayout(statsGrid);
    }

    // Typing
    {
        QLabel *h = new QLabel("Typing", this);
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("font-weight:bold; font-size:14px; padding-top:10px;");
        leftLayout->addWidget(h);
        leftLayout->addSpacing(8);

        QHBoxLayout *typingRow = new QHBoxLayout;
        typingRow->setAlignment(Qt::AlignCenter);
        typingRow->setSpacing(10);
        typingRow->addWidget(new QLabel("Type 1:", this));
        typingRow->addWidget(type1Combo);
        typingRow->addWidget(new QLabel("Type 2:", this));
        typingRow->addWidget(type2Combo);

        leftLayout->addLayout(typingRow);
    }

    // Abilities
    {
        QLabel *h = new QLabel("Abilities", this);
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("font-weight:bold; font-size:14px; padding-top:10px;");
        leftLayout->addWidget(h);
        leftLayout->addSpacing(8);

        QHBoxLayout *abRow1 = new QHBoxLayout;
        abRow1->setAlignment(Qt::AlignCenter);
        abRow1->setSpacing(20);
        abRow1->addWidget(new QLabel("Ability 1:", this));
        abRow1->addWidget(ability1Combo);
        abRow1->addWidget(new QLabel("Ability 2:", this));
        abRow1->addWidget(ability2Combo);
        leftLayout->addLayout(abRow1);

        leftLayout->addSpacing(6);

        QHBoxLayout *abRow2 = new QHBoxLayout;
        abRow2->setAlignment(Qt::AlignCenter);
        abRow2->setSpacing(20);
        abRow2->addWidget(new QLabel("Hidden Ability:", this));
        abRow2->addWidget(abilityHiddenCombo);
        leftLayout->addLayout(abRow2);
    }

    leftLayout->addStretch();
    root->addWidget(leftWidget, 2);

    // ─── Right Column (Details) ────────────────────────────
    QWidget *rightWidget = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(10, 10, 10, 0);

    // Project path
    rightLayout->addWidget(pathLabel);
    rightLayout->addSpacing(8);

    // Pokémon Name Field
    {
        QLabel *nameLabel = new QLabel("Pokémon Name:", this);
        nameField = new QLineEdit(this);
        nameField->setReadOnly(false);
        connect(nameField, &QLineEdit::editingFinished,
                this, &PokemonTab::onNameEdited);
        rightLayout->addWidget(nameLabel);
        rightLayout->addWidget(nameField);
        rightLayout->addSpacing(8);
    }

    // Gender
    {
        QLabel *h = new QLabel("Gender", this);
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("font-weight:bold; font-size:14px; padding-top:10px;");
        rightLayout->addWidget(h);
        rightLayout->addSpacing(8);

        QHBoxLayout *r1 = new QHBoxLayout;
        r1->setAlignment(Qt::AlignCenter);
        r1->setSpacing(10);
        r1->addWidget(new QLabel("♀ Female", this));
        r1->addWidget(genderSlider);
        r1->addWidget(new QLabel("♂ Male", this));
        rightLayout->addLayout(r1);

        QHBoxLayout *r2 = new QHBoxLayout;
        r2->setAlignment(Qt::AlignCenter);
        r2->setSpacing(6);
        r2->addWidget(new QLabel("Gender Ratio:", this));
        r2->addWidget(genderDecimalField);
        r2->addWidget(new QLabel("% Female", this));
        rightLayout->addLayout(r2);
    }

    // Breeding
    {
        QLabel *h = new QLabel("Breeding", this);
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("font-weight:bold; font-size:14px; padding-top:10px;");
        rightLayout->addWidget(h);
        rightLayout->addSpacing(8);

        QGridLayout *g = new QGridLayout;
        g->setHorizontalSpacing(20);
        g->setVerticalSpacing(8);
        g->addWidget(new QLabel("Egg Group 1:", this), 0, 0, Qt::AlignRight);
        g->addWidget(egg1Combo,                        0, 1);
        g->addWidget(new QLabel("Egg Group 2:", this), 0, 2, Qt::AlignRight);
        g->addWidget(egg2Combo,                        0, 3);
        g->addWidget(new QLabel("Egg Cycles:", this),  1, 0, Qt::AlignRight);
        g->addWidget(eggCycleSpin,                     1, 1);
        g->addWidget(new QLabel("Steps:", this),       1, 2, Qt::AlignRight);
        g->addWidget(stepsField,                       1, 3);
        rightLayout->addLayout(g);
    }

    // Misc
    {
        QLabel *h = new QLabel("Misc", this);
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("font-weight:bold; font-size:14px; padding-top:10px;");
        rightLayout->addWidget(h);
        rightLayout->addSpacing(8);

        QGridLayout *m = new QGridLayout;
        m->setHorizontalSpacing(20);
        m->setVerticalSpacing(8);
        m->addWidget(new QLabel("Growth Rate:", this),   0, 0, Qt::AlignRight);
        m->addWidget(growthCombo,                        0, 1);
        m->addWidget(new QLabel("Catch Rate:", this),    0, 2, Qt::AlignRight);
        m->addWidget(catchRateSpin,                      0, 3);
        m->addWidget(new QLabel("Base EXP:", this),      1, 0, Qt::AlignRight);
        m->addWidget(expYieldSpin,                       1, 1);
        m->addWidget(new QLabel("Friendship:", this),    1, 2, Qt::AlignRight);
        m->addWidget(friendshipSpin,                     1, 3);
        rightLayout->addLayout(m);
    }

    rightLayout->addStretch();
    root->addWidget(rightWidget, 1);

    // Initial population
    loadSpeciesList();
}

//------------------------------------------------------------------------------
// Slots & Logic
//------------------------------------------------------------------------------

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

    currentSpecies = speciesMacroList.at(index - 1);

    PokemonInfo info;
    if (loadSpeciesInfo(currentSpecies, info)) {
        populateUI(info);
        loadSprites(currentSpecies);
    }
}

void PokemonTab::onNameEdited()
{
    QString newName = nameField->text().trimmed();
    if (newName.isEmpty() || currentSpecies.isEmpty())
        return;

    if (!saveSpeciesName(currentSpecies, newName)) {
        // handle error
    }
}

bool PokemonTab::saveSpeciesName(const QString &speciesMacro,
                                 const QString &newName)
{
    const QString filePath = path + "/src/data/text/species_names.h";
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList lines;
    QTextStream in(&f);
    while (!in.atEnd())
        lines << in.readLine();
    f.close();

    QRegularExpression rx(
        "\\[SPECIES_" + QRegularExpression::escape(speciesMacro) +
        "\\]\\s*=\\s+_\\(\"([^\"]*)\"\\)");

    bool replaced = false;
    for (int i = 0; i < lines.size(); ++i) {
        auto m = rx.match(lines[i]);
        if (m.hasMatch()) {
            QString prefix = lines[i].section("_(\"", 0, 0) + "_(\"";
            lines[i] = prefix + newName + "\"),";
            replaced = true;
            break;
        }
    }

    if (!replaced)
        return false;

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    QTextStream out(&f);
    for (const QString &L : lines)
        out << L << "\n";
    f.close();
    return true;
}

void PokemonTab::populateUI(const PokemonInfo &i)
{
    // Name
    nameField->setText(i.displayName);

    // Stats
    hpSpin ->setValue(i.baseHP);
    atkSpin->setValue(i.baseAttack);
    defSpin->setValue(i.baseDefense);
    spaSpin->setValue(i.baseSpAttack);
    spdSpin->setValue(i.baseSpDefense);
    speSpin->setValue(i.baseSpeed);

    // Typing
    type1Combo->setCurrentText(i.type1);
    type2Combo->setCurrentText(i.type2);

    // Abilities
    ability1Combo     ->setCurrentText(i.ability1);
    ability2Combo     ->setCurrentText(i.ability2);
    abilityHiddenCombo->setCurrentText(i.abilityHidden);

    // Growth / Gender
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

    // Breeding
    egg1Combo->setCurrentText(i.eggGroup1);
    egg2Combo->setCurrentText(i.eggGroup2);

    // Catch / EXP / Friendship
    catchRateSpin ->setValue(i.catchRate);
    expYieldSpin  ->setValue(i.expYield);
    friendshipSpin->setValue(i.friendship);

    // Egg cycles & steps
    eggCycleSpin->setValue(i.eggCycles);
    stepsField->setText(QString::number(i.eggCycles * 256));
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
        if (c == ' ')
            capNext = true;
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
        QTextStream in2(&f2);
        QRegularExpression rx("#define\\s+ABILITY_([A-Z_]+)\\s+\\d+");
        while (!in2.atEnd()) {
            QString line = in2.readLine();
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
    f.close();
}

bool PokemonTab::loadSpeciesInfo(const QString &speciesMacro, PokemonInfo &out)
{
    // — load the in-game name
    const QString nameFile = path + "/src/data/text/species_names.h";
    QFile fName(nameFile);
    if (fName.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fName);

        QRegularExpression rxName(
          "\\[SPECIES_" + QRegularExpression::escape(speciesMacro) +
          "\\]\\s*=\\s+_\\(\"([^\"]+)\"\\)");

        while (!in.atEnd()) {
            QString line = in.readLine();
            auto m = rxName.match(line);
            if (m.hasMatch()) {
                out.displayName = m.captured(1);
                break;
            }
        }
        fName.close();
    }

    // Now parse species_info
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
        if (!grab) {
            if (line.contains(header)) grab = true;
            continue;
        }
        block += line + '\n';
        if (line.contains('{')) ++braces;
        if (line.contains('}')) { --braces; if (!braces) break; }
    }
    f.close();
    if (block.isEmpty()) return false;

    auto getInt = [&](const QString &k, int &dst) {
        QRegularExpression rx("\\." + k + "\\s*=\\s*(\\d+)");
        if (auto m = rx.match(block); m.hasMatch())
            dst = m.captured(1).toInt();
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
    getInt("eggCycles",    out.eggCycles);

    if (auto m = QRegularExpression(
            R"(.types\s*=\s*\{TYPE_([A-Z_]+)\s*,\s*TYPE_([A-Z_]+)\})")
            .match(block); m.hasMatch())
    {
        out.type1 = typeMap.value("TYPE_" + m.captured(1),
                                  prettify(m.captured(1)));
        out.type2 = typeMap.value("TYPE_" + m.captured(2),
                                  prettify(m.captured(2)));
    }

    if (auto m = QRegularExpression(
            R"(.abilities\s*=\s*\{ABILITY_([A-Z_]+)\s*,\s*ABILITY_([A-Z_]+)(?:\s*,\s*ABILITY_([A-Z_]+))?\})")
            .match(block); m.hasMatch())
    {
        out.ability1 = abilityMap.value("ABILITY_" + m.captured(1),
                                        prettify(m.captured(1)));
        out.ability2 = abilityMap.value("ABILITY_" + m.captured(2),
                                        prettify(m.captured(2)));
        out.abilityHidden = m.captured(3).isEmpty()
                           ? "-"
                           : abilityMap.value("ABILITY_" + m.captured(3),
                                              prettify(m.captured(3)));
    }

    if (auto m = QRegularExpression(
            R"(.eggGroups\s*=\s*\{EGG_GROUP_([A-Z_]+)\s*,\s*EGG_GROUP_([A-Z_]+)\})")
            .match(block); m.hasMatch())
    {
        out.eggGroup1 = eggMap.value("EGG_GROUP_" + m.captured(1),
                                     prettify(m.captured(1)));
        out.eggGroup2 = eggMap.value("EGG_GROUP_" + m.captured(2),
                                     prettify(m.captured(2)));
    }

    if (auto m = QRegularExpression(
            R"(.growthRate\s*=\s*GROWTH_([A-Z_]+))")
            .match(block); m.hasMatch())
    {
        out.growthRate = growthMap.value("GROWTH_" + m.captured(1),
                                         prettify(m.captured(1)));
    }

    if (auto m = QRegularExpression(
            R"(.genderRatio\s*=\s*([A-Z_()%\.0-9]+))")
            .match(block); m.hasMatch())
    {
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

//------------------------------------------------------------------------------
// Original PNG loader overridden by palette when available
//------------------------------------------------------------------------------

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
        spriteLabelFront->setPixmap(pix.scaled(
            spriteLabelFront->size(), Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    } else {
        spriteLabelFront->clear();
    }

    if (QFile::exists(backPath)) {
        QPixmap pix(backPath);
        spriteLabelBack->setPixmap(pix.scaled(
            spriteLabelBack->size(), Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    } else {
        spriteLabelBack->clear();
    }

    if (QFile::exists(iconPath)) {
        QPixmap pix(iconPath);
        spriteLabelIcon->setPixmap(pix.scaled(
            spriteLabelIcon->size(), Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    } else {
        spriteLabelIcon->clear();
    }

    if (QFile::exists(footprintPath)) {
        QPixmap pix(footprintPath);
        spriteLabelFootprint->setPixmap(pix.scaled(
            spriteLabelFootprint->size(), Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    } else {
        spriteLabelFootprint->clear();
    }

    // override front.png if .4bpp + palette exist
    updateSpriteVariant();
}

//------------------------------------------------------------------------------
// Update front sprite using 4bpp + palette, aligned to PNG dimensions
//------------------------------------------------------------------------------

void PokemonTab::updateSpriteVariant()
{
    if (currentSpecies.isEmpty()) return;

    // base folder path
    QString base = path + "/graphics/pokemon/" + currentSpecies.toLower() + "/";
    QVector<QColor> palette =
        parsePalFile(base +
                     (normalRadio->isChecked() ? "normal.pal" : "shiny.pal"));

    // loop over front and back
    for (const QString &side : { "front", "back" }) {
        QLabel *lbl = (side == "front")
                      ? spriteLabelFront
                      : spriteLabelBack;

        // try raw 4bpp data
        QString gfxPath = base + side + ".4bpp";
        QByteArray gfx  = readFile(gfxPath);

        // fallback to PNG if missing
        if (gfx.isEmpty() || palette.isEmpty()) {
            QString pngPath = base + side + ".png";
            if (QFile::exists(pngPath)) {
                QPixmap px(pngPath);
                lbl->setPixmap(px.scaled(
                    lbl->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
            } else {
                lbl->clear();
            }
            continue;
        }

        // render aligned to the PNG’s dimensions
        QImage img = render4bppSprite(gfx,
                                      palette,
                                      base + side + ".png");
        if (img.isNull()) {
            lbl->clear();
        } else {
            lbl->setPixmap(QPixmap::fromImage(
                img.scaled(
                    lbl->size(),
                    Qt::KeepAspectRatio,
                    Qt::FastTransformation)));
        }
    }
}

//------------------------------------------------------------------------------
// File I/O & palette parsing
//------------------------------------------------------------------------------

QByteArray PokemonTab::readFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return {};
    return f.readAll();
}

QVector<QColor> PokemonTab::parsePalFile(const QString &palPath)
{
    QVector<QColor> palette;
    QFile file(palPath);
    if (!file.open(QIODevice::ReadOnly)) return palette;

    QByteArray data = file.readAll();
    if (!data.startsWith("JASC-PAL")) return palette;

    auto lines = data.split('\n');
    for (int i = 3; i < lines.size(); ++i) {
        auto parts = lines[i].trimmed().split(' ');
        if (parts.size() == 3) {
            palette.append(QColor(parts[0].toInt(),
                                  parts[1].toInt(),
                                  parts[2].toInt()));
        }
    }
    return palette;
}

//------------------------------------------------------------------------------
// Render 4bpp data using PNG dimensions for exact alignment
//------------------------------------------------------------------------------

QImage PokemonTab::render4bppSprite(const QByteArray &data,
                                    const QVector<QColor> &palette,
                                    const QString &pngPath)
{
    const int tileSize = 8;

    QImage ref(pngPath);
    if (ref.isNull()) return QImage();

    int spriteW     = ref.width();
    int spriteH     = ref.height();
    int tilesPerRow = spriteW / tileSize;
    int rows        = spriteH / tileSize;
    int tileCount   = data.size() / 32;

    if (tilesPerRow * rows != tileCount) {
        return QImage();
    }

    QImage img(spriteW, spriteH, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    for (int t = 0; t < tileCount; ++t) {
        int row = t / tilesPerRow;
        int col = t % tilesPerRow;
        for (int y = 0; y < tileSize; ++y) {
            for (int x = 0; x < tileSize; ++x) {
                int idx   = y * tileSize + x;
                int bIdx  = t * 32 + (idx / 2);
                quint8 b  = static_cast<quint8>(data[bIdx]);
                int ci    = (idx & 1) ? ((b & 0xF0) >> 4) : (b & 0x0F);
                if (ci > 0 && ci < palette.size()) {
                    img.setPixelColor(col*tileSize + x,
                                      row*tileSize + y,
                                      palette[ci]);
                }
            }
        }
    }

    return img;
}

//------------------------------------------------------------------------------
// Remaining original methods: populateUI, prettify, loadConstants,
// loadSpeciesList, loadSpeciesInfo
// These are unchanged from your original file.
//------------------------------------------------------------------------------