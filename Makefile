all: clean
	$(MAKE) -C libmysyslog all
	$(MAKE) -C myRPC-client all
	$(MAKE) -C myRPC-server all
	sudo touch /var/log/myRPC.log

clean:
	$(MAKE) -C libmysyslog clean
	$(MAKE) -C myRPC-client clean
	$(MAKE) -C myRPC-server clean
	sudo rm -f *.deb /var/log/myRPC.log

deb: all
	$(MAKE) -C libmysyslog deb
	$(MAKE) -C myRPC-client deb
	$(MAKE) -C myRPC-server deb
	mv libmysyslog/*.deb .
	mv myRPC-client/*.deb .
	mv myRPC-server/*.deb .

repo: deb
	mkdir -p /usr/local/repos
	cp *.deb /usr/local/repos
	cd /usr/local/repos && dpkg-scanpackages . /dev/null | gzip -9c > Packages.gz
	echo "deb [trusted=yes] file:/usr/local/repos ./" | sudo tee /etc/apt/sources.list.d/myRPC.list
	sudo apt-get update

systemd_install:
	sudo cp systemd/myRPC-server.service /etc/systemd/system/
	sudo systemctl daemon-reload
