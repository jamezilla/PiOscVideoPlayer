#pragma once

#include <atomic>
#include <map>
#include "ConsoleListener.h"
#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ofxOsc.h"
#include "Poco/RWLock.h"
#include "Poco/Thread.h"
#include "RingBuffer.h"

struct videoPlayerAppConfig {
    int           window_width;
    int           window_height;
    string        player_video_path;
    bool          player_enable_looping;
    bool          player_flip_texture;
    bool          player_full_screen;
    bool          player_hide_cursor;
    uint_fast16_t osc_local_port;
    string        osc_local_host;
    uint_fast16_t osc_remote_port;
    string        osc_remote_host;
};

struct loaderParams {
    ofFile file;
    float x;
    float y;
    float rx;
    float ry;
};

class videoPlayerApp
    : public ofBaseApp,
      public ofxOMXPlayerListener,
      public SSHKeyListener,
      public Poco::Runnable
{
    typedef ofPtr<loaderParams> loaderParamsPtr;
    typedef ofPtr<ofxOMXPlayer> ofxOMXPlayerPtr;
    typedef RingBuffer<loaderParamsPtr, 2> LoaderQueue;

public:

    videoPlayerApp(videoPlayerAppConfig _config)
        : config(_config),
          screen_blanked(true),
          debug(false),
          hide_cursor(true),
          x(0),
          y(0),
          rx(0),
          ry(0)
    {};

    void blankScreen();
    void draw();
    void keyPressed(int key);
    void loadMovie(std::string file_name, float x = 0, float y = 0, float rx = 0, float ry = 0);
    void onVideoEnd(ofxOMXPlayerListenerEventData& e);
    void onVideoLoop(ofxOMXPlayerListenerEventData& e){ /*empty*/ };
    void onCharacterReceived(SSHKeyListenerEventData& e);
    void run();
    void setCursorHidden(bool is_hidden);
    void setup();
    void update();

    videoPlayerAppConfig    config;
    atomic<bool>            screen_blanked;
    bool                    debug;
    bool                    hide_cursor;
    ofxOMXPlayerPtr         front_player;
    ofxOMXPlayerPtr         back_player;
    std::map<std::string, ofFile> files;
    LoaderQueue             loader_queue;
    ConsoleListener         consoleListener;
    Poco::Thread            loader;
    Poco::RWLock            front_lock;

    float x;
    float y;
    float rx;
    float ry;

    ofxOscReceiver          osc_receiver;
    //ofxOscSender            osc_sender;
};
