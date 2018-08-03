INSTGRP 	?= "root"
MANGRP 		?= "man"
INSTUSR 	?= "root"

BLDHOME		= .

CXX			= g++
CC			= gcc
LD			= g++
CCLD		= gcc

LDFLAGS		+=  -fopenmp -fopenmp
CFLAGS		+= -Wall -g -pthread -I/usr/include/libnm -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libnl3  -fopenmp -pthread 
CXXFLAGS 	+= -Wall -g -I. -g -O2 -pthread
CPPFLAGS 	+=  -fopenmp 

INSTALL		= /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = @INSTALL_STRIP_PROGRAM@

LIBS		+= -lcap -lncurses  -ldw -ldl -lz -lpcre -lsqlite3
KSLIBS		+=  -lmicrohttpd -lncurses -ltinfo -lpthread -lprotobuf -pthread -lpthread

PTHREADLIBS = -lpthread
CAPLIBS		= -lcap
NETLINKLIBS	= -lnl-genl-3 -lnl-3
AIRCAPLIBS	= 

PROTOLIBS 	= -lprotobuf -pthread -lpthread
PROTOCFLAGS	= -pthread
PROTOCBIN 	?= protoc

PROTOCLIBS	= -lprotobuf-c
PROCOCCFLAGS	= 
PROTOCCBIN	?= protoc-c

PTHREADCFLAGS = -pthread
NMFLAGS		= -pthread -I/usr/include/libnm -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
NMLIBS		= -lnm -lgio-2.0 -lgobject-2.0 -lglib-2.0

PCAPLIBS	= -lpcap

LIBMLIB		= -lm

SUIDGROUP 	= kismet

DATASOURCE_LIBS	+= $(CAPLIBS) -lpthread -lprotobuf-c -lm

PYTHON2		?= python2


prefix 		= /usr/local
exec_prefix = ${prefix}
plugindir	= ${exec_prefix}/lib/kismet
ETC			= ${DESTDIR}${prefix}/etc
BIN			= ${DESTDIR}${exec_prefix}/bin
SHARE 		= ${DESTDIR}${prefix}/share/kismet/
MAN 		= ${DESTDIR}${prefix}/share/man
LIB			= ${DESTDIR}${exec_prefix}/lib
WAV 		= ${DESTDIR}${prefix}/share/kismet/wav/
HTTPD		= ${DESTDIR}${prefix}/share/kismet/httpd/

PLUGINLDFLAGS +=   -fopenmp -fopenmp

