
#gcc -o test_main test_main.c -L./kismet -L./aircrack-ng/osdep -losdep -lkismetlinuxwifi -lnl-3 -lnl-genl-3 -lm


SUBDIRS = kismet aircrack-ng

all:
	@for dir in $(SUBDIRS); \
		do $(MAKE) -C $$dir || exit 1; \
	done \



test_main:


clean:
	@for dir in $(SUBDIRS); \
		do $(MAKE) -C $$dir clean || exit 1; \
	done \

	rm -f *.o *.so *.a *~


