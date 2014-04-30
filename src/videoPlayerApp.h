#pragma once

#include <atomic>
#include "ConsoleListener.h"
#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ofxOsc.h"
#include "Poco/RWLock.h"
#include "Poco/ThreadPool.h"

struct videoPlayerAppConfig {
    int           window_width;
    int           window_height;
    string        player_video_path;
    bool          player_enable_looping;
    bool          player_flip_texture;
    uint_fast16_t osc_local_port;
    string        osc_local_host;
    uint_fast16_t osc_remote_port;
    string        osc_remote_host;
};

class videoPlayerApp
    : public ofBaseApp,
      public ofxOMXPlayerListener,
      public SSHKeyListener,
      public Poco::Runnable
{
    typedef ofPtr<ofxOMXPlayer> ofxOMXPlayerPtr;

public:

    videoPlayerApp(videoPlayerAppConfig _config)
        : config(_config),
          screen_blanked(true),
          debug(false),
          front_index(0),
          back_index(0),
          pool(1)
    {};

    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void onVideoEnd(ofxOMXPlayerListenerEventData& e);
    void onVideoLoop(ofxOMXPlayerListenerEventData& e){ /*empty*/ };
    void onCharacterReceived(SSHKeyListenerEventData& e);
    void loadMovie(uint_fast8_t index);
    void blankScreen();
    void run();

    videoPlayerAppConfig  config;
    atomic<bool>            screen_blanked;
    bool                    debug;
    ofxOMXPlayerPtr         front_player;
    atomic<uint_fast8_t>    front_index;
    ofxOMXPlayerPtr         back_player;
    atomic<uint_fast8_t>    back_index;
    vector<ofFile>          files;
    ConsoleListener         consoleListener;
    Poco::ThreadPool        pool;
    Poco::RWLock            front_lock;
    Poco::RWLock            back_lock;


    ofxOscReceiver          osc_receiver;
    //ofxOscSender            osc_sender;
};
