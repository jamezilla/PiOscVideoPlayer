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
    setPaused(true);
}

void videoPlayerApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
    keyPressed((int)e.character);
}

void videoPlayerApp::setCursorHidden(bool is_hidden)
{
    hide_cursor = is_hidden;
    if (hide_cursor)
        ofHideCursor();
    else
        ofShowCursor();
}


void videoPlayerApp::setPaused(bool is_paused)
{
    front_lock.writeLock();
    front_player->setPaused(is_paused);
    front_lock.unlock();
}

void videoPlayerApp::togglePaused()
{
    front_lock.readLock();
    bool pause_state = front_player->isPaused();
    front_lock.unlock();

    ofLogVerbose() << "pause: " << !pause_state;
    setPaused(pause_state);
}


//--------------------------------------------------------------
void videoPlayerApp::setup()
{
    ofBackground(ofColor::black);
    ofSetFullscreen(config.player_full_screen);
    setCursorHidden(config.player_hide_cursor);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetLogLevel("Loader", OF_LOG_VERBOSE);
    consoleListener.setup(this);
    osc_receiver.setup(config.osc_local_port);

    //this will let us just grab a video without recompiling
    ofDirectory dir(ofToDataPath(config.player_video_path, true));
    if (dir.exists()) {
        dir.listDir();
        dir.sort();
        std::vector<ofFile> dir_files = dir.getFiles();

        for (auto& file: dir_files) {
            files[file.getFileName()] = file;
            ofLogVerbose("-- file map added: " + file.getFileName());
        }

        if (dir_files.size() > 0) {
            ofxOMXPlayerSettings player_settings;
            player_settings.videoPath = dir_files[0].path();
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

    loader.start(*this);

    blankScreen();
}

void videoPlayerApp::run()
{
    loaderParamsPtr params;

    while(true) {
        if (loader_queue.pop(params)) {
            ofLogVerbose("Loader") << ("-- loading file: " + params->file.path());

            back_player->loadMovie(params->file.path());
            back_player->stepFrameForward();
            back_player->setPaused(false);

            while(!front_lock.tryWriteLock())
                Poco::Thread::yield();

            swap(back_player, front_player);
            x  = params->x;
            y  = params->y;
            rx = params->rx;
            ry = params->ry;
            front_lock.unlock();
            screen_blanked = false;
            back_player->setPaused(true);
        } else {
            Poco::Thread::yield();
        }
    }
}

void videoPlayerApp::loadMovie(std::string _file_name, float _x, float _y, float _rx, float _ry)
{
    try {
        loaderParamsPtr dpp = loaderParamsPtr(new loaderParams);
        dpp->file = files.at(_file_name);
        dpp->x    = _x;
        dpp->y    = _y;
        dpp->rx   = _rx;
        dpp->ry   = _ry;
        loader_queue.push(dpp);
    }
    catch (const std::out_of_range& oor) {
        ofLogError("Out of range error: " + _file_name);
    }
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

        if ( m.getAddress() == "/hideCursor" ) {
            ofLogVerbose("got message: /hideCursor");
            ofToggleFullscreen();
        }

        if ( m.getAddress() == "/loadMovie" ) {
            string file_name = m.getArgAsString(0);
            ofLogVerbose("got message: /loadMovie " + file_name);
            if (0 == file_name.compare("blankScreen"))
                blankScreen();
            else {
                float x  = m.getArgAsFloat(1);
                float y  = m.getArgAsFloat(2);
                float rx = m.getArgAsFloat(3);
                float ry = m.getArgAsFloat(4);
                loadMovie(file_name, x, y, rx, ry);
            }
        }

        if ( m.getAddress() == "/position" ) {
            ofLogVerbose("got message: /position");
            x  = m.getArgAsFloat(0);
            y  = m.getArgAsFloat(1);
            rx = m.getArgAsFloat(2);
            ry = m.getArgAsFloat(3);
        }
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
        ofPushMatrix();
        ofRotateX(rx);
        ofRotateY(ry);
        front_lock.readLock();
        front_player->draw(x, y);
        if (debug) {
            stringstream info;
            info << "\n" <<  "CURRENT MOVIE: " << front_player->settings.videoPath;
            ofDrawBitmapStringHighlight(front_player->getInfo() + info.str(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
        }
        front_lock.unlock();
        ofPopMatrix();
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
        setCursorHidden(!hide_cursor);
        break;
    case 'd':
        debug = !debug;
        break;
    case 'p':
        togglePaused();
        break;
    }
}
