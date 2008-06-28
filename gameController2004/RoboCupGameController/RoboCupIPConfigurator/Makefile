#
# Copyright 2003 Sony Corporation 
#
# Permission to use, copy, modify, and redistribute this software is
# hereby granted.
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#

INSTALLDIR=../RP/host
CXXFLAGS= \
	-O2 \
	-I. \
	-I../include \
	`pkg-config gtk+-2.0 --cflags`


LDFLAGS= `pkg-config gtk+-2.0 --libs`

TARGET= RoboCupIPConfigurator

CP=cp

.PHONY: all install clean

all: $(TARGET)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

$(TARGET): RoboCupIPConfigurator.o Gui.o ../libTeamInfo/src/TeamInfo.o
	$(CXX) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	if test -f $^.exe; then $(CP) $^.exe $(INSTALLDIR)/../; else $(CP) $^ $(INSTALLDIR)/../; fi

clean:
	rm -f *.o $(TARGET)
