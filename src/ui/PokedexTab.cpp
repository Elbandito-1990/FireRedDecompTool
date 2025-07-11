#include "PokedexTab.h"
#include "Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

PokedexTab::PokedexTab(const QString &projectPath, QWidget *parent)
    : QWidget(parent), projectPath(projectPath) {
    setupUI();
    loadSpeciesMetadata();
}

void PokedexTab::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);

    auto *headerLayout = new QHBoxLayout();
    speciesDropdown = new QComboBox(this);
    connect(speciesDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PokedexTab::onSpeciesSelected);

    categoryLabel = new QLabel("Category: —", this);
    headerLayout->addWidget(speciesDropdown);
    headerLayout->addWidget(categoryLabel);
    mainLayout->addLayout(headerLayout);

    auto *statsLayout = new QHBoxLayout();
    heightLabel = new QLabel("Height: —", this);
    weightLabel = new QLabel("Weight: —", this);
    statsLayout->addWidget(heightLabel);
    statsLayout->addWidget(weightLabel);
    mainLayout->addLayout(statsLayout);

    fireRedTextEdit = new QTextEdit(this);
    leafGreenTextEdit = new QTextEdit(this);
    fireRedTextEdit->setReadOnly(true);
    leafGreenTextEdit->setReadOnly(true);

    auto *fireBox = new QGroupBox("🔥 FireRed", this);
    auto *lgBox = new QGroupBox("🍃 LeafGreen", this);
    fireBox->setLayout(new QVBoxLayout);
    lgBox->setLayout(new QVBoxLayout);
    fireBox->layout()->addWidget(fireRedTextEdit);
    lgBox->layout()->addWidget(leafGreenTextEdit);

    mainLayout->addWidget(fireBox);
    mainLayout->addWidget(lgBox);
}

QString PokedexTab::formatSpeciesName(const QString &raw) {
    QString name = raw.toLower().replace('_', ' ');
    if (!name.isEmpty())
        name[0] = name[0].toUpper();
    return name;
}

void PokedexTab::loadSpeciesMetadata() {
    QFile entriesFile(projectPath + "/src/data/pokemon/pokedex_entries.h");

    if (!entriesFile.open(QIODevice::ReadOnly)) {
        logMessage("❌ Failed to open dex entries file.");
        return;
    }

    QTextStream in(&entriesFile);
    in.setCodec("UTF-8"); // 🔥 Ensure correct character decoding
    QString currentSpeciesName;
    DexInfo currentInfo;

    QRegularExpression speciesRx(R"(\[NATIONAL_DEX_(.+?)\])");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        auto match = speciesRx.match(line);
        if (match.hasMatch()) {
            if (!currentSpeciesName.isEmpty()) {
                dexData[currentSpeciesName] = currentInfo;
                speciesDropdown->addItem(currentSpeciesName);
                logMessage("✅ Loaded species: " + currentSpeciesName);
            }

            currentSpeciesName = formatSpeciesName(match.captured(1));
            currentInfo = DexInfo{};
            continue;
        }

        if (line.contains(".categoryName")) {
            QRegularExpression rx("\"([^\"]+)\"");
            auto match = rx.match(line);
            if (match.hasMatch()) currentInfo.category = match.captured(1);
        } else if (line.contains(".height")) {
            currentInfo.heightM = line.section('=', 1).remove(',').trimmed().toDouble() / 10.0;
        } else if (line.contains(".weight")) {
            currentInfo.weightKg = line.section('=', 1).remove(',').trimmed().toDouble() / 10.0;
        } else if (line.contains(".description")) {
            QRegularExpression rx(R"(=\s*(g\w+PokedexText))");
            auto match = rx.match(line);
            if (match.hasMatch()) currentInfo.descriptionKey = match.captured(1);
        }
    }

    if (!currentSpeciesName.isEmpty()) {
        dexData[currentSpeciesName] = currentInfo;
        speciesDropdown->addItem(currentSpeciesName);
        logMessage("✅ Loaded species: " + currentSpeciesName);
    }

    logMessage("✅ Species list loaded: " + QString::number(dexData.size()) + " entries");
}

QString PokedexTab::parseSingleDexEntry(const QString &filePath, const QString &descKey) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        logMessage("❌ Failed to open dex file: " + filePath);
        return "";
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8"); // ✅ Ensures accented characters like é are correct
    QString content = stream.readAll();

    QString pattern = "const u8\\s+" + QRegularExpression::escape(descKey) +
                      "\\[\\]\\s*=\\s*_\\s*\\(\\s*((?:\\s*\"[^\"]*\"\\s*)+)\\s*\\);";
    QRegularExpression entryRx(pattern, QRegularExpression::DotMatchesEverythingOption);

    auto match = entryRx.match(content);
    if (!match.hasMatch()) return "";

    QString rawText = match.captured(1);
    QStringList lines;
    QRegularExpression quoteRx("\"([^\"]*)\"");
    auto lineMatches = quoteRx.globalMatch(rawText);
    while (lineMatches.hasNext()) {
        auto lineMatch = lineMatches.next();
        lines << lineMatch.captured(1).replace("\\n", "\n");
    }

    return lines.join("\n").trimmed();
}

void PokedexTab::onSpeciesSelected(int index) {
    updateDexDisplay(speciesDropdown->itemText(index));
}

void PokedexTab::updateDexDisplay(const QString &speciesName) {
    if (!dexData.contains(speciesName)) return;
    const DexInfo &info = dexData[speciesName];

    categoryLabel->setText("Category: " + info.category);
    heightLabel->setText(QString("Height: %1 m").arg(info.heightM));
    weightLabel->setText(QString("Weight: %1 kg").arg(info.weightKg));

    QString frPath = projectPath + "/src/data/pokemon/pokedex_text_fr.h";
    QString lgPath = projectPath + "/src/data/pokemon/pokedex_text_lg.h";

    QString fireText = parseSingleDexEntry(frPath, info.descriptionKey);
    QString leafText = parseSingleDexEntry(lgPath, info.descriptionKey);

    fireRedTextEdit->setText(fireText.isEmpty() ? "[No FireRed text found]" : fireText);
    leafGreenTextEdit->setText(leafText.isEmpty() ? "[No LeafGreen text found]" : leafText);
}