#
# Copyright 2002 Sony Corporation 
#
# Permission to use, copy, modify, and redistribute this software for
# non-commercial use is hereby granted.
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#

PREFIX=/usr/local/OPEN_R_SDK
INSTALLDIR=../MS
CXX=$(PREFIX)/bin/mipsel-linux-g++
STRIP=$(PREFIX)/bin/mipsel-linux-strip
MKBIN=$(PREFIX)/OPEN_R/bin/mkbin
STUBGEN=$(PREFIX)/OPEN_R/bin/stubgen2
MKBINFLAGS=-p $(PREFIX)
LIBS=-L$(PREFIX)/OPEN_R/lib -lObjectComm -lOPENR
CXXFLAGS= \
	-O2 \
	-g \
	-I. \
	-I$(PREFIX)/OPEN_R/include/R4000 \
	-I$(PREFIX)/OPEN_R/include

CXXFLAGS+=$(DEBUGFLAGS)

.PHONY: all install clean

all: powerMonitor.bin

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

PowerMonitorStub.cc: stub.cfg
	$(STUBGEN) stub.cfg	

powerMonitor.bin: PowerMonitorStub.o PowerMonitor.o powerMonitor.ocf
	$(MKBIN) $(MKBINFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

install: powerMonitor.bin
	mkdir -p $(INSTALLDIR)/OPEN-R/MW/OBJS/POWERMON
	gzip -c powerMonitor.bin > $(INSTALLDIR)/OPEN-R/MW/OBJS/POWERMON.BIN

clean:
	rm -f *.o *.bin *.elf *.snap.cc
	rm -f PowerMonitorStub.h PowerMonitorStub.cc def.h entry.h
	rm -f $(INSTALLDIR)/OPEN-R/MW/OBJS/POWERMON.BIN
