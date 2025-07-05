#include <QMessageBox>
#include "StartupWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QLabel>

StartupWindow::StartupWindow(QWidget *parent) : QWidget(parent) {
    QLabel *label = new QLabel("Select the PokeFireRed decomp folder:", this);
    pathEdit = new QLineEdit(this);
    browseButton = new QPushButton("Browse", this);
    openButton = new QPushButton("Open", this);
    openButton->setEnabled(false);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(browseButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addLayout(pathLayout);
    mainLayout->addWidget(openButton);

    connect(browseButton, &QPushButton::clicked, this, &StartupWindow::browse);
    connect(openButton, &QPushButton::clicked, this, [this]() {
        QFile file("last_path.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << pathEdit->text();
            file.close();
        }
        emit pathSelected(pathEdit->text());
    });
    connect(pathEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        openButton->setEnabled(!text.trimmed().isEmpty());
    });


    setWindowTitle("Select Project");
    resize(720, 250);
    QFile file("last_path.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString savedPath = in.readLine().trimmed();
        file.close();
        if (!savedPath.isEmpty()) {
            pathEdit->setText(savedPath);
            openButton->setEnabled(true);
        }
    }
}

void StartupWindow::browse() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select FireRed Disassembly Folder");
    if (folderPath.isEmpty())
        return;

    QString makefilePath = folderPath + "/Makefile";
    if (!QFile::exists(makefilePath)) {
        QMessageBox::critical(this, "Error", "The selected folder does not contain a Makefile.");
        return;
    }

    QFile makefile(makefilePath);
    if (makefile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&makefile);
        QString contents = in.readAll();
        makefile.close();

        if (!contents.contains("firered", Qt::CaseInsensitive)) {
            QMessageBox::critical(this, "Error", "Makefile does not appear to be from a FireRed disassembly.");
            return;
        }
    } else {
        QMessageBox::critical(this, "Error", "Could not open the Makefile.");
        return;
    }

    pathEdit->setText(folderPath);
}


QString StartupWindow::getPath() const {
    return pathEdit->text();
}

void StartupWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap background(":/resources/background1.png");
    painter.drawPixmap(rect(), background);
}
