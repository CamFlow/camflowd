Summary: CamFlow publisher service
Name: camflowd
Version: 0.1.8
Release: 1
Group: audit/camflow
License: GPLv2
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
cp -f %{SOURCEURL0}/src/service.o ./usr/bin/camflowd
cp -f %{SOURCEURL0}/src/camflowd.service ./etc/systemd/system/camflowd.service
cp -f %{SOURCEURL0}/camflowd.ini ./etc/camflowd.ini


%clean
rm -r -f "$RPM_BUILD_ROOT"

%files
%defattr(755,root,root)
/usr/bin/camflowd
%defattr(644,root,root)
/etc/systemd/system/camflowd.service
/etc/camflowd.ini
