# Compilation

This project has c++11 features. As of 2014-05-19, raspian doesn't come with a c++11 compiler. To get one and set it as the default compiler:

    $ sudo apt-get install g++-4.7
    $ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.6
    $ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.7
    $ sudo update-alternatives --config gcc

The build scripts expect a directory layout where openframeworks is a peer folder at the same level as PiOscVideoPlayer.

NOTE: the PiOscVideoPlayer folder is not inside the the openframeworks
folder (usually two folders deep in "apps"). If you want to move the
PiOscVideoPlayer folder somewhere else you can adjust the OF_ROOT
variable in config.make to point to the location of the root of the
openframeworks framework source.

Compiling:

    $ cd /path-to-PiOscVideoPlayer
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
    $ sudo cp /path-to-PiOscVideoPlayer/scripts/PiOscVideoPlayer.conf /etc/init/
    $ sudo reboot

## Running

    $ sudo start PiOscVideoPlayer
    $ sudo stop PiOscVideoPlayer
    $ sudo restart PiOscVideoPlayer
    $ sudo status PiOscVideoPlayer

## Troubleshooting

If you see errors in /var/log/upstart/PiOscVideoPlayer.log like:

```
* failed to open vchiq instance
```

...here's the rememdy:

    $ sudo su -
    $ echo 'SUBSYSTEM=="vchiq",GROUP="video",MODE="0666"' > /etc/udev/rules.d/10-vchiq-permissions.rules
    $ usermod -a -G video pi
    $ exit
