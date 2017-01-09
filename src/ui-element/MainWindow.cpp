//
// Created by mistlight on 12/22/2016.
//

#include "MainWindow.h"

const static int MAXIMUM_VOLUME = 150;

MainWindow::MainWindow(Directory* directoryModel,
                       Audiobook* audiobookModel,
                       Core::ConcretePlayer* player,
                       Core::Setting* setting,
                       QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow()) {
    ui->setupUi( this );
    this->setIsPlaying(false);

    this->settings = setting;

    // we will need this reference so FileList can make direct reference to it
    this->concretePlayer = player;

    // a hack so the menu shows up on mac
    ui->menubar->setNativeMenuBar(false);

    // set the model
    this->directoryModel = directoryModel;
    this->audiobookModel = audiobookModel;
    this->fileDisplayModel = new FileDisplayModel(this);

    // initialize the settings form
    this->settingsForm = new SettingsForm(this->directoryModel);

    this->setup();
}

void MainWindow::setup() {
    this->setWindowTitle("Nodoka");

    connect(this->ui->actionExit, &QAction::triggered, this, &MainWindow::performExit);
    connect(this->ui->actionSettings, &QAction::triggered, this, &MainWindow::performSettings);

    // set up the audobook view
    this->ui->audiobookView->setModel(this->audiobookModel);


    // connect the audiobook view events to the file selector view
    auto audiobookModel = this->audiobookModel;
    connect(this->ui->audiobookView->selectionModel(), &QItemSelectionModel::selectionChanged,
            [this, audiobookModel] (const QItemSelection &selected, const QItemSelection &deselected) {
                if(selected.indexes().size() > 0) {
                    // set the audiobook file list
                    auto modelIndex = selected.indexes().first();
                    auto record = audiobookModel->record(modelIndex.row());
                    int audiobookId = record.value("id").toInt();

                    this->fileDisplayModel->setSelectedAudiobook(audiobookId);

                    // set the selected audiobook file if it exists
                    QSqlQuery query;
                    query.prepare("SELECT id, selected_file from audiobooks WHERE id=?");
                    query.addBindValue(audiobookId);
                    if(!query.exec()) {
                        // if we are here, something went wrong while the query was executing
                        auto error = query.lastError();
                        qWarning() << "Something went wrong: "
                                   << error.driverText()
                                   << ", " << error.databaseText();
                    } else {
                        if(query.next()) {
                            auto currentRecord = query.record();
                            if(!currentRecord.value("selected_file").isNull()) {
                                auto path = currentRecord.value("selected_file").toString();
                                auto index =
                                        reinterpret_cast<FileDisplayModel*>(this->ui->fileView->model())->getFileIndex(path);

                                this->ui->fileView->selectionModel()->select(index, QItemSelectionModel::Select);

                                return;
                            }
                        }
                    }

                }
            });

    this->ui->fileView->setModel(this->fileDisplayModel);

    // set up the event handler for file view
    connect(this->ui->fileView->selectionModel(), &QItemSelectionModel::selectionChanged,
            [this] (const QItemSelection &selected, const QItemSelection &deselected) {
                if(selected.indexes().size() > 0) {
                    if(this->currentlyPlayingFile.getNullState()) {
                        auto modelIndex = selected.indexes().first();
                        auto model = reinterpret_cast<FileDisplayModel*>(this->ui->fileView->model());
                        auto record = model->record(modelIndex.row());

                        this->concretePlayer->loadMedia(record);
                        this->setCurrentlyPlayingFile(*this->concretePlayer->getAudiobookFile());
                    }
                }
            });

    // connect file selector view to concrete player
    connect(this->ui->fileView, &QListView::doubleClicked,
            [this] (const QModelIndex &index) {
                QSqlTableModel* model = (QSqlTableModel *) this->ui->fileView->model();
                auto row = index.row();

                auto currentRecord = model->record(row);

                this->concretePlayer->loadMedia(currentRecord);
                this->concretePlayer->play();
    });

    // prevent editing of audiobook and file list view
    this->ui->audiobookView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->fileView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // slider interaction is disabled by default
    this->ui->progressSlider->setEnabled(false);
    this->ui->progressSlider->setTracking(false);

    // define what the play button do
    connect(this->ui->playButton, &QPushButton::clicked, [=]() {
        if(this->isPlaying) {
            this->concretePlayer->stop();
        } else {
            this->concretePlayer->play();
        }
    });

    // connect the changes from the progress slider to the player
    connect(this->ui->progressSlider, &QSlider::sliderMoved,
            concretePlayer, &Core::ConcretePlayer::updateSeekPosition);

    connect(this->ui->progressSlider, &QSlider::sliderPressed,
            [=]() {
                this->concretePlayer->stop();
            });

    connect(this->ui->progressSlider, &QSlider::sliderReleased,
            [=]() {
                this->concretePlayer->play();
            });

    // set up the volume controls
    this->ui->volumeSlider->setMaximum(MAXIMUM_VOLUME);
    this->ui->volumeSlider->setValue(settings->getVolume());

    connect(this->ui->volumeSlider, &QSlider::sliderMoved,
            this->settings, &Core::Setting::setVolume);

    loadCurrentAudiobookIfExists();
}


MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::performExit() {
    this->close();
}

void MainWindow::performSettings() {
    this->settingsForm->setWindowModality(Qt::WindowModality::ApplicationModal);
    this->settingsForm->show();
}

void MainWindow::setIsPlaying(bool isPlaying) {
    this->isPlaying = isPlaying;

    if(!isPlaying) {
        QIcon playIcon(":/icons/play.png");
        this->ui->playButton->setIcon(playIcon);
    } else {
        QIcon playIcon(":/icons/pause.png");
        this->ui->playButton->setIcon(playIcon);
    }
}

void MainWindow::playerStateUpdated(AudiobookFileProxy abFile, bool isPlaying) {
    this->setCurrentlyPlayingFile(abFile);
    this->setIsPlaying(isPlaying);
}

void MainWindow::playerTimeUpdated(AudiobookFileProxy abFile, long long currentTime) {
    this->setCurrentTime(currentTime);
}

void MainWindow::setCurrentlyPlayingFile(AudiobookFileProxy file) {
    this->currentlyPlayingFile = file;

    if(this->currentlyPlayingFile.getNullState() == false) {
        QString text = "Currently Playing: " + this->currentlyPlayingFile.name();
        this->ui->currentlyPlayingLabel->setText(text);
    }

    // set the slider max value if we have a parsed duration
    if(this->currentlyPlayingFile.isPropertyParsed()) {
        long long totalDuration = this->currentlyPlayingFile.getMediaDuration();
        this->ui->progressSlider->setMaximum(static_cast<int>(totalDuration));

        // enable the slider
        this->ui->progressSlider->setEnabled(true);
    }
}

void MainWindow::setCurrentTime(long long currentTime) {
    this->currentTime = currentTime;

    // update the progress slider
    this->ui->progressSlider->setValue(static_cast<int>(currentTime));

    // update the label
    QTime time(0, 0);
    time = time.addMSecs(static_cast<int>(currentTime));
    QString timeInFormat = time.toString("hh:mm:ss");

    this->ui->timeLabel->setText(timeInFormat);
}

// if there is an update with the AudiobookFile state, the Proxy file will be updated
void MainWindow::audiobookFileStateUpdated(AudiobookFileProxy abFile) {
    this->setCurrentlyPlayingFile(abFile);
}

void MainWindow::loadCurrentAudiobookIfExists() {
    auto audiobookId = this->settings->getCurrentAudiobookId();
    if(!audiobookId != -1) {
        Audiobook* currentModel = reinterpret_cast<Audiobook*>(this->ui->audiobookView->model());
        for(int i = 0; i < currentModel->rowCount(); i++) {
            if(currentModel->record(i).value("id").toInt() == audiobookId) {
                QModelIndex currentIndex = currentModel->index(i, 0);
                auto selectionModel = this->ui->audiobookView->selectionModel();
                selectionModel->select(currentIndex, QItemSelectionModel::Select);
            }
        }

    }
}

