include ../Makefile.common

LIBRARY_OBJECT_CODE = \
	peakstream.o

LIBRARY_HEADER_FILES =

PUBLIC_HEADER_FILES = \
	peakstream.h

all : $(LIBRARY_OBJECT_CODE)

#	install --mode=644 $(LIBRARY_HEADER_FILES) $(INTERNAL_HEADER_DIR)
install : all
	install --mode=644 $(PUBLIC_HEADER_FILES) $(HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
