all: clean
	$(MAKE) -C libmysyslog all
	$(MAKE) -C myRPC-client all
	$(MAKE) -C myRPC-server all

clean:
	$(MAKE) -C libmysyslog clean
	$(MAKE) -C myRPC-client clean
	$(MAKE) -C myRPC-server clean
	rm -f *.deb

deb: all
	$(MAKE) -C libmysyslog deb
	$(MAKE) -C myRPC-client deb
	$(MAKE) -C myRPC-server deb
	mv libmysyslog/*.deb .
	mv myRPC-client/*.deb .
	mv myRPC-server/*.deb .

repo: deb
	mkdir /usr/local/repos
	cp *.deb /usr/local/repos
	cd /usr/local/repos && dpkg-scanpackages . /dev/null | gzip -9c > Packages.gz
	echo "deb [trusted=yes] file:/usr/local/repos ./" | sudo tee /etc/apt/sources.list.d/myRPC.list
	sudo apt install update

systemd_install:
	sudo cp systemd/myRPC-server.service /etc/systemd/system/
	sudo systemctl daemon-reload
