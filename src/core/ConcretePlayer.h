//
// Created by mistlight on 12/22/2016.
//

#ifndef NODOKANATIVE_MEDIAPLAYER_H
#define NODOKANATIVE_MEDIAPLAYER_H

#include <memory>
#include <QObject>
#include <src/proxy-objects/AudiobookFileProxy.h>
#include <src/simple-lib/ThreadPool.h>
#include "vlc/vlc.h"
#include "Setting.h"
#include <QFile>
#include <src/proxy-objects/ProxyManager.h>
#include <include/vlc_input.h>

namespace Core {
    class ConcretePlayer : public QObject {
        Q_OBJECT

        int volume;

        // outside dependency
        Setting* setting;
        std::shared_ptr<ProxyManager> proxyManager;

        // state
        std::shared_ptr<AudiobookFileProxy> audiobookFileProxy;
        std::unique_ptr<QFile> currentFile;
        QString speed;
        bool mediaLoaded;
        QString currentPath;
        long long seekTo;
        bool hasSeekTo;
        bool autoPlay;

        // thread pool manager
        std::unique_ptr<ThreadPool> threadPool;

        // libvlc objects
        libvlc_instance_t* inst;
        libvlc_media_player_t* mediaPlayer;
        libvlc_media_t* mediaItem;
        libvlc_event_manager_t* mediaEventManager;
        libvlc_event_manager_t* playerEventManager;
        libvlc_state_t currentState;


        // set up the vlc callbacks
        void setupVLCCallbacks();

        // media item related callbacks
        void setupMediaCallbacks();

        // set up the callbacks that we are supposed to listen to
        void setupEventHandlers();

        static void handleFinished(const struct libvlc_event_t* event, void* data);

    public:
        ConcretePlayer(Setting* setting, std::shared_ptr<ProxyManager> manager);
        ~ConcretePlayer();
        std::shared_ptr<AudiobookFileProxy> getAudiobookFile();
        libvlc_state_t getCurrentState();
        libvlc_time_t getCurrentTime();
        void play();
        void playNextFile();
        void stop();
        void loadMedia(QSqlRecord record);
        void releaseMedia();
        void setVolume(int volume);
        void setSpeed(QString speed);
        long long getDurationInMs();
        double getDurationInSeconds();
        float getRate();

        bool canLoadMedia(QSqlRecord record);

    signals:
        void stateChanged(input_state_e newState);
        void timeProgressed(libvlc_time_t time);
        void parsedStatusChanged(bool isParsed);
        void currentFileFinished();

    public slots:
        void updateSeekPosition(long long position);

    };
}



#endif //NODOKANATIVE_MEDIAPLAYER_H
