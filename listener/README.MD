# Listener 

step 1. use FFmpeg to decode .mp3 file.
Follow the guide https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu) to make and install FFmpeg.
Follow the Makefile in ffmpeg_build/share/ffmpeg/examples to make our C++ program.
There is a bug in ffmpeg_build/lib/pkgconfig/libavutil.pc, add following line to fix it.
Libs: -L${libdir}  -lavutil -lm -lX11 -lvdpau -lva-drm -lva-x11 -lva