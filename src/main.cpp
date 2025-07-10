#include <QApplication>
#include "MainWindow.h"
#include "StartupWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    StartupWindow startup;
    MainWindow mainWin;

    QObject::connect(&startup, &StartupWindow::pathSelected, [&](const QString &path) {
        mainWin.setProjectPath(path);
        mainWin.setWindowTitle("FireRed Decomp Tool - " + path);

        // Add Pokémon tab after path is set
        mainWin.initializeTabs();

        mainWin.show();
        startup.close();
    });

    startup.show();
    return app.exec();
}
