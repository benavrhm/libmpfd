.PHONY : clean

LIBTOOL_FILES = config.guess config.sub install-sh ltmain.sh m4
ACLOCAL_FILES = autom4te.cache aclocal.m4
AUTOCONF_FILES = configure
AUTOMAKE_FILES = depcomp missing Makefile.in src/Makefile.in

MPFD = libmpfd-2.0.0

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
