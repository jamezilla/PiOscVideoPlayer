#include "videoPlayerApp.h"
#include <algorithm>
#include "Poco/ScopedLock.h"

void videoPlayerApp::onVideoEnd(ofxOMXPlayerListenerEventData& e)
{
    ofLogVerbose(__func__) << " RECEIVED";
    blankScreen();
}

void videoPlayerApp::blankScreen()
{
    screen_blanked = true;
    front_lock.writeLock();
    front_player->setPaused(true);
    front_lock.unlock();
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
    consoleListener.setup(this);
    osc_receiver.setup(config.osc_local_port);

    //this will let us just grab a video without recompiling
    ofDirectory dir(ofToDataPath(config.player_video_path, true));
    if (dir.exists()) {
        dir.listDir();
        dir.sort();
        files = dir.getFiles();
        if (files.size() > 0) {
            ofxOMXPlayerSettings player_settings;
            player_settings.videoPath = files[0].path();
            player_settings.useHDMIForAudio = true;                         //default true
            player_settings.enableLooping = config.player_enable_looping;   //default true
            player_settings.doFlipTexture = config.player_flip_texture;
            player_settings.enableTexture = true;                           //default true
            player_settings.listener = this;                                //this app extends ofxOMXPlayerListener so it will receive events ;

            front_player = ofxOMXPlayerPtr(new ofxOMXPlayer);
            front_player->setup(player_settings);
            front_index = 0;

            back_player = ofxOMXPlayerPtr(new ofxOMXPlayer);
            back_player->setup(player_settings);
            back_index = 0;
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

void videoPlayerApp::run()
{
    while(!back_lock.tryWriteLock())
        Poco::Thread::yield();

    back_player->loadMovie(files[back_index].path());
    back_player->stepFrameForward();
    back_player->setPaused(false);

    while(!front_lock.tryWriteLock())
        Poco::Thread::yield();

    swap(back_player, front_player);
    front_lock.unlock();
    back_lock.unlock();
    front_index = back_index.load();
    if (screen_blanked) screen_blanked = false;

}
void videoPlayerApp::loadMovie(uint_fast8_t index)
{
    if (index > files.size()-1) {
        ofLogError("!! File index out of bounds !!");
        return;
    }

    back_index = index;
    pool.start(*this);
}

//--------------------------------------------------------------
void videoPlayerApp::update()
{
    while (osc_receiver.hasWaitingMessages() ) {
        ofxOscMessage m;
        osc_receiver.getNextMessage( &m );

        if ( m.getAddress() == "/loadMovie" ) {
            int i = m.getArgAsInt32(0);
            loadMovie(i);
        }

        if (m.getAddress() == "/blankScreen" ) {
            blankScreen();
        }
    }
}


//--------------------------------------------------------------
void videoPlayerApp::draw(){
    if (screen_blanked) {
        ofBackground(ofColor::black);
    } else {
        front_lock.readLock();
        front_player->draw(0, 0, ofGetWidth(), ofGetHeight());
        if (debug) {
            stringstream info;
            info << "\n" <<  "CURRENT MOVIE: " << files[front_index].path();
            ofDrawBitmapStringHighlight(front_player->getInfo() + info.str(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
        }
        front_lock.unlock();
    }
}

//--------------------------------------------------------------
void videoPlayerApp::keyPressed  (int key){

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
            front_lock.writeLock();
            ofLogVerbose() << "pause: " << !front_player->isPaused();
            front_player->setPaused(!front_player->isPaused());
            front_lock.unlock();
        }
        break;
    }
}
