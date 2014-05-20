b = NetAddr.new("10.0.1.10", 57121);    // port is configurable in bin/data/settings.xml
b.sendMsg("/loadMovie", "Timecoded_Big_bunny_1.mov");
b.sendMsg("/loadMovie", "Timecoded_Big_bunny_1.mov", 100, 100, 45, 45);
b.sendMsg("/position", 100, 100, 15, 5);
b.sendMsg("/position", 100, 100, -30, 20);
b.sendMsg("/loadMovie", "Timecoded_Big_bunny_2.mov");
b.sendMsg("/loadMovie", "01_07_light_moth_90.mp4");
b.sendMsg("/loadMovie", "yelloMoth.mp4");
b.sendMsg("/loadMovie", "blankScreen");
b.sendMsg("/blankScreen");
b.sendMsg("/debug");                    // turn on debug view
b.sendMsg("/fullScreen");               // toggle full screen mode


