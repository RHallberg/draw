CFLAGS = -Wall
SRCS = draw.c

TARGET = draw
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	CC = clang
	LDFLAGS = -I/usr/local/include/SDL3 -L/usr/local/lib -lSDL3 -Wl,-rpath,/usr/local/lib
else ifeq ($(UNAME_S),Linux)
	CC = gcc
	LDFLAGS = $(shell pkg-config --libs sdl3)
endif

ifeq ($(DEBUG), 1)
    CFLAGS += -g
endif

all: $(TARGET)

debug:
	$(MAKE) DEBUG=1

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
