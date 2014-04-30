b = NetAddr.new("10.0.1.23", 57121);    // port is configurable in bin/data/settings.xml
b.sendMsg("/loadMovie", 0);
b.sendMsg("/loadMovie", 1);
b.sendMsg("/loadMovie", 2);
b.sendMsg("/blankScreen");
