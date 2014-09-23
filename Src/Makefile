# Makefile for Desktop Application Chooser GTK+ 2.0 application.
# Date: 2008-09-11
# Author: William.L

#CC = gcc
PROG = DesktopAppChooser
HEADERS = CDesktopAppChooser.h

CC = g++
STRIP = strip

CFLAGS = `pkg-config --cflags gtk+-2.0`
LIBS = `pkg-config --libs gtk+-2.0` -lgnome-menu 
       #Add "-lstdc++" parameter if using "gcc" to compile
INCPATH = -I/usr/include/gnome-menus/
# For 64-bit CPU architecture
CPU64 = -m64
GDB = -g

DEFINES = -DTEST
#DEFINES =

appchooser_OBJS = CDesktopAppChooser.o main.o

all: $(PROG)

$(PROG): $(appchooser_OBJS)
	$(CC) -o $(PROG) $(appchooser_OBJS) $(INCPATH) $(CFLAGS) $(LIBS)
#	$(CC) -o $(PROG) $(appchooser_OBJS) $(INCPATH) $(CFLAGS) $(LIBS) $(CPU64)
#Add "-Xlinker --verbose" to gcc's command-line arguments to have it pass this option to ld.
	$(STRIP) $@

%.o: %.cpp $(HEADERS)
	echo Compiling $@ ...
	$(CC) $(DEFINES) $(INCPATH) $(CFLAGS) -c $< -o $@
#	$(CC) $(INCPATH) $(CFLAGS) $(CPU64) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o *.bak *~ *.~cpp *.~h $(PROG)

