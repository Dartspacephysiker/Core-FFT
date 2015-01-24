CC = gcc
CFLAGS = -std=gnu99 -pipe -O2 -pthread -Wall -fPIC -D_FILE_OFFSET_BITS=64
INCLUDE = -I/usr/include/python2.7 -I/opt/local/include -I/SPENCEdata2/software/Enthought/Canopy/appdata/canopy-1.4.0.1938.rh5-x86_64/include/python2.7/

LIBS = -L/usr/lib/python2.7 -L/opt/local/lib -L/usr/local/lib
LDLIBS = -lpthread -lfftw3_threads -lfftw3 -lm -pipe
OSXLD = -bundle -flat_namespace -undefined suppress # OS X Linker Flags
POSLD = -shared # POSIX Linker Flags
OSTYPE := $(shell uname)

ifeq ($(OSTYPE),Darwin)
	LDFLAGS = $(OSXLD) $(LDLIBS)
else
	LDFLAGS = $(POSLD) $(LDLIBS)
endif

SWIGDEF = core-fft.i
SOURCES = complex-1chan.c float4-1chan.c float8-1chan.c uint16-1chan.c real-nchan.c uint8-1chan.c core-fft.c
WRAPPER = core-fft_wrap.c
OBJECTS = $(SOURCES:.c=.o) $(WRAPPER:.c=.o)
EXEC = _coreFFT.so

all: $(WRAPPER) $(EXEC)

$(WRAPPER):
	swig -python $(SWIGDEF)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS) 

.c.o:
	$(CC) -o $@ $(CFLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o $(EXEC) $(WRAPPER) *.pyc

install:
	cp _coreFFT.so coreFFT.py /usr/ggse/lib/
	cp core-fft.py /usr/ggse/bin/
