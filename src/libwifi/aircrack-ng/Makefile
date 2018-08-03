

SUBDIRS = osdep

#all:
#	@for dir in $(SUBDIRS); \
#		do $(MAKE) -C $$dir || exit 1; \
#	done \

include common.mak


RTAP		= radiotap

LIB		= libosdep.a 
CFLAGS		+= $(PIC) -I.. $(LIBAIRPCAP)

OBJS_COMMON	= ./osdep/network.o ./osdep/file.o
OBJS		= osdep_wifi_transmit.o ./osdep/osdep.o $(OBJS_COMMON)




OBJS_LINUX	= $(OBJS) ./osdep/linux.o ./osdep/linux_tap.o ./osdep/radiotap/radiotap.o ./osdep/common.o


all: 
	@echo Building for $(OSNAME)
	@$(MAKE) .os.$(OSNAME)

.os.Linux: $(OBJS_LINUX)
	$(AR) crvs $(LIB) $(OBJS_LINUX)



install:


clean:
	@for dir in $(SUBDIRS); \
		do $(MAKE) -C $$dir clean || exit 1; \
	done \

	rm -f *.o *.so *.a *~