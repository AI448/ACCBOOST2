
COMPILER=

ifneq ($(strip $(COMPILER)), )
	include $(COMPILER).mk
endif

all:
	$(MAKE) -C utility all
	$(MAKE) -C CONTAINER all
#	$(MAKE) -C VARIANT all
	
clean:
	$(MAKE) -C utility clean
	$(MAKE) -C CONTAINER clean
#	$(MAKE) -C VARIANT clean

