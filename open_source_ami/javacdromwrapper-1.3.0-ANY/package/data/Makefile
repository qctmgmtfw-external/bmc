
ifeq (${shell arch},x86_64)
MAKE = make -f Makefile.x86_64
else
ifeq (${shell echo $$HOSTTYPE},x86_64)
MAKE = make -f Makefile.x86_64
else
MAKE = make -f Makefile.x86
endif
endif

# Add all subdirs
all: subdirs

subdirs:
	@echo -e "\n***********************\nFound ${shell arch} architecture\n***********************" 
	$(MAKE)

clean:
	@echo -e "\n***********************\nFound ${shell arch} architecture\n***********************"
	$(MAKE) clean

