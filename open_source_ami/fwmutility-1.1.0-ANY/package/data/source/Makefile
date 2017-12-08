FMHCORE = ./fmhcore

CFLAGS  = -Wall -I$(FMHCORE)/src -m32 -static 
#CFLAGS += -DDEBUG					# Uncomment to enable debug
LFLAGS  = -m32 -L$(FMHCORE) -lfmhcore
RM      = rm -f

all: fwm

$(FMHCORE)/libfmhcore.a:
	@(make -C $(FMHCORE) all)

fwm: main.o cmd_list.o cmd_seprate.o cmd_generate.o cmd_replace.o $(FMHCORE)/libfmhcore.a
	@(echo "generating  application ...")
	@($(CC)  -o fwm main.o cmd_list.o cmd_seprate.o cmd_generate.o cmd_replace.o $(LFLAGS))


clean:
	@($(RM) fwm *.o)
	@(make -C $(FMHCORE) clean)


COMPILE.c=$(CC) $(CFLAGS) -c
.c.o:
	@./setlocalversion
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)

