CC := gcc 
CFLAGS := -Wall -O2

SRC := myRPC-client.c  
OBJ := myRPC-client.o  
BIN := myRPC-client  

version     := 1.0
revision    := 0
architecture := all

all: $(BIN)

$(OBJ): $(SRC)  
	$(CC) $(CFLAGS) -c $< -o $@  

$(BIN): $(OBJ)  
	$(CC) $(CFLAGS) -o $@ $^ 

deb: deb_client all
	@rm -rf MyRPC_client_$(version)-$(revision)_$(architecture)  

deb_client:
	mkdir MyRPC_client_$(version)-$(revision)_$(architecture)/DEBIAN  
	printf "Package: myRPC-client\nVersion: $(version)\nArchitecture: $(architecture)\nDepends: libmysyslog\nMaintainer: DarthNik\nDescription: Client for remote command execution via myRPC\n" \
		> MyRPC_client_$(version)-$(revision)_$(architecture)/DEBIAN/control
	mkdir MyRPC_client_$(version)-$(revision)_$(architecture)/usr/local/bin  
	cp $(BIN) MyRPC_client_$(version)-$(revision)_$(architecture)/usr/local/bin  
	dpkg-deb --root-owner-group --build MyRPC_client_$(version)-$(revision)_$(architecture)  

clean:  
	rm -f $(OBJ) $(BIN)  
