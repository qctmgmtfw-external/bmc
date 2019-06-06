#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n

#---------------------- Change according to your files ----------------------------
srcdir = .
sysconfdir = /conf
localdbdir = /conf

CFLAGS= -g -O2 -I$(srcdir) -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DYYTEXT_POINTER=1 -DHAVE_GETADDRINFO=1 -DHAVE_GETNAMEINFO=1 -DHAVE_GETIFADDRS=1 -DHAVE_IF_NAMETOINDEX=1 -DHAVE_DAEMON=1 -DHAVE_WARNX=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYSLOG_H=1 -DHAVE_UNISTD_H=1 -DHAVE_IFADDRS_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_STRUCT_TM_TM_ZONE=1 -DHAVE_TM_ZONE=1 -DHAVE_SIG_ATOMIC_T=1 -DGETPGRP_VOID=1 -DSETPGRP_VOID=1 -DRETSIGTYPE=void -DHAVE_MKTIME=1 -DHAVE_SELECT=1 -DHAVE_SOCKET=1 -DHAVE_ANSI_FUNC=1 -DHAVE_TAILQ_FOREACH_REVERSE_OLD=1 -DHAVE_STDARG_H=1  -DSYSCONFDIR=\"${sysconfdir}\" \
-DLOCALDBDIR=\"${localdbdir}\" -D_GNU_SOURCE -DYY_NO_INPUT

GENSRCS = cfparse.c cftoken_lex.yy.c

CLIENTOBJS = dhcp6c.c common.c config.c prefixconf.c dhcp6c_ia.c timer.c \
	dhcp6c_script.c if.c base64.c auth.c dhcp6_ctl.c addrconf.c lease.c

LIBOBJS = missing/strlcpy.c missing/strlcat.c missing/arc4random.c

SRC = $(GENSRCS) $(CLIENTOBJS) $(LIBOBJS)
TARGET = dhcp6c

include ${TOOLDIR}/rules/Rules.make.apps

#----------------------------------------------------------------------------------------
