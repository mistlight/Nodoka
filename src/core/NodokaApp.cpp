//
// Created by mistlight on 1/2/2017.
//

#include "NodokaApp.h"

Core::NodokaApp::NodokaApp() {
    // initialize db backed models
    this->directoryModel = new Directory();
    this->audiobookFileModel = new AudiobookFile();
    this->audiobookModel = new Audiobook(this->audiobookFileModel);

    // initialize player, which will initialize vlc backend related items
    this->player = new Core::ConcretePlayer();

    // initialize the ui
    this->mainWindow = new MainWindow(this->directoryModel, this->audiobookModel, this->player);

    // initialize event handlers
    this->directoryHandler = new DirectoryHandler(this->audiobookModel, this->audiobookFileModel);

    this->setup();
}

void Core::NodokaApp::start() {
    mainWindow->show();
}

void Core::NodokaApp::setup() {
    // set up the listeners for the directory add/remove
    connect(this->directoryModel, &Directory::directoryAdded,
            this->directoryHandler, &DirectoryHandler::handleDirectoryAdded);
    connect(this->directoryModel, &Directory::directoryRemove,
            this->directoryHandler, &DirectoryHandler::handleDirectoryRemoved);
}
