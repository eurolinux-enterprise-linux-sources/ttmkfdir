Summary: Utility used to create fonts.scale files for truetype fonts
Name: ttmkfdir
Version: 3.0.9
Release: 1
#URL: 
Source0: %{name}-%{version}.tar.bz2
License: GPL
Group: Applications/System
BuildRoot: %{_tmppath}/%{name}-root
BuildRequires: freetype-devel >= 2.0

# ttmkfdir used to be in the following packages at one point
Conflicts: XFree86-font-utils < 4.2.99.2-0.20021126.3
Conflicts: freetype < 2.0.6-3

%description
ttmkfdir is a utility which is used to create fonts.scale files in
directories full of TrueType fonts in order to prepare them for use
by the font server.

%prep
%setup -q

%build
make OPTFLAGS="$RPM_OPT_FLAGS"

%install
rm -rf $RPM_BUILD_ROOT

%makeinstall DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README
%{_bindir}/ttmkfdir

%changelog
* Thu Jan 23 2003 Yu Shao <yshao@redhat.com> 3.0.9-1
- added freetype-devel build requirement #82468

* Mon Jan 20 2003 Yu Shao <yshao@redhat.com> 3.0.8-1
- revert additional-entries to 0 #82125

* Wed Jan 15 2003 Yu Shao <yshao@redhat.com> 3.0.7-1
- set default value of additional-entries to 1

* Mon Jan 13 2003 Yu Shao <yshao@redhat.com> 3.0.6-1
- added iso8859-13 support from Nerijus Baliunas #77289
- added README 

* Wed Jan 8 2003 Yu Shao <yshao@redhat.com> 3.0.5-1
- encoding.l fix and ttc support patch 
- default read system encodings.dir instead of the one
- in current directory

* Wed Dec 18 2002 Yu Shao <yshao@redhat.com> 3.0.4-1
- make ttmkfdir keep silent with FIRSTINDEX. #61769

* Wed Dec 18 2002 Yu Shao <yshao@redhat.com> 3.0.3-1
- Applied new patches to make ttmkfdir provide more infomation when meets 
- bad fonts

* Mon Dec  9 2002 Mike A. Harris <mharris@devel.capslock.lan> 3.0.2-1
- Changed the nonstandard RPM Group from System/Utilities to Applications/System
- Added a new Makefile install target, and changed specfile to use makeinstall

* Mon Dec  9 2002 Mike A. Harris <mharris@devel.capslock.lan> 3.0.1-1
- Imported ttmkfdir into CVS on cvs.devel and applied all patches to CVS
- Removed patches from spec file
- Rewrote Makefile, now uses freetype-config to autodetect CFLAGS and libs,
  allowing a lot of spec file cleanups.  Added new targets for tagging CVS,
  making tarball, and making srpm.

* Mon Dec  2 2002 Mike A. Harris <mharris@devel.capslock.lan> 3.0.0-2
- Added Conflicts for prior packages which contained ttmkfdir

* Fri Nov 29 2002 Mike A. Harris <mharris@devel.capslock.lan> 3.0.0-1
- Initial build.  Basically just renamed our existing ttmkfdir to version
  3.0.0 to differentiate it.  It's the same old thing as before, but is
  likely going to move to CVS for easier development.
