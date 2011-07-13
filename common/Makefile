include ../Makefile.common

LIBRARY_OBJECT_CODE = \
	BackMem.o \
	ClientTrace.o \
	DeviceMem.o \
	FrontMem.o \
	MemManager.o \
	SingleNut.o \
	SingleTrace.o \
	VectorNut.o \
	VectorTrace.o

LIBRARY_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp) \
	VisitAst.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(LIBRARY_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
