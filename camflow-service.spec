Summary: CamFlow publisher service
Name: camflow-service
Version: 0.1.1
Release: 1
Group: audit/camflow
License: GPLv3
Source: %{expand:%%(pwd)}
BuildRoot: %{_topdir}/BUILD/%{name}-%{version}-%{release}
Requires: openssl, zlib, glibc

%description
%{summary}

%prep
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p $RPM_BUILD_ROOT/usr/local/lib
mkdir -p $RPM_BUILD_ROOT/etc/systemd/system
cd $RPM_BUILD_ROOT
cp -f %{SOURCEURL0}/src/service.o ./usr/bin/camflow-provenance
cp -f %{SOURCEURL0}/src/camflow-provenance.service ./etc/systemd/system/camflow-provenance.service
cp -f %{SOURCEURL0}/camflow-service.ini ./etc/camflow-service.ini
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3a.so.1 ./usr/lib/libpaho-mqtt3a.so
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3as.so.1 ./usr/lib/libpaho-mqtt3as.so
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3c.so.1 ./usr/lib/libpaho-mqtt3c.so
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3cs.so.1 ./usr/lib/libpaho-mqtt3cs.so
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3a.so.1 ./usr/local/lib/libpaho-mqtt3a.so
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3as.so.1 ./usr/local/lib/libpaho-mqtt3as.so
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3c.so.1 ./usr/local/lib/libpaho-mqtt3c.so
cp -f %{SOURCEURL0}/build/paho.mqtt.c/build/output/libpaho-mqtt3cs.so.1 ./usr/local/lib/libpaho-mqtt3cs.so


%clean
rm -r -f "$RPM_BUILD_ROOT"

%files
%defattr(755,root,root)
/usr/bin/camflow-provenance
%defattr(644,root,root)
/etc/systemd/system/camflow-provenance.service
/etc/camflow-service.ini
/usr/lib/libpaho-mqtt3a.so
/usr/lib/libpaho-mqtt3as.so
/usr/lib/libpaho-mqtt3c.so
/usr/lib/libpaho-mqtt3cs.so
/usr/local/lib/libpaho-mqtt3a.so
/usr/local/lib/libpaho-mqtt3as.so
/usr/local/lib/libpaho-mqtt3c.so
/usr/local/lib/libpaho-mqtt3cs.so
