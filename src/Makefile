INCLUDES = -I../build/SimpleLogger -I../build/inih
CCFLAGS = -g -O2 -fpic
CCC = gcc
LIB = /usr/local/lib/provenancelib.a ../build/paho.mqtt.c/build/output/libpaho-mqtt3c.so -lpthread ../build/SimpleLogger/libsimplog.a ../build/inih/extra/libinih.a -lz
.SUFFIXES: .c

all:
	$(CCC) $(INCLUDES) publisher.c $(LIB) -o publisher.o $(CCFLAGS)

debug:
	$(CCC) $(INCLUDES) publisher.c $(LIB) -o publisher.o $(CCFLAGS) -g

install:
	cp --force ./publisher.o /usr/bin/camflow-provenance
	mkdir -p /var/relay
	cp --force ./camflow-provenance.service /etc/systemd/system/camflow-provenance.service
	systemctl enable camflow-provenance.service

restart:
	systemctl restart camflow-provenance.service

clean:
	rm -f publisher.o
