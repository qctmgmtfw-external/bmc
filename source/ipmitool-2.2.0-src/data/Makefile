#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n

#---------------------- Change according to your files ----------------------------
SRC = lib/helper.c lib/ipmi_sdr.c lib/ipmi_sol.c lib/ipmi_tsol.c lib/ipmi_isol.c 
SRC += lib/ipmi_sel.c lib/ipmi_pef.c lib/ipmi_lanp.c lib/ipmi_fru.c lib/ipmi_chassis.c 
SRC += lib/ipmi_mc.c lib/log.c lib/dimm_spd.c lib/ipmi_sensor.c lib/ipmi_channel.c 
SRC += lib/ipmi_event.c lib/ipmi_session.c lib/ipmi_strings.c lib/ipmi_user.c 
SRC += lib/ipmi_raw.c lib/ipmi_oem.c lib/ipmi_sunoem.c lib/ipmi_fwum.c lib/ipmi_picmg.c 
SRC += lib/ipmi_main.c lib/ipmi_firewall.c lib/ipmi_kontronoem.c lib/ipmi_hpmfwupg.c 
SRC += lib/ipmi_sdradd.c lib/ipmi_ekanalyzer.c lib/ipmi_gendev.c 
SRC += src/ipmitool.c src/ipmishell.c src/plugins/ipmi_intf.c src/plugins/lan/lan.c 
SRC += src/plugins/lan/auth.c src/plugins/lan/md5.c src/plugins/lanplus/lanplus.c 
SRC += src/plugins/lanplus/lanplus_strings.c src/plugins/lanplus/lanplus_crypt.c 
SRC += src/plugins/lanplus/lanplus_dump.c src/plugins/lanplus/lanplus_crypt_impl.c

TARGET = ipmitool

# Add the Correspoding includes
CFLAGS += -I${SPXINC}/libipmi/
CFLAGS += -I${SPXINC}/global/
CFLAGS += -I${SPXINC}/ipmi/
CFLAGS += -I${SPXINC}/encryption
CFLAGS += -I. -I./include/
CFLAGS += -I${TARGETDIR}/usr/include/
CFLAGS += -DHAVE_CONFIG_H -DHAVE_PATHS_H -DHAVE_TERMIOS_H

# Add the Corresponding library
# Order of Libraries: AMI Libraries, Standard Libraries, and then our external libraries
# Other order does not work
LIBS += -L${SPXLIB}/ipmiparams -lipmiparams
LIBS += -L${SPXLIB}/dbgout -ldbgout
LIBS    += -L${SPXLIB}/encryption -lencryption
LIBS += -L${SPXLIB}/ipmi -lipmi
LIBS += -Wl,-L${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB} -lcrypto

LIBS += -lm  -lssl

include ${TOOLDIR}/rules/Rules.make.apps

#----------------------------------------------------------------------------------------
