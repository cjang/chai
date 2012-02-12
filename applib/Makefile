include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	ParseArgs.o \
	Watchdog.o

INTERNAL_HEADER_FILES = \
	Watchdog.hpp

EXTERNAL_HEADER_FILES = \
	ParseArgs.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install --mode=644 $(EXTERNAL_HEADER_FILES) $(EXTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
