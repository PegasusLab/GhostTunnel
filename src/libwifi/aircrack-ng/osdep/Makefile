#MDK_ROOT	= ./..
#include $(MDK_ROOT)/common.mak

RTAP		= radiotap

#LIB		= libosdep.a 
#CFLAGS		+= $(PIC) -I.. $(LIBAIRPCAP)

#OBJS_COMMON	= network.o file.o
#OBJS		= osdep.o $(OBJS_COMMON)


#OBJS_LINUX	= $(OBJS) linux.o linux_tap.o radiotap/radiotap.o common.o


all: 
#	@echo Building for $(OSNAME)
#	@$(MAKE) .os.$(OSNAME)

#.os.Linux: $(OBJS_LINUX)
#	$(AR) crv $(LIB) $(OBJS_LINUX)
#	$(RANLIB) $(LIB)
#	touch $(@)

install: all

uninstall:

clean: 
	$(MAKE) -C $(RTAP) clean
	rm -f $(LIB) *.o .os.*
