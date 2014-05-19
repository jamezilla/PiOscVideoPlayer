#include "videoPlayerApp.h"
#include <algorithm>
#include <cassert>

void videoPlayerApp::onVideoEnd(ofxOMXPlayerListenerEventData& e)
{
    ofLogVerbose(__func__) << " RECEIVED";
    blankScreen();
}

void videoPlayerApp::blankScreen()
{
    screen_blanked = true;
    if (player) player->setPaused(true);
}

void videoPlayerApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
    keyPressed((int)e.character);
}

//--------------------------------------------------------------
void videoPlayerApp::setup()
{
    ofBackground(ofColor::black);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetFullscreen(config.window_full_screen);

    if (hide_cursor)
        ofHideCursor();
    else
        ofShowCursor();

    consoleListener.setup(this);
    osc_receiver.setup(config.osc_local_port);

    // do a quick check to make sure the videos directory exists
    video_dir = ofToDataPath(config.player_video_path, true);
    if (video_dir.exists()) {
        video_dir.listDir();
    } else {
        ofLogError("Videos directory does not exist: " + video_dir.getAbsolutePath());
        ofExit(1);
    }

    ofLogVerbose("--- starting loader thread");
    loader.start(*this);

    blankScreen();
}

void videoPlayerApp::run()
{
    std::string          file_name;
    ofxOMXPlayerSettings player_settings;

    // setup the player defaults
    player_settings.useHDMIForAudio = true;                           //default true
    player_settings.enableTexture   = true;                           //default true
    player_settings.listener        = this;

    // TODO: lock config?
    player_settings.enableLooping   = config.player_enable_looping;   //default true
    player_settings.doFlipTexture   = config.player_flip_texture;
    // TODO: unlock config?


    // TODO: make this a settable condition so we can kill this thread in a destructor
    while(true) {
        if (video_file_names.pop(file_name)) {
            player_settings.videoPath = file_name;
            ofLogVerbose("--- got file name: " + file_name);

            ofxOMXPlayerPtr new_player = ofxOMXPlayerPtr(new ofxOMXPlayer);
            ofLogVerbose("--- constructed new player");

            new_player->setup(player_settings);
            ofLogVerbose("--- new video setup");

            video_queue.push(new_player);
            ofLogVerbose("--- video pushed to queue");
        } else {
            Poco::Thread::yield();
        }
    }
}

void videoPlayerApp::loadMovie(string& file_name )
{
    string file_path = video_dir.getAbsolutePath() + "/" + file_name;

    if (!ofFile(file_path).exists()) {
        ofLogError("!! File does not exist !!");
        return;
    }

    ofLogNotice("--- queueing file name: " + file_path);
    video_file_names.push(file_path);
}

//--------------------------------------------------------------
void videoPlayerApp::update()
{
    while (osc_receiver.hasWaitingMessages() ) {
        ofxOscMessage m;
        osc_receiver.getNextMessage( &m );

        if (m.getAddress() == "/blankScreen" ) {
            ofLogVerbose("got message: /blankScreen");
            blankScreen();
        }

        if ( m.getAddress() == "/debug" ) {
            ofLogVerbose("got message: /debug");
            debug = !debug;
        }

        if ( m.getAddress() == "/fullScreen" ) {
            ofLogVerbose("got message: /fullScreen");
            ofToggleFullscreen();
        }

        if ( m.getAddress() == "/loadMovie" ) {
            string file_name = m.getArgAsString(0);
            ofLogVerbose("got message: /loadMovie " + file_name);
            if (0 == file_name.compare("blankScreen"))
                blankScreen();
            else
                loadMovie(file_name);
        }
    }

    bool swapped = false;
    ofxOMXPlayerPtr next_player;

    while(video_queue.pop(next_player)) {
        ofLogVerbose("--- found player on queue");
        swapped = true;
        player = ofxOMXPlayerPtr(next_player);
    }

    if (swapped) {
        // player->stepFrameForward();
        ofLogVerbose("--- unpausing new player");
        assert(player);
        player->setPaused(false);
        if (screen_blanked) screen_blanked = false;
    }

}


//--------------------------------------------------------------
void videoPlayerApp::draw(){
    if (screen_blanked) {
        ofBackground(ofColor::black);
        if (debug) {
            ofDrawBitmapStringHighlight("screen blanked", 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
        }
    } else {
        assert(player);
        player->draw(0, 0);
        if (debug) {
            ostringstream info;
            info << "\n" <<  "CURRENT MOVIE: " << player->settings.videoPath;
            ofDrawBitmapStringHighlight(player->getInfo() + info.str(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
        }
    }
}

//--------------------------------------------------------------
void videoPlayerApp::keyPressed  (int key){

    ofLogVerbose(__func__) << "key: " << key;
    switch (key) {
    case 'b':
        blankScreen();
        break;
    case 'c':
        hide_cursor = !hide_cursor;
        if (hide_cursor)
            ofHideCursor();
        else
            ofShowCursor();
        break;
    case 'd':
        debug = !debug;
        break;
    case 'f':
        ofToggleFullscreen();
        break;
    case 'p':
        if (player) {
            ofLogVerbose() << "pause: " << !player->isPaused();
            player->setPaused(!player->isPaused());
        }
        break;
    }
}
