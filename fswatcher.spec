Name:           fswatcher
Version:        1.0.0
Release:        1%{?dist}
Summary:        Filesystem Watcher Library using inotify

License:        MIT
URL:            https://example.com/fswatcher
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc, make

%description
This C/C++ library provides a simple, efficient, and flexible way to monitor changes in directories and files on Linux. It uses inotify, a Linux kernel subsystem, to track file modifications, deletions, creations, and other events.

%prep
%setup -q

%build
make

%install
mkdir -p %{buildroot}/usr/lib64
mkdir -p %{buildroot}/usr/include
mkdir -p %{buildroot}/usr/bin

# Install the static library
install -m 644 build/libfswatcher.a %{buildroot}/usr/lib64

# Install the dynamic library
install -m 755 build/libfswatcher.so %{buildroot}/usr/lib64

# Install the header file
install -m 644 include/fswatcher.h %{buildroot}/usr/include

# Install the example binary
install -m 755 build/example_static %{buildroot}/usr/bin
install -m 755 build/example_dynamic %{buildroot}/usr/bin

%files
/usr/lib64/libfswatcher.a
/usr/lib64/libfswatcher.so
/usr/include/fswatcher.h
/usr/bin/example_static
/usr/bin/example_dynamic

%changelog
* Thu Oct 12 2023 Omkar Pokharkar <https://github.com/OmkarPokharkar> - 1.0.0-1
- Initial package
