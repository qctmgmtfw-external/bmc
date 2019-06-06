#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n

#---------------------- Change according to your files ----------------------------
SRC 	=   cmdhndlr.c hid_cmdhandlr.c hid_usb.c encrypt.c reader.c sync.c utils.c # Source Files
SRC	+=  threadmgr.c sessionmgr.c avse.c cmworker.c conmgr.c main.c # Source Files
SRC	+=  video_misc.c	# Source Files
TARGET  =   adviserd 		# Output file

CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/dynamicdrv
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/ipmi
CFLAGS += -I${SPXINC}/libipmi
CFLAGS += -I${SPXINC}/userm
CFLAGS += -I${SPXINC}/usbe
CFLAGS += -I${SPXINC}/compress
CFLAGS += -I${SPXINC}/unix
CFLAGS += -I${SPXINC}/vmedia
CFLAGS += -I${SPXINC}/advisercfg
CFLAGS += -I${SPXINC}/videorecordcfg
CFLAGS += -I${SPXINC}/autovideorecord
CFLAGS += -I${SPXINC}/blowfish
CFLAGS += -I${SPXINC}/encryption
CFLAGS += -I${SPXINC}/token
CFLAGS += -I${SPXINC}/adviser
CFLAGS += -I${SPXINC}/procmanager
CFLAGS += -I${SPXINC}/ncml
CFLAGS += -I$(SPXINC)/fmh
CFLAGS += -I$(SPXINC)/flash
CFLAGS += -I$(SPXINC)/network
CFLAGS += -I$(SPXINC)/hostname
CFLAGS += -I$(SPXINC)/unix
CFLAGS += -I${SPXINC}/featuredefine
CFLAGS += -I${SPXINC}/userprivilege
CFLAGS += -I${SPXINC}/userprivilegepdk
CFLAGS += -I${SPXINC}/Extendedpriv
CFLAGS += -I${SPXINC}/user_auth
CFLAGS += -I${SPXINC}/flash
CFLAGS += -I${SPXINC}/racsessioninfo
CFLAGS += -I${SPXINC}/user_auth
CFLAGS += -I${SPXINC}/userprivilegepdk
CFLAGS += -I${SPXINC}/userm

# Add the Corresponding library
LIBS   += -L${SPXLIB}/video -lvideo
LIBS   += -L${SPXLIB}/dynamicdrv -ldynamicdrv
LIBS   += -L${SPXLIB}/compress -lcompress
LIBS   += -L${SPXLIB}/dbgout -ldbgout
LIBS   += -L${SPXLIB}/advisercfg -ladvisercfg
LIBS   += -L${SPXLIB}/videorecordcfg -lvideorecordcfg
LIBS   += -L${SPXLIB}/vmedia -lvmedia
LIBS   += -L${SPXLIB}/blowfish -lblowfish
LIBS   += -L${SPXLIB}/unix -lunix
LIBS   += -L${SPXLIB}/iniparser -liniparser
LIBS   += -L${SPXLIB}/racsessioninfo -lracsessioninfo
LIBS   += -L${SPXLIB}/token -ltoken
LIBS   += -L${SPXLIB}/procmanager -lprocmanager
LIBS   += -L${SPXLIB}/ncml -lncml
LIBS   += -L${SPXLIB}/i2c -li2c
LIBS   += -L${SPXLIB}/ipmiparams -lipmiparams
LIBS   += -L${SPXLIB}/ipmi -lipmi
LIBS   += -L${SPXLIB}/userprivilege -luserprivilege
LIBS   += -L${SPXLIB}/userprivilegepdk -luserprivilegepdk
LIBS   += -L${SPXLIB}/user_auth -luserauth
LIBS   += -L${SPXLIB}/Extendedpriv -lExtendedpriv
LIBS  	+= -L${SPXLIB}/featuredefine -lfeaturedefine
LIBS 	+= -L${SPXLIB}/flash -lflash
LIBS   +=  ${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB}/libcrypto.so
LIBS   +=  ${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB}/libz.so
LIBS   +=  ${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB}/libssl.so
LIBS   += -rdynamic
LIBS   += -lpthread -ldl
LIBS    += -L${SPXLIB}/racsessioninfo -lracsessioninfo

include ${TOOLDIR}/rules/Rules.make.apps

#----------------------------------------------------------------------------------------

