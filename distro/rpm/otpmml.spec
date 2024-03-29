# norootforbuild
%{?__python3: %global __python %{__python3}}
%if 0%{?suse_version}
%global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")
%else
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}
%endif

%define __cmake %{_bindir}/cmake
%define _cmake_lib_suffix64 -DLIB_SUFFIX=64
%define cmake \
CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS ; \
CXXFLAGS="${CXXFLAGS:-%optflags}" ; export CXXFLAGS ; \
FFLAGS="${FFLAGS:-%optflags}" ; export FFLAGS ; \
%__cmake \\\
-DCMAKE_INSTALL_PREFIX:PATH=%{_prefix} \\\
%if "%{?_lib}" == "lib64" \
%{?_cmake_lib_suffix64} \\\
%endif \
-DBUILD_SHARED_LIBS:BOOL=ON

Name:           otpmml
Version:        1.12
Release:        0%{?dist}
Summary:        OpenTURNS module
Group:          System Environment/Libraries
License:        LGPLv3+
URL:            http://www.openturns.org/
Source0:        http://downloads.sourceforge.net/openturns-modules/otpmml/otpmml-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:  gcc-c++, cmake, swig
BuildRequires:  openturns-devel
BuildRequires:  libxml2-devel
BuildRequires:  python3-openturns
BuildRequires:  python3-devel
Requires:       libotpmml0

%description
Template module for OpenTURNS library.

%package -n libotpmml0
Summary:        OTPMML development files
Group:          Development/Libraries/C and C++

%description -n libotpmml0
Development libraries for OTPMML library.

%package devel
Summary:        OTPMML development files
Group:          Development/Libraries/C and C++
Requires:       libotpmml0 = %{version}
Requires:       openturns-devel

%description devel
Development files for OTPMML library.

%package -n python3-%{name}
Summary:        OTPMML library
Group:          Productivity/Scientific/Math
Requires:       python3-openturns
%description -n python3-%{name}
Python textual interface to OTPMML uncertainty library

%prep
%setup -q

%build
%cmake -DINSTALL_DESTDIR:PATH=%{buildroot} \
       -DCMAKE_SKIP_INSTALL_RPATH:BOOL=ON \
       -DPYTHON_EXECUTABLE=%{__python} \
       -DBUILD_DOC=OFF .
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%check
make tests %{?_smp_mflags}
LD_LIBRARY_PATH=%{buildroot}/usr/lib64 ctest %{?_smp_mflags} --output-on-failure

%clean
rm -rf %{buildroot}

%post -n libotpmml0 -p /sbin/ldconfig 
%postun -n libotpmml0 -p /sbin/ldconfig 

%files -n libotpmml0
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.h*
%{_includedir}/%{name}/swig/
%{_libdir}/*.so
%{_libdir}/cmake/

%files -n python3-%{name}
%defattr(-,root,root,-)
%{python_sitearch}/%{name}/
%{python_sitearch}/%{name}-*.dist-info/


%changelog
* Wed Nov 28 2012 Julien Schueller <schueller at phimeca dot com> 0.0-1
- Initial package creation

