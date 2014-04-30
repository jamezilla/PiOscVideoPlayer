#include "networkPlayerApp.h"
#include <algorithm>
#include "Poco/ScopedLock.h"

void networkPlayerApp::onVideoEnd(ofxOMXPlayerListenerEventData& e)
{
    ofLogVerbose(__func__) << " RECEIVED";
    blankScreen();
}

void networkPlayerApp::blankScreen()
{
    screen_blanked = true;
    front_player->setPaused(true);
}

void networkPlayerApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
    keyPressed((int)e.character);
}

//--------------------------------------------------------------
void networkPlayerApp::setup()
{
    ofBackground(ofColor::black);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    consoleListener.setup(this);

    //this will let us just grab a video without recompiling
    ofDirectory dir(ofToDataPath(config.player_video_path, true));
    if (dir.exists()) {
        dir.listDir();
        dir.sort();
        files = dir.getFiles();
        if (files.size() > 0) {
            ofxOMXPlayerSettings player_settings;
            player_settings.videoPath = files[front_video_index].path();
            player_settings.useHDMIForAudio = true;                         //default true
            player_settings.enableLooping = config.player_enable_looping;   //default true
            player_settings.doFlipTexture = config.player_flip_texture;
            player_settings.enableTexture = true;                           //default true
            player_settings.listener = this;                                //this app extends ofxOMXPlayerListener so it will receive events ;

            front_player = ofxOMXPlayerPtr(new ofxOMXPlayer);
            front_player->setup(player_settings);

            back_player = ofxOMXPlayerPtr(new ofxOMXPlayer);
            back_player->setup(player_settings);
            back_player->setPaused(true);
        } else {
            ofLogError("Videos directory does not exist: " + dir.getAbsolutePath());
            ofExit(2);
        }
    } else {
        ofLogError("Videos directory does not exist: " + dir.getAbsolutePath());
        ofExit(1);
    }

    blankScreen();
}

void networkPlayerApp::run()
{
    Poco::FastMutex::ScopedLock lock(mutex);

    back_player->loadMovie(files[back_video_index].path());
    back_player->updatePixels();
    back_player->setPaused(false);

    swap_players = true;
}

void networkPlayerApp::loadMovie(uint_fast8_t index)
{
    if (index > files.size()-1) {
        ofLogError("!! File index out of bounds !!");
        return;
    }

    {
        Poco::FastMutex::ScopedLock lock(mutex);
        back_video_index = index;
    }

    thread.start(*this);
}

//--------------------------------------------------------------
void networkPlayerApp::update()
{
    Poco::FastMutex::ScopedLock lock(mutex);
    if (swap_players) {
        swap(front_player, back_player);
        swap(front_video_index, back_video_index);
        back_player->setPaused(true);
        swap_players = false;
        if (screen_blanked) screen_blanked = false;
    }
}


//--------------------------------------------------------------
void networkPlayerApp::draw(){
    if (screen_blanked) {
        ofBackground(ofColor::black);
    } else {
        Poco::FastMutex::ScopedLock lock(mutex);
        front_player->draw(0, 0, ofGetWidth(), ofGetHeight());
        if (debug) {
            stringstream info;
            info << "\n" <<  "CURRENT MOVIE: " << files[front_video_index].path();
            ofDrawBitmapStringHighlight(front_player->getInfo() + info.str(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
        }
    }
}

//--------------------------------------------------------------
void networkPlayerApp::keyPressed  (int key){

    ofLogVerbose(__func__) << "key: " << key;
    switch (key) {
    case '0':
        loadMovie(0);
        break;
    case '1':
        loadMovie(1);
        break;
    case '2':
        loadMovie(2);
        break;
    case '3':
        loadMovie(3);
        break;
    case '4':
        loadMovie(4);
        break;
    case '5':
        loadMovie(5);
        break;
    case '6':
        loadMovie(6);
        break;
    case '7':
        loadMovie(7);
        break;
    case '8':
        loadMovie(8);
        break;
    case '9':
        loadMovie(9);
        break;
    case 'b':
        blankScreen();
        break;
    case 'd':
        debug = !debug;
        break;
    case 'p':
        {
            Poco::FastMutex::ScopedLock lock(mutex);
            ofLogVerbose() << "pause: " << !front_player->isPaused();
            front_player->setPaused(!front_player->isPaused());
        }
        break;
    }
}
