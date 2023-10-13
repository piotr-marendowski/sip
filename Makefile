CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LIBS = -lX11 -lImlib2

all: sip install

sip: sip.c
	$(CC) $(CFLAGS) $(LIBS) -o sip sip.c

# REQUIRES ROOT PRIVILAGES!
install:
	cp sip /usr/local/bin
