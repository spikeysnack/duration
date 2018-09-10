#!/usr/bin/env gmake

SHELL=/bin/sh

CC= gcc
CFLAGS= -O2 -mtune=native --optimize-strlen
#CCOPTS= -O2 -mtune=native 

#CC= clang
#CFLAGS= -O2 -mtune=native
DEBUGCFLAGS= -O0 -ggdb3 

ifneq (,$(wildcard ~/bin))
prefix = ~
else
prefix = /usr/local	
endif

TARGET = duration
SRCDIR = src
BINDIR = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)

.SUFFIXES:
.SUFFIXES: .c .o


testfile = 'hours_of_work_for_nothing.mp3'

#DISTFILES = README* AUTHORS LICENSE Makefile src/  *.bash $(testfile) --exclude="*.old" "duration"
DISTFILES = *  --exclude="*.old" --exclude $(TARGET)

all:	duration


duration: $(SOURCES) $(INCLUDES)
	@cd $(SRCDIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)


debug: $(SOURCES) $(INCLUDES)
	@cd $(SRCDIR)
	$(CC) $(DEBUGCFLAGS) -o $(TARGET) $(SOURCES)


help:	README.md  duration
	@clear
	@view README.md
	@echo "To Install:    make install"

test:	duration 
	@echo
	@echo "[NORMAL]"
	@./duration $(testfile)
	@echo
	@echo "[VERBOSE]"
	@./duration -v $(testfile)
	@echo
	@echo "[duration]"
	@./duration -d $(testfile)
	@echo
	@echo "[bitrate]"
	@./duration -b $(testfile)
	@echo
	@echo "[rate]"
	@./duration -r $(testfile)
	@echo
	@echo "[timecode]"
	@./duration -t $(testfile)
	@echo "[QUIET]"
	@./duration -q $(testfile)
	@echo
	@echo "[QUIET DURATION]"
	@./duration -qd $(testfile)
	@echo
	@echo "[QUIET BITRATE]"
	@./duration -qb $(testfile)
	@echo
	@echo "[QUIET RATE]"
	@./duration -qr $(testfile)
	@echo
	@echo "[QUIET TIMECODE]"
	@./duration -qt $(testfile)
	@echo 


install:	duration
	install  -m 755  $(TARGET) $(prefix)/bin
	@echo "duration installed"

uninstall:
	rm -f $(prefix)/bin/$(TARGET)
	@echo " $(TARGET) uninstalled"

update:	uninstall install
	@echo "$(TARGET) updated"
	@duration -V


dist:	clean
	@rm -f *.tgz
	@tar czvf duration-src.tgz  $(DISTFILES)

clean:
	rm -f core *~ duration

.PHONY:	help test install uninstall update dist clean

