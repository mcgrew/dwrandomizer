
CC          = gcc
WINCC       = ../mxe/usr/bin/i686-w64-mingw32.static-gcc
INCLUDES    = -I. 
CFLAGS      = -g # -fPIC
DEFINES     = 
EXTRA_CFLAGS= -Wall -Wextra -Wno-sign-compare
PROGRAM_NAME= dwrandomizer
 
LIBS        = -lm
 
CC_SWITCHES= $(EXTRA_CFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES)
 
OBJS=
SOURCE= crc64.c dwrando-cli.c dwr.c map.c mt19937-64.c
 
#install: all
#	cp $(PROGRAM_NAME) /usr/bin/
 
all: $(OBJS)
	$(CC) $(CC_SWITCHES) $(SOURCE) $(LIBS) -o $(PROGRAM_NAME)
 
win: $(WINOBJS)
	$(WINCC) $(CC_SWITCHES) $(SOURCE) $(LIBS) -o $(PROGRAM_NAME).exe

.c.o:
	$(CC) -c $(CC_SWITCHES) $<
 
clean:
	rm -rf $(OBJS) $(WINOBJS)
 
