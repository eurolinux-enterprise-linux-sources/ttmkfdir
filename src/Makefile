# The original author of this version of ttmkfdir (formerly known as
# ttmkfdir2) is currently unknown to me, however if someone lets me know
# I will add credit here.
#
# I just did a major revamp on this Makefile to clean it up for development
# purposes and make it play nice with CVS and RPM building.  I've bumped the
# version number from "2" to 3.0.0 to indicate it is being actively worked
# on once again, and is now managed in CVS.
#       Mike A. Harris <mharris@redhat.com> - Dec 8, 2002
#
# The current version is maintained by Yu Shao <yshao@redhat.com>, and bug
# reports can be filed in Red Hat bugzilla

NAME=ttmkfdir
VERSION=$(shell rpm -q --qf "%{version}\n" --specfile $(NAME).spec | head -1)
RELEASE=$(shell rpm -q --qf "%{release}\n" --specfile $(NAME).spec | head -1)

CVSTAG = r$(subst .,_,$(VERSION))
CVSROOT = $(shell cat CVS/Root)

# Autodetec the proper command used to build RPM packages
RPMBUILD=$(shell [ -x /usr/bin/rpmbuild ] && echo rpmbuild || echo rpm)

FREETYPE_INCL=$(shell freetype-config --cflags)
FREETYPE_LIB=$(shell freetype-config --libs)

OPTFLAGS=
DEBUG=-ggdb
CXX=g++
CXXFLAGS=-Wall -pedantic $(FREETYPE_INCL) $(DEBUG) $(OPTFLAGS)
LDFLAGS=$(FREETYPE_LIB) $(DEBUG)

DESTDIR=
PREFIX=/usr
BINDIR=$(PREFIX)/bin

.PHONY: clean

all: ttmkfdir

.cpp.o:
	libtool --mode=compile $(CXX) $(CXXFLAGS) -o $@ -c $^

ttmkfdir: ttmkfdir.o directory.o commandline.o ttf.o encoding.o parser.o builtin.o
	libtool --mode=link $(CXX) -o $@ $^ $(LDFLAGS)

parser.cpp: encoding.l
	    flex -i -8 -o$@ $<

install:
	mkdir -p $(DESTDIR)$(BINDIR)
	install -m 755 ttmkfdir $(DESTDIR)$(BINDIR)

clean:
	-rm -f *.o *.lo *~ ttmkfdir core parser.cpp
	rmdir .libs

tag:
	cvs -d $(CVSROOT) tag $(CVSTAG)

force-tag:
	cvs -d $(CVSROOT) tag -F $(CVSTAG)

archive:
	@rm -rf /tmp/$(NAME)-$(VERSION) /tmp/$(NAME)
	@cd /tmp; cvs -d $(CVSROOT) export -r $(CVSTAG) $(NAME)
	@mv /tmp/$(NAME) /tmp/$(NAME)-$(VERSION)
	@dir=$$PWD; cd /tmp; tar cf - $(NAME)-$(VERSION) | bzip2 -9 - > $$dir/$(NAME)-$(VERSION).tar.bz2
	@rm -rf /tmp/$(NAME)-$(VERSION)
	@echo "Wrote: $(NAME)-$(VERSION).tar.bz2"

tarball: archive

srpm:
	@echo "The general purpose 'srpm' target is not currently implemented"

# Private target, if you need to, copy it to a new target first and then
# modify it <mharris@redhat.com>
srpm-mh:
	@echo Creating src.rpm
	@mkdir -p $(HOME)/rpmbuild/$(NAME)-$(VERSION)
	@mv $(NAME)-$(VERSION).tar.bz2 $(HOME)/rpmbuild/$(NAME)-$(VERSION)/
	@cp $(NAME).spec $(HOME)/rpmbuild/$(NAME)-$(VERSION)/
	@pushd $(HOME)/rpmbuild/$(NAME)-$(VERSION) &> /dev/null ; $(RPMBUILD) --nodeps -bs $(NAME).spec ; popd &> /dev/null
	@echo SRPM is: $(HOME)/rpmbuild/SRPMS/$(NAME)-$(VERSION)-$(RELEASE).src.rpm


