#pragma once

#include <atomic>
#include "ConsoleListener.h"
#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ofxOsc.h"
#include "Poco/RWLock.h"
#include "Poco/ThreadPool.h"
#include "RingBuffer.h"

struct videoPlayerAppConfig {
    bool          window_full_screen;
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
    typedef RingBuffer<ofxOMXPlayerPtr, 8> VideoQueue;
    typedef RingBuffer<std::string, 8> VideoFileNameQueue;

public:

    videoPlayerApp(videoPlayerAppConfig _config)
        : config(_config),
          hide_cursor(true),
          screen_blanked(true),
          debug(false)
    {};

    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void onVideoEnd(ofxOMXPlayerListenerEventData& e);
    void onVideoLoop(ofxOMXPlayerListenerEventData& e){ /*empty*/ };
    void onCharacterReceived(SSHKeyListenerEventData& e);
    void loadMovie(std::string& file_name);
    void blankScreen();
    void run();

    videoPlayerAppConfig    config;
    bool                    hide_cursor;
    atomic<bool>            screen_blanked;
    bool                    debug;
    ofxOMXPlayerPtr         player;
    ofDirectory             video_dir;
    VideoFileNameQueue      video_file_names;
    VideoQueue              video_queue;
    ConsoleListener         consoleListener;

    Poco::Thread            loader;
    Poco::RWLock            load_file_lock;

    ofxOscReceiver          osc_receiver;
    //ofxOscSender            osc_sender;
};
