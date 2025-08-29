CFLAGS = -Wall
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

all: draw

draw: draw.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	rm -f draw.o

draw.o: draw.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f draw

debug:
	$(MAKE) DEBUG=1

