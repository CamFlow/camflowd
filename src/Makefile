INCLUDES = -I../build/inih -I../include -I../build/paho.mqtt.c/src
CCFLAGS = -g -O2 -fpic
CCC = gcc
LIB = /usr/local/lib/libprovenance.a  ../output/libpaho-mqtt3c.a -lbsd -lpthread ../build/inih/extra/libinih.a -lz
.SUFFIXES: .c

all:
	$(CCC) $(INCLUDES) service.c $(LIB) -o service.o $(CCFLAGS)

debug:
	$(CCC) $(INCLUDES) service.c $(LIB) -o service.o $(CCFLAGS) -g

install:
	cp --force ./service.o /usr/bin/camflowd
	mkdir -p /var/relay
	cp --force ./camflowd.service /etc/systemd/system/camflowd.service
	systemctl enable camflowd.service

restart:
	systemctl restart camflowd.service

clean:
	rm -f service.o
