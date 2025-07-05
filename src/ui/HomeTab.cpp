#include "HomeTab.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QSize>
#include <QTabWidget>
#include <QMainWindow>

HomeTab::HomeTab(QWidget *parent) : QWidget(parent) {
    // Outer layout: gives top-left alignment with margins
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(20, 20, 20, 20);
    outerLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Layout for the button and its label
    QVBoxLayout *buttonWithLabelLayout = new QVBoxLayout();

    QPushButton *squareButton = new QPushButton(this);
    squareButton->setFixedSize(100, 100);
    squareButton->setIcon(QIcon(":/resources/bulbasaur.png"));
    squareButton->setIconSize(QSize(64, 64));
    squareButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ccc;
            border: 2px solid #555;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #ddd;
        }
        QPushButton:pressed {
            background-color: #bbb;
        }
    )");

    // Connect button to switch to Pokémon tab
    connect(squareButton, &QPushButton::clicked, this, [=]() {
        if (auto *mw = qobject_cast<QMainWindow *>(window())) {
            if (auto *tabs = mw->findChild<QTabWidget *>()) {
                tabs->setCurrentIndex(1); // Pokémon tab index
            }
        }
    });

    QLabel *pokemonLabel = new QLabel("Pokémon", this);
    pokemonLabel->setStyleSheet("font-size: 14px;");
    pokemonLabel->setAlignment(Qt::AlignHCenter); // Center under button

    // Add button and label to layout with specific alignment
    buttonWithLabelLayout->addWidget(squareButton, 0, Qt::AlignLeft);
    buttonWithLabelLayout->addWidget(pokemonLabel, 0, Qt::AlignHCenter);

    // Add the grouped layout to the main one
    outerLayout->addLayout(buttonWithLabelLayout);
}
