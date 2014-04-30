#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ConsoleListener.h"
#include "Poco/Mutex.h"

struct networkPlayerAppConfig {
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

class networkPlayerApp
    : public ofBaseApp,
      public ofxOMXPlayerListener,
      public SSHKeyListener,
      public Poco::Runnable
{

    typedef ofPtr<ofxOMXPlayer> ofxOMXPlayerPtr;

public:

    networkPlayerApp(networkPlayerAppConfig _config)
        : config(_config),
          screen_blanked(true),
          debug(false),
          video_count(0),
          front_video_index(0),
          back_video_index(0),
          swap_players(false)
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

    networkPlayerAppConfig  config;
    bool                    screen_blanked;
    bool                    debug;
    uint_fast8_t            video_count;

    ofxOMXPlayerPtr         front_player;
    uint_fast8_t            front_video_index;

    ofxOMXPlayerPtr         back_player;
    uint_fast8_t            back_video_index;

    vector<ofFile>          files;
    ConsoleListener         consoleListener;

    Poco::Thread            thread;
    Poco::FastMutex         mutex;
    bool                    swap_players;

};
