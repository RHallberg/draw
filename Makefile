CC = gcc
CFLAGS = -Wall
LDFLAGS = `pkg-config --libs sdl3`
SRCS = draw.c

TARGET = draw

ifeq ($(DEBUG), 1)
    CFLAGS += -g -O0
endif

all: $(TARGET)

debug:
	$(MAKE) DEBUG=1

$(TARGET): $(SRCS)
	rm -f $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
