
machType = $(shell echo $$HOSTTYPE)
ifeq ($(machType),x86_64)
SUBDIRS = Mac_x86_64
else
SUBDIRS = Mac_x86_32
endif

# Add all subdirs
all: subdirs

subdirs:
	@echo -e "\n***********************\nFound $(machType) architecture\n***********************" 
	echo $(MAKE)
	for n in $(SUBDIRS); do $(MAKE) -C $$n all  || exit 1; done

clean:
	@echo -e "\n***********************\n Found $(machType) architecture\n***********************"
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done
