version=0.1.8
CCC = gcc

all:
	cd ./src && $(MAKE) all

checkout:
	mkdir -p build
	cd ./build && git clone https://github.com/benhoyt/inih.git
	cd ./build/inih/extra && $(MAKE) -f Makefile.static default
	cd ./build && git clone https://github.com/eclipse/paho.mqtt.c.git
	cd ./build/paho.mqtt.c && git checkout tags/v1.1.0

PAHO_SRC= ./build/paho.mqtt.c/src
PAHO_FILES = $(wildcard $(PAHO_SRC)/*.c)
PAHO_EXEC = $(wildcard *.o)

release.version = 1.1.0
SED_COMMAND = sed \
	-e "s/@CLIENT_VERSION@/${release.version}/g" \
	-e "s/@BUILD_TIMESTAMP@/$(shell date)/g"

build_paho:
	mkdir -p output
	$(SED_COMMAND) <$(PAHO_SRC)/VersionInfo.h.in >$(PAHO_SRC)/VersionInfo.h
	$(CCC) -c -g -fPIC -Os -Wall -I$(PAHO_SRC) $(PAHO_FILES)
	ar rvs output/libpaho-mqtt3c.a $(PAHO_EXEC)
	$(CCC) -c -g -fPIC -Os -Wall -I$(PAHO_SRC) $(PAHO_FILES)
	ar rvs output/libpaho-mqtt3c.a $(PAHO_EXEC)

prepare: checkout
	$(MAKE) build_paho
	$(MAKE) build_paho

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

publish:
	cd ./output && package_cloud push camflow/provenance/fedora/26 camflowd-$(version)-1.x86_64.rpm


restart:
	cd ./src && sudo $(MAKE) restart

clean:
	cd ./src && $(MAKE) clean
	rm -rf *.o
	rm -rf output
	rm -rf build
