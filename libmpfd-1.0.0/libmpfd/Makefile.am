AM_CPPFLAGS = -I.. $(all_includes)
 
lib_LTLIBRARIES = libmpfd.la
 
libmpfd_la_LIBADD = $(LIB_KPARTS)
libmpfd_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -no-undefined
libmpfd_la_SOURCES = Exception.cpp Field.cpp Parser.cpp
otherincludedir = $(includedir)/libmpfd
pkginclude_HEADERS = Parser.h Field.h Exception.h
 
METASOURCES = AUTO
