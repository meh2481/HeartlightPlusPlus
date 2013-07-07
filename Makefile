# Entry point for Heartlight++ makefiles
# Based loosely off FreeImage makefiles - thanks, doods
# Default to 'make -f Makefile.unix' for Linux and for unknown OS. 
#
OS = $(shell uname)
MAKEFILE = unix

ifeq ($(OS), Darwin)
    MAKEFILE = osx
endif
ifeq ($(OS), windows32)
    MAKEFILE = win
endif

default:
	$(MAKE) -f Makefile.$(MAKEFILE) 

all:
	$(MAKE) -f Makefile.$(MAKEFILE) all 

clean:
	$(MAKE) -f Makefile.$(MAKEFILE) clean 

