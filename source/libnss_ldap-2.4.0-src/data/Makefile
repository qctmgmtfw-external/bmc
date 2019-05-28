DEBUG = n

LIBRARY_NAME = libnss_ldap

SRC= nss-ldap.c

CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/iniparser
CFLAGS += -I$(SPXINC)/pamhelper
CFLAGS += -I$(SPXINC)/user_auth
CFLAGS += -I$(SPXINC)/userm
CFLAGS += -I$(SPXINC)/userprivilegepdk
CFLAGS += -I$(SPXINC)/featuredefine
CFLAGS += -I${SPXINC}/userprivilege
CFLAGS += -I${SPXINC}/ldapconf
CFLAGS += -I${TARGETDIR}/usr/include

LIBS += -L$(SPXLIB)/dbgout -Wl,-rpath /usr/local/lib -ldbgout
LIBS += -L$(SPXLIB)/iniparser -Wl,-rpath /usr/local/lib -liniparser
LIBS += -L$(SPXLIB)/pamhelper -lpamhelper
LIBS += -L$(SPXLIB)/user_auth -luserauth
LIBS += -L$(SPXLIB)/userprivilege -luserprivilege
LIBS += -L${SPXLIB}/ldapconf -lldapconf

include ${TOOLDIR}/rules/Rules.make.libs

