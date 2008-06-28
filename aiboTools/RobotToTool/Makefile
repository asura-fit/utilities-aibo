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

OPENRSDK_ROOT?=/usr/local/OPEN_R_SDK
CP=cp
INSTALLDIR=../MS
STRIP=strip
MKBIN=g++
STUBGEN=$(OPENRSDK_ROOT)/RP_OPEN_R/bin/rp-openr-stubgen2
MKBINFLAGS=
OPENRCONFIG=$(OPENRSDK_ROOT)/RP_OPEN_R/bin/rp-openr-config
LIBS=`$(OPENRCONFIG) --libs`
CXXFLAGS= \
	-O2 \
	-g \
	-I. \
	-I.. \
	-D_OPENR_DEBUG=1 \
	`$(OPENRCONFIG) --cflags`
OCF=

VPATH=..

# Copyright 2002 Sony Corporation 
#
# Permission to use, copy, modify, and redistribute this software for
# non-commercial use is hereby granted.
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#

# NOTE: This makefile is included by the makefiles in the
#       subdirectories.  

.PHONY: all install clean

all: TOTOOL.bin

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

RobotToToolStub.cc: stub.cfg
	$(STUBGEN) $^

TOTOOL.bin: RobotToToolStub.o RobotToTool.o $(OCF)
	$(MKBIN) $(MKBINFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

install: TOTOOL.bin
	mkdir -p $(INSTALLDIR)/OPEN-R/MW/OBJS
	$(CP) $^ $(INSTALLDIR)/OPEN-R/MW/OBJS/TOTOOL.BIN

clean:
	rm -f *.o *.bin *.elf *.snap.cc
	rm -f RobotToToolStub.h RobotToToolStub.cc def.h entry.h
	rm -f $(INSTALLDIR)/OPEN-R/MW/OBJS/TOTOOL.BIN
