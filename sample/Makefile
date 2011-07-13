ATI_SDK = $(ATISTREAMSDKROOT)
ATI_CFLAGS = -I$(ATI_SDK)/include $(GLOBAL_CFLAGS)
ATI_LDFLAGS = -L$(ATI_SDK)/lib/x86_64 -lOpenCL $(GLOBAL_LDFLAGS)

NVIDIA_SDK = $(NVIDIACUDASDKROOT)
NVIDIA_CFLAGS = -I$(NVIDIA_SDK)/include $(GLOBAL_CFLAGS)
NVIDIA_LDFLAGS = -L$(NVIDIA_SDK)/lib64 -lOpenCL $(GLOBAL_LDFLAGS)

include ../Makefile.common

.SUFFIXES : .x .ati .nvidia

.o.x :
	$(GNU_CXX) -o $@ $< $(GLOBAL_LDFLAGS) -lchai -lm

.o.ati :
	$(GNU_CXX) -o $@ $< $(ATI_LDFLAGS) -lchai -lchaiATI -lm

.o.nvidia :
	$(GNU_CXX) -o $@ $< $(NVIDIA_LDFLAGS) -lchai -lchaiNVIDIA -lm

BUILD_TARGETS = \
	cg.x \
	kirch.x \
	loopsum.x \
	mbrot.x \
	monte.x \
	pi.x \
	sum.x

default :
	@echo "Do one or both:"
	@echo "    make ati"
	@echo "    make nvidia"

ati : $(BUILD_TARGETS:.x=.ati)

nvidia : $(BUILD_TARGETS:.x=.nvidia)

para : ompcg.ati ompmatmul.ati ompsum.ati pthsum.ati

clean :
	rm -f *.o *.x *.ati *.nvidia
