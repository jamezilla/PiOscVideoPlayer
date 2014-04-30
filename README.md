# Compilation

The build scripts expect a directory layout where openframeworks is a peer folder at the same level as videoPlayerApp.

NOTE: the videoPlayerApp folder is not inside the the openframeworks
folder (usually two folders deep in "apps"). If you want to move the
videoPlayerApp folder somewhere else you can adjust the OF_ROOT
variable in config.make to point to the location of the root of the
openframeworks framework source.

Compiling:

    $ cd /path-to-videoPlayerApp
    $ make

# Configuration

See bin/data/settings.xml


# Upstart Daemon (automatic start at bootup)

You have to replace sysvinit with upstart for this (don't do this
unless you know what this means!).

NOTE: After you do this you may have to use SSH to get into the
machine to shutdown the daemon if you don't want it to run. Make sure
you have SSH turned on for your pi.

## Installing

If you still insist:

    $ sudo apt-get install upstart
    $ sudo cp /path-to-videoPlayerApp/scripts/videoPlayerApp.conf /etc/init/
    $ sudo reboot

## Running

    $ sudo start videoPlayerApp
    $ sudo stop videoPlayerApp
    $ sudo restart videoPlayerApp
    $ sudo status videoPlayerApp
