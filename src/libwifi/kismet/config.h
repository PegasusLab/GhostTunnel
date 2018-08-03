/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* backtrace stack support */
#define BACKWARD_HAS_BACKTRACE 0

/* backtrace stack support */
/* #undef BACKWARD_HAS_BACKTRACE_SYMBOL */

/* libbfd for stack printing */
/* #undef BACKWARD_HAS_BFD */

/* libdl for stack printing */
#define BACKWARD_HAS_DW 1

/* unwind stack support */
#define BACKWARD_HAS_UNWIND 1

/* system binary directory */
#define BIN_LOC "/usr/local/bin"

/* Able to build hackrf sweep source */
/* #undef BUILD_CAPTURE_HACKRF_SWEEP */

/* system data directory */
#define DATA_LOC "/usr/local/share"

/* cannot support backtrace dumping */
/* #undef DISABLE_BACKWARD */

/* Remove mutex deadlock timeout protection */
/* #undef DISABLE_MUTEX_TIMEOUT */

/* gcc version */
#define GCC_VERSION_MAJOR 7

/* gcc version */
#define GCC_VERSION_MINOR 0

/* gcc version */
#define GCC_VERSION_PATCH 0

/* libairpcap header */
/* #undef HAVE_AIRPCAP_H */

/* Define to 1 if you have the <bfd.h> header file. */
/* #undef HAVE_BFD_H */

/* BSD radiotap packet headers */
/* #undef HAVE_BSD_SYS_RADIOTAP */

/* kernel capability support */
#define HAVE_CAPABILITY 1

/* define if the compiler supports basic C++11 syntax */
#define HAVE_CXX11 1

/* Define to 1 if you have the <dwarf.h> header file. */
#define HAVE_DWARF_H 1

/* Define to 1 if you have the <elfutils/libdwfl.h> header file. */
#define HAVE_ELFUTILS_LIBDWFL_H 1

/* Define to 1 if you have the <elfutils/libdw.h> header file. */
#define HAVE_ELFUTILS_LIBDW_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Define to 1 if you have the <fftw3.h> header file. */
/* #undef HAVE_FFTW3_H */

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* system defines getopt_long */
#define HAVE_GETOPT_LONG 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <glob.h> header file. */
#define HAVE_GLOB_H 1

/* have __GNU_PARALLEL extensions */
#define HAVE_GNU_PARALLEL 1

/* GPS support will be built. */
#define HAVE_GPS 1

/* inttypes.h is present */
#define HAVE_INTTYPES_H 1

/* libairpcap win32 control lib */
/* #undef HAVE_LIBAIRPCAP */

/* Define to 1 if you have the `cap' library (-lcap). */
#define HAVE_LIBCAP 1

/* Curses terminal lib */
/* #undef HAVE_LIBCURSES */

/* Define to 1 if you have the <libhackrf/hackrf.h> header file. */
#define HAVE_LIBHACKRF_HACKRF_H 1

/* Define to 1 if you have the `ncurses' library (-lncurses). */
#define HAVE_LIBNCURSES 1

/* libnl netlink library */
#define HAVE_LIBNL 1

/* libnl netlink library */
/* #undef HAVE_LIBNL10 */

/* libnl-2.0 netlink library */
/* #undef HAVE_LIBNL20 */

/* libnl-3.0 netlink library */
#define HAVE_LIBNL30 1

/* libnl-2.0 netlink library */
/* #undef HAVE_LIBNLTINY */

/* libnltiny headers present */
/* #undef HAVE_LIBNLTINY_HEADERS */

/* NetworkManager interface library */
#define HAVE_LIBNM 1

/* libpcap packet capture lib */
#define HAVE_LIBPCAP 1

/* libpcre regex support */
#define HAVE_LIBPCRE 1

/* libsqlite3 database support */
#define HAVE_LIBSQLITE3 1

/* Define to 1 if you have the <libutil.h> header file. */
/* #undef HAVE_LIBUTIL_H */

