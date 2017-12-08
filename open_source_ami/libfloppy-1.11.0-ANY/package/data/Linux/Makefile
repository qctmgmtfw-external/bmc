
ifeq (${shell arch},x86_64)
SUBDIRS = Linux_x86_64
else
SUBDIRS = Linux_x86_32
endif

# Add all subdirs
all: subdirs

subdirs:
	@echo -e "\n***********************\nFound ${shell arch} architecture\n***********************"
	 for n in $(SUBDIRS); do $(MAKE) -C $$n all  || exit 1; done

clean:
	@echo -e "\n***********************\nFound ${shell arch} architecture\n***********************"
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done

