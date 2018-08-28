
C_SRC = ./edit/edit.c ./edit/eloop.c ./edit/common.c ./edit/trace.c ./edit/wpa_debug.c ./edit/wpabuf.c ./edit/os_internal.c ./edit/cli.c ./packet/packet.c ./packet/mac_addr.c

C_OBJS = edit.o eloop.o common.o trace.o wpa_debug.o wpabuf.o os_internal.o cli.o packet.o mac_addr.o

CPP_SRC = gt_server.cpp gt_console.cpp 

CPP_OBJS = gt_server.o gt_console.o

all: c_objs cpp_objs ghosttunnel


c_objs:
	gcc -c $(C_SRC)

cpp_objs:
	g++ -c $(CPP_SRC)

ghosttunnel:
	$(MAKE) -C libwifi || exit 1;
	g++ -o ghosttunnel ghosttunnel.cpp $(C_OBJS) $(CPP_OBJS) $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0) -L./libwifi/kismet -L./libwifi/aircrack-ng -losdep -lkismetlinuxwifi -lm -lpthread


install:



clean:
	rm -f ghosttunnel *.o
