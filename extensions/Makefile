include ../Makefile.config

STATIC_ARCHIVE_LIBRARY = libchaiExt.a
SHARED_OBJECT_LIBRARY = libchaiExt.so

LIBRARY_OBJECT_CODE = \
	chaiExt.o \
	ExtNOP.o \
	InterpNOP.o \
	TransNOP.o

EXTERNAL_HEADER_FILES = \
	chaiExt.h

USE_CFLAGS=$(SHARED_CFLAGS)

include ../Makefile.common

all :
	@if [ $(SHARED_LDFLAGS) ]; \
	then \
	  make $(SHARED_OBJECT_LIBRARY); \
	else \
	  make $(STATIC_ARCHIVE_LIBRARY); \
	fi

install : all
	install --mode=644 $(EXTERNAL_HEADER_FILES) $(EXTERNAL_HEADER_DIR)
	@if [ $(SHARED_LDFLAGS) ]; \
	then \
	  install $(SHARED_OBJECT_LIBRARY) $(LIB_DIR); \
	else \
	  install $(STATIC_ARCHIVE_LIBRARY) $(LIB_DIR); \
	fi

clean :
	rm -f *.o $(STATIC_ARCHIVE_LIBRARY) $(SHARED_OBJECT_LIBRARY)
