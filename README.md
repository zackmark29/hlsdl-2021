hlsdl
=====
Forked from https://github.com/xavave/hlsdl-vs2019

This program converts .m3u8 playlists (using fragmented MPEG-2 Transport Streams) to a .ts video. It supports decryption of both AES-128 and SAMPLE-AES encryption.

Requirements
------------

This program requires libcurl and libcrypto libraries.

Build
-----

![hlsdl build](https://github.com/xavave/hlsdl/workflows/hlsdl%20build/badge.svg)

Linux:
`make && make install && make clean`

Windows:
https://github.com/xavave/hlsdl/blob/master/msvc/BUILD_WINDOWS.txt


Usage and Options
-----------------
`./hlsdl [options] url`

---------------------------
```
-b ... Automatically choose the best quality.

-W ... Choose largest width lower or equal than this.

-H ... Choose largest height lower or equal than this.

-A ... Select audio language.

-v ... Verbose more information.

-o ... Choose name of output file ("-" alias for stdout).

-u ... Set custom HTTP User-Agent header.

-h ... Set custom HTTP header.

-p ... Set proxy uri.

-k ... Allow to replace part of AES key uri - old.

-n ... Allow to replace part of AES key uri - new.

-f ... Force overwriting the output file.

-F ... Force ignore detection of DRM.

-K ... Force AES key value (hexstring)

-q ... Print less to the console.

-d ... Print the openssl decryption command.

-t ... Print the links to the .ts files.

-s ... Set live start offset in seconds.

-i ... Set live stream download duration in seconds.

-e ... Set refresh delay in seconds.

-r ... Set max retries at open.

-w ... Set max download segment retries.

-a ... Set additional url to the audio media playlist.

-c ... Treat HTTP code 206 as 200 even if request was made without range header.

-C ... the file name of file holding cookie data in the old Netscape / Mozilla cookie data format.
```

ToDo
-----
* support for Fragmented MPEG-4 playlist
* support for EXT-X-MAP in the MPEG-2 Transport Streams playlist

Ideas
-----

- Multithreading

License
-------

[MIT License](https://github.com/xavave/hlsdl/blob/master/LICENSE)
