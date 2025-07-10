#include "MainWindow.h"
#include "ui/HomeTab.h"
#include "ui/PokemonTab.h"
#include "ui/PokedexTab.h"
#include <QIcon>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), tabWidget(new QTabWidget(this)) {
    setWindowIcon(QIcon("resources/Icon.ico"));
    setCentralWidget(tabWidget);
    setWindowTitle("FireRed Decomp Tool");
    resize(800, 600);

    tabWidget->addTab(new HomeTab(this), "Home");
    // Pokémon and Pokedex tabs will be added after project path is set
}

void MainWindow::setProjectPath(const QString &path) {
    projectPath = path;
}

void MainWindow::initializeTabs() {
    tabWidget->addTab(new PokemonTab(projectPath, this), "Pokémon");
    tabWidget->addTab(new PokedexTab(projectPath, this), "Pokedex");
}