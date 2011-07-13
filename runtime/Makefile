include ../Makefile.common

LIBRARY_OBJECT_CODE = \
	ArrayClient.o \
	DeviceBase.o \
	DeviceMap.o \
	Executor.o \
	Interpreter.o \
	Scheduler.o \
	Translator.o

LIBRARY_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp)

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(LIBRARY_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
