include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	chai.o

EXTERNAL_HEADER_FILES = \
	chai.h

all : preinstall $(LIBRARY_OBJECT_CODE)

preinstall :
	install --mode=644 $(EXTERNAL_HEADER_FILES) $(EXTERNAL_HEADER_DIR)

install : all
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
