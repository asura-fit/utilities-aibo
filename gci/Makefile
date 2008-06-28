CC=g++
CFLAGS=-O2
LIBRARY=-lm

CC_SRCS=gengci.cc gengci2.cc
TARGET=gengc2ls2 

all: $(TARGET)

install: all
	

gengci: gengci.cc
	$(CC) $(CFLAGS) -o $@ $< -lm

gengc2ls2: gengc2ls2.cc
	$(CC) $(CFLAGS) -o $@ $< -lm

gengc2: gengc2.cc
	$(CC) $(CFLAGS) -o $@ $< -lm

clean:
	$(RM) -fv $(TARGET)
	$(RM) -fv $(TARGET:=.exe)
	$(RM) -fv $(TARGET:=.o)