/* Linux wireless iwfreq.flag */
#define HAVE_LINUX_IWFREQFLAG 1

/* Netlink works */
#define HAVE_LINUX_NETLINK 1

/* Linux wireless extentions present */
#define HAVE_LINUX_WIRELESS 1

/* local radiotap packet headers */
#define HAVE_LOCAL_RADIOTAP 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* microhttpd is present */
#define HAVE_MICROHTTPD_H 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* pcap/pcap.h */
/* #undef HAVE_PCAPPCAP_H */

/* pcapfileno-capable libwpcap */
/* #undef HAVE_PCAP_FILENO */

/* Selectablefd-capable libpcap */
#define HAVE_PCAP_GETSELFD 1

/* libpcap header */
#define HAVE_PCAP_H 1

/* Nonblocking-capable libpcap */
#define HAVE_PCAP_NONBLOCK 1

/* System has pipe2 */
#define HAVE_PIPE2 1

/* libpcap supports PPI */
#define HAVE_PPI 1

/* Define to 1 if you have the `pstat' function. */
/* #undef HAVE_PSTAT */

/* have pthread timelock */
/* #undef HAVE_PTHREAD_TIMELOCK */

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `setproctitle' function. */
/* #undef HAVE_SETPROCTITLE */

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* accept() takes type socklen_t for addrlen */
#define HAVE_SOCKLEN_T 1

/* Define to 1 if `stat' has the bug that it succeeds when given the
   zero-length file name argument. */
/* #undef HAVE_STAT_EMPTY_STRING_BUG */

/* stdint.h is present */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* System headers are there */
#define HAVE_SYSHEADERS 1

/* Define to 1 if you have the <sys/pstat.h> header file. */
/* #undef HAVE_SYS_PSTAT_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <unwind.h> header file. */
#define HAVE_UNWIND_H 1

/* __PROGNAME glibc macro available */
#define HAVE___PROGNAME 1

/* system library directory */
#define LIB_LOC "/usr/local/lib"

/* system state directory */
#define LOCALSTATE_DIR "/usr/local/var"

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#define LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* Able to use MHD_quiesce_daemon to shut down */
#define MHD_QUIESCE 1

/* we need to shim isnan */
/* #undef MISSING_STD_ISNAN */

/* we need to shim std snprintf */
/* #undef MISSING_STD_SNPRINTF */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* writeable argv type */
#define PF_ARGV_TYPE PF_ARGV_WRITEABLE

/* Libprelude support enabled */
/* #undef PRELUDE */

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* system config directory */
#define SYSCONF_LOC "/usr/local/etc"

/* Compiling for Cygwin */
/* #undef SYS_CYGWIN */

/* Compiling for OSX/Darwin */
/* #undef SYS_DARWIN */

/* Compiling for FreeBSD */
/* #undef SYS_FREEBSD */

/* Compiling for Linux OS */
#define SYS_LINUX 1

/* Compiling for NetBSD */
/* #undef SYS_NETBSD */

/* Compiling for OpenBSD */
/* #undef SYS_OPENBSD */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
/* proftpd argv stuff */
#define PF_ARGV_NONE        0
#define PF_ARGV_NEW     	1
#define PF_ARGV_WRITEABLE   2
#define PF_ARGV_PSTAT       3
#define PF_ARGV_PSSTRINGS   4

/* Maximum number of characters in the status line */
#define STATUS_MAX 1024

/* Stupid ncurses */
#define NCURSES_NOMACROS

/* Number of hex pairs in a key */
#define WEPKEY_MAX 32

/* String length of a key */
#define WEPKEYSTR_MAX ((WEPKEY_MAX * 2) + WEPKEY_MAX)

/* system min isn't reliable */
#define kismin(x,y) ((x) < (y) ? (x) : (y))
#define kismax(x,y) ((x) > (y) ? (x) : (y))

/* Timer slices per second */
#define SERVER_TIMESLICES_SEC 10

/* Max chars in SSID */
#define MAX_SSID_LEN    255

#ifndef _
#define _(x) x
#endif

