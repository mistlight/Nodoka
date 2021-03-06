//
// Created by mistlight on 1/29/2017.
//

#include "PlayerScanTask.h"

Core::PlayerScanTask::PlayerScanTask(Core::ScanPlayer *player, std::shared_ptr<AudiobookProxy> audiobook) {

    this->player = player;
    this->audiobook = audiobook;
}

void Core::PlayerScanTask::run() {
    // if every file has already been scanned, skip this step
    if(audiobook->hasDuration() && audiobook->allFileDurationScanned()) {
        return;
    }

    this->player->performScan();

    if(this->audiobook != nullptr) {
        this->audiobook->handlePropertyScanFinished();
    }
}


