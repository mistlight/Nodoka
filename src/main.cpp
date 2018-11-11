#include <QApplication>
#include <QWidget>
#include <src/core/DatabaseConnect.h>
#include <src/core/ConcretePlayer.h>
#include <QtWidgets/QMessageBox>
#include <src/core/NodokaApp.h>
#include <src/simple-lib/RunGuard.h>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setOrganizationName("Otakukingdom Co");
    QApplication::setOrganizationDomain("nodoka.otakukingdom.com");
    QApplication::setApplicationName("Nodoka");

    RunGuard guard("Z0DWjf33Am1YeCUdIW7h0vSxjU2RJjZcUqzgG ver0.0.2a");


    if(!guard.tryToRun()) {
        new QMessageBox();
        QMessageBox::critical(nullptr,
                "Error", "Cannot launch multiple instances of Nodoka Player");

        return EXIT_FAILURE;
    }



    if(!Core::openDb()) {
        new QMessageBox();
        QMessageBox::critical(nullptr,
                "Error", "Failed to load the config file");

        // since we failed to load the db, we shouldn't continue
        return EXIT_FAILURE;
    } else {
        Core::NodokaApp nodokaApp(&app);

        nodokaApp.start();
    }

    return QApplication::exec();
}
