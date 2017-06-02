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
mkdir -p $RPM_BUILD_ROOT/etc/systemd/system
cd $RPM_BUILD_ROOT
cp -f %{SOURCEURL0}/src/service.o ./usr/bin/camflow-provenance
cp -f %{SOURCEURL0}/src/camflow-provenance.service ./etc/systemd/system/camflow-provenance.service
cp -f %{SOURCEURL0}/camflow-service.ini ./etc/camflow-service.ini


%clean
rm -r -f "$RPM_BUILD_ROOT"

%files
%defattr(755,root,root)
/usr/bin/camflow-provenance
%defattr(644,root,root)
/etc/systemd/system/camflow-provenance.service
/etc/camflow-service.ini
