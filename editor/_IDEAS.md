# prototyping
- asset downloader (+license manager) (move to editor?)

# target manager
- chat server (move to editor?)
- synergy-ish (move to editor?)
- target manager: send/play, screenshot/videocap, put/get savegame, send cvar/cmd, read logs

# synergy-ish command
- send console commands
- send mouse{l,m,r,wheel,accel,pos}/keyboard/gamepad/clipboard inputs to console
- send screen dimensions {0,0,w,h,warp}, focus/blur, 
- send clipboard
- send savegame
- recv savegame
- recv clipboard
- recv screen dimensions (if resizing or if desktop)
- recv clipboard (if desktop)
- recv console output
- recv logger + filter
- recv screenshot
- recv video recording

```
void download( const char *url ) {
#ifdef _WIN32
//powershell -Command "Invoke-WebRequest https://github.com/r-lyeh/AVA/archive/master.zip -OutFile master.zip"
#else
///usr/bin/curl https://github.com/r-lyeh/AVA/archive/master.zip -o master.zip
#endif

#if 0
curl -fsSL https://raw.githubusercontent.com/tboox/xmake/master/scripts/get.sh
wget https://raw.githubusercontent.com/tboox/xmake/master/scripts/get.sh -O -
Invoke-Webrequest 'https://raw.githubusercontent.com/tboox/xmake/master/scripts/get.ps1' -UseBasicParsing
#endif
}
```