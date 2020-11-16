version=0.3.0
CCC = gcc

all:
	cd ./src && $(MAKE) all

prepare:
	echo 'Nothing to do.'

mosquitto:
	sudo cp -f ./mosquitto.conf /etc/mosquitto/mosquitto.conf
	sudo systemctl enable mosquitto.service
	sudo systemctl start mosquitto.service

install:
	cd ./src && sudo $(MAKE) install
	sudo cp --force ./camflowd.ini /etc/camflowd.ini

rpm:
	mkdir -p ~/rpmbuild/{RPMS,SRPMS,BUILD,SOURCES,SPECS,tmp}
	cp -f ./camflowd.spec ~/rpmbuild/SPECS/camflowd.spec
	rpmbuild -bb camflowd.spec
	mkdir -p output
	cp ~/rpmbuild/RPMS/x86_64/* ./output

deb:
	sudo alien output/camflowd-$(version)-1.x86_64.rpm
	cp *.deb ./output

publish_rpm:
	cd ./output && package_cloud push camflow/provenance/fedora/33 camflowd-$(version)-1.x86_64.rpm

publish_deb:
	cd ./output && package_cloud push camflow/provenance/ubuntu/bionic camflowd_$(version)-2_amd64.deb

publish: publish_rpm publish_deb


restart:
	cd ./src && sudo $(MAKE) restart

clean:
	cd ./src && $(MAKE) clean
	rm -rf *.o
	rm -rf output
