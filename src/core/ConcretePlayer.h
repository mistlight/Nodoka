//
// Created by mistlight on 12/22/2016.
//

#ifndef NODOKANATIVE_MEDIAPLAYER_H
#define NODOKANATIVE_MEDIAPLAYER_H

#include <memory>
#include <QObject>
#include <src/model/AudiobookFileProxy.h>
#include "vlc/vlc.h"

namespace Core {
    class ConcretePlayer : public QObject {
        Q_OBJECT

        std::shared_ptr<AudiobookFileProxy> audiobookFileProxy;

        long long seekTo;
        bool hasSeekTo;

        libvlc_instance_t* inst;
        libvlc_media_player_t* mediaPlayer;
        libvlc_media_t* mediaItem;
        libvlc_event_manager_t* mediaEventManager;
        libvlc_event_manager_t* playerEventManager;
        libvlc_state_t currentState;

        bool mediaLoaded;
        QString currentPath;

        void setupCallbacks();

    public:
        std::shared_ptr<AudiobookFileProxy> getAudiobookFile();
        libvlc_state_t getCurrentState();
        libvlc_time_t getCurrentTime();
        void play();
        void stop();
        void loadMedia(QSqlRecord record);
        void releaseMedia();
        ConcretePlayer();
        long long getDurationInMs();
        double getDurationInSeconds();

    signals:
        void stateChanged(libvlc_state_t newState);
        void timeProgressed(libvlc_time_t time);
        void parsedStatusChanged(bool isParsed);

    public slots:
        void updateSeekPosition(long long position);
    };
}



#endif //NODOKANATIVE_MEDIAPLAYER_H
