CC = gcc
CFLAGS = -Wall -O2 -fPIC

SRC = libmysyslog.c
OBJ = libmysyslog.o
STATIC = libmysyslog.a
SHARED = libmysyslog.so

version     = 1.0
revision    = 0
architecture = all

all: $(STATIC) $(SHARED)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC): $(OBJ)
	ar rcs $@ $^

$(SHARED): $(OBJ)
	$(CC) -shared -o $@ $^

deb:
	mkdir -p MySyslog_library_$(version)-$(revision)_$(architecture)/DEBIAN
	printf "Package: libmysyslog\nVersion: $(version)\nArchitecture: $(architecture)\nMaintainer: DarthNik\nDescription: Core library for mysyslog logging\n" \
		> MySyslog_library_$(version)-$(revision)_$(architecture)/DEBIAN/control
	mkdir -p MySyslog_library_$(version)-$(revision)_$(architecture)/usr/local/lib
	mkdir -p MySyslog_library_$(version)-$(revision)_$(architecture)/usr/local/include
	cp $(STATIC) MySyslog_library_$(version)-$(revision)_$(architecture)/usr/local/lib
	cp libmysyslog.h MySyslog_library_$(version)-$(revision)_$(architecture)/usr/local/include
	dpkg-deb --root-owner-group --build MySyslog_library_$(version)-$(revision)_$(architecture)

clean:
	rm -f $(OBJ) $(STATIC) $(SHARED)
	rm -rf MySyslog_library_$(version)-$(revision)_$(architecture)
