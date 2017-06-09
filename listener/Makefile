CC=g++ -std=c++11
INC=-I/home/vei/ffmpeg_build/include
LIB=-L/home/vei/ffmpeg_build/lib

# use pkg-config for getting CFLAGS and LDLIBS
FFMPEG_LIBS=    libavdevice                        \
                libavformat                        \
                libavfilter                        \
                libavcodec                         \
                libswresample                      \
                libswscale                         \
                libavutil                          \

PKG_CFG_LIBS=   cairo $(FFMPEG_LIBS)
            	


CFLAGS += -Wall -g
CFLAGS := $(shell pkg-config --cflags $(PKG_CFG_LIBS)) $(CFLAGS)
LDLIBS := $(shell pkg-config --libs $(PKG_CFG_LIBS)) $(LDLIBS)

Source=main.cpp wave_explorer.cpp

all: $(Objects)
	$(CC) $(Source) $(INC) $(LIB) $(LDLIBS)


