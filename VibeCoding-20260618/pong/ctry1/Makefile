CC      = cc
CFLAGS  = -O2 -Wall -Wextra $(shell pkg-config --cflags freetype2 xft 2>/dev/null)
LDLIBS  = -lX11 -lXft -lm

pong: pong.c
	$(CC) $(CFLAGS) -o pong pong.c $(LDLIBS)

clean:
	rm -f pong

.PHONY: clean
