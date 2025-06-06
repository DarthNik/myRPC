CC := gcc

PKG_CONFIG := pkg-config

CONFIG_CFLAGS  := $(shell $(PKG_CONFIG) --cflags libconfig)
CONFIG_LDFLAGS := $(shell $(PKG_CONFIG) --libs libconfig)

LIBDIR := ../libmysyslog

CFLAGS := -Wall -O2 -I$(LIBDIR) $(CONFIG_CFLAGS)
LDFLAGS := -L$(LIBDIR) -lmysyslog $(CONFIG_LDFLAGS)

SRC := myRPC-server.c
OBJ := myRPC-server.o
BIN := myRPC-server

version     := 1.0
revision    := 0
architecture := all

all: $(BIN)

# Compile server
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Link server
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# deb

deb: deb_server all
	@rm -rf MyRPC_server_$(version)-$(revision)_$(architecture)

deb_server:
	mkdir -p MyRPC_server_$(version)-$(revision)_$(architecture)/DEBIAN
	printf "Package: myRPC-server\nVersion: $(version)\nArchitecture: $(architecture)\nDepends: libmysyslog\nMaintainer: DarthNik\nDescription: Server for remote command execution via myRPC\n" \
		> MyRPC_server_$(version)-$(revision)_$(architecture)/DEBIAN/control

	mkdir -p MyRPC_server_$(version)-$(revision)_$(architecture)/usr/local/bin
	cp $(BIN) MyRPC_server_$(version)-$(revision)_$(architecture)/usr/local/bin

	mkdir -p MyRPC_server_$(version)-$(revision)_$(architecture)/lib/systemd/system
	cp ../systemd/myRPC-server.service MyRPC_server_$(version)-$(revision)_$(architecture)/lib/systemd/system

	printf "#!/bin/sh\nsystemctl daemon-reload\n" \
		> MyRPC_server_$(version)-$(revision)_$(architecture)/DEBIAN/postinst
	chmod 755 MyRPC_server_$(version)-$(revision)_$(architecture)/DEBIAN/postinst

	dpkg-deb --root-owner-group --build MyRPC_server_$(version)-$(revision)_$(architecture)

# Copy unit and reload systemd

systemd_install:
	sudo cp ../systemd/myRPC-server.service /etc/systemd/system/
	sudo systemctl daemon-reload

# Cleanup

clean:
	rm -f $(OBJ) $(BIN)
