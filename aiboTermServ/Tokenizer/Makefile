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
INSTALLDIR=../RP/host/MS
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

all: TOKENIZER.bin

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

TokenizerStub.cc: stub.cfg
	$(STUBGEN) $^

TOKENIZER.bin: Buffer.o TokenizerStub.o Tokenizer.o $(OCF)
	$(MKBIN) $(MKBINFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

install: TOKENIZER.bin
	mkdir -p $(INSTALLDIR)/OPEN-R/MW/OBJS
	$(CP) $^ $(INSTALLDIR)/OPEN-R/MW/OBJS/TOKENIZER.BIN

clean:
	rm -f *.o *.bin *.elf *.snap.cc *.orig
	rm -f TokenizerStub.h TokenizerStub.cc def.h entry.h
	rm -f $(INSTALLDIR)/OPEN-R/MW/OBJS/TOKENIZER.BIN

