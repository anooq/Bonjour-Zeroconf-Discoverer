TARGETS = build/device_finder
LIBS = -L../mDNSPosix/build/prod/ -ldns_sd

all: $(TARGETS)

clean:
	rm -rf build

build:
	mkdir build

build/device_finder: build device_finder.c ClientCommon.c
	cc $(filter %.c %.o, $+) $(LIBS) -I../mDNSShared -Wall -o $@

build/device_finder-64: build device_finder.c ClientCommon.c
	cc $(filter %.c %.o, $+) $(LIBS) -I../mDNSShared -Wall -o $@ -m64
