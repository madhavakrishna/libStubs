#/bin/bash

CC = gcc
CFLAGS = -O3
LIBS += -lm

.PHONY : out

out : FFT.c FFTCommon.c fftCommon.h server.c serverCommon.c serverCommon.h 
	$(CC) $(CFLAGS) FFT.c FFTCommon.c serverCommon.c server.c  $(LIBS)
