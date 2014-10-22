.PHONY : clean

LIBTOOL_FILES = config.guess config.sub install-sh ltmain.sh m4
ACLOCAL_FILES = autom4te.cache aclocal.m4
AUTOCONF_FILES = configure
AUTOMAKE_FILES = depcomp missing Makefile.in src/Makefile.in

MPFD = libmpfd-2.0.0
_MPFD = libmpfd-2.0-0

all :
	cd $(MPFD); \
	libtoolize -c; \
	aclocal; \
	autoconf; \
	automake -a -c

pack :
	rm -f $(MPFD).tar.xz
	tar cvf $(MPFD).tar $(MPFD); \
	xz $(MPFD).tar

clean :
	rm -f $(MPFD).tar.xz
	cd $(MPFD); \
	rm -rf $(LIBTOOL_FILES) \
		$(ACLOCAL_FILES) \
		$(AUTOCONF_FILES) \
		$(AUTOMAKE_FILES)

deb :
	rm -rf ./$(_MPFD); \
	rsync -avz --exclude=.svn ./debian-skel/ $(_MPFD)
	#
	# Copy in the libs
	rsync -avz $(MPFD)/libmpfd/.libs/   $(_MPFD)/usr/lib; \
	strip $(_MPFD)/usr/lib/*.so
	chmod 644 $(_MPFD)/usr/lib/*.so
	#
	# Copy in the man pages
	cp $(MPFD)/man/*\.[1-8] $(_MPFD)/usr/share/man/man1; \
	chmod 644 $(_MPFD)/usr/share/man/man1/*
	gzip -9 $(_MPFD)/usr/share/man/man1/*; \
	#
	# Copy in the copyright notice
	cp $(MPFD)/doc/copyright $(_MPFD)/usr/share/doc/libmpfd
	#
	# Copy in the changelogs, chmod them and gzip them
	cp $(MPFD)/doc/changelog $(_MPFD)/usr/share/doc/libmpfd/changelog.Debian
	chmod 644 $(_MPFD)/usr/share/doc/libmpfd/*
	gzip --best $(_MPFD)/usr/share/doc/libmpfd/changelog.Debian
	#
	# Run the packaging
	chmod 644  $(_MPFD)/DEBIAN/control
	fakeroot dpkg-deb --build $(_MPFD)
	lintian $(_MPFD).deb


