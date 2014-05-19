#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "videoPlayerApp.h"

#define SETTINGS_XML_FILE              "settings.xml"
#define DEFAULT_WINDOW_FULL_SCREEN     "false"
#define DEFAULT_WINDOW_WIDTH           1280
#define DEFAULT_WINDOW_HEIGHT          720
#define DEFAULT_PLAYER_VIDEO_PATH      "videos"
#define DEFAULT_PLAYER_ENABLE_LOOPING  "false"
#define DEFAULT_PLAYER_FLIP_VERTICAL   "false"
#define DEFAULT_OSC_LOCAL_PORT         57121
#define DEFAULT_OSC_LOCAL_HOST         "127.0.0.1"
// #define DEFAULT_OSC_REMOTE_PORT        57120
// #define DEFAULT_OSC_REMOTE_HOST        "127.0.0.1"

int main()
{
    ofSetLogLevel(OF_LOG_VERBOSE);

    videoPlayerAppConfig app_config;
    ofxXmlSettings settings;

    if (!settings.loadFile(SETTINGS_XML_FILE)) {
        ofLogNotice("Couldn't load settings file, using defaults");
    }

    app_config.window_width           = settings.getValue("settings:window:width",          DEFAULT_WINDOW_WIDTH);
    app_config.window_height          = settings.getValue("settings:window:height",         DEFAULT_WINDOW_HEIGHT);
    app_config.player_video_path      = settings.getValue("settings:player:videoPath",      DEFAULT_PLAYER_VIDEO_PATH);
    app_config.osc_local_port         = settings.getValue("settings:osc:localPort",         DEFAULT_OSC_LOCAL_PORT);
    app_config.osc_local_host         = settings.getValue("settings:osc:localHost",         DEFAULT_OSC_LOCAL_HOST);
    // app_config.osc_remote_port        = settings.getValue("settings:osc:remotePort",        DEFAULT_OSC_REMOTE_PORT);
    // app_config.osc_remote_host        = settings.getValue("settings:osc:remoteHost",        DEFAULT_OSC_REMOTE_HOST);

    string val = settings.getValue("settings:player:enableLooping",  DEFAULT_PLAYER_ENABLE_LOOPING);
    app_config.player_enable_looping = (val.compare("true") == 0);

    val = settings.getValue("settings:player:flipTexture",   DEFAULT_PLAYER_FLIP_VERTICAL);
    app_config.player_flip_texture = (val.compare("true") == 0);

    val = settings.getValue("settings:window:fullScreen",     DEFAULT_WINDOW_FULL_SCREEN);
    app_config.window_full_screen = (val.compare("true") == 0);

    ofSetupOpenGL(app_config.window_width, app_config.window_height, OF_WINDOW);

    ofRunApp(new videoPlayerApp(app_config));

}
