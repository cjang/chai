include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	Stmt.o \
	StmtBarrier.o \
	StmtCompound.o \
	StmtCreateData.o \
	StmtExtension.o \
	StmtExtensionAuto.o \
	StmtGatherAuto.o \
	StmtIndex.o \
	StmtLiteral.o \
	StmtMatmul.o \
	StmtMatmulAuto.o \
	StmtMatmulBase.o \
	StmtReadData.o \
	StmtReduce.o \
	StmtRepeat.o \
	StmtSendData.o \
	StmtSingle.o \
	VisitStmt.o

INTERNAL_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp)

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
