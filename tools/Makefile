include ../Makefile.config

AMD_LDFLAGS = -L$(AMD_SDK)/lib/$(CPU_HOST_ARCH) -lOpenCL $(GLOBAL_LDFLAGS)
NVIDIA_LDFLAGS = -L$(NVIDIA_CUDA)/$(LIB_ARCH_DIR) -lOpenCL $(GLOBAL_LDFLAGS)
INTEL_LDFLAGS = -L$(INTEL_SDK)/usr/$(LIB_ARCH_DIR) -lOpenCL $(GLOBAL_LDFLAGS)

include ../Makefile.common

.SUFFIXES : .amd .nvidia .intel

.o.amd :
	$(GNU_CXX) -o $@ $< $(AMD_LDFLAGS) -lchai -lchaiAMD -lm

.o.nvidia :
	$(GNU_CXX) -o $@ $< $(NVIDIA_LDFLAGS) -lchai -lchaiNVIDIA -lm

.o.intel :
	$(GNU_CXX) -o $@ $< $(INTEL_LDFLAGS) -lchai -lchaiINTEL -lm

BUILD_TARGETS = \
	ATmatmul.x \
	ATmatvec.x \
	LSdevice.x

default :
	@if [ $(AMD_SDK) ]; then make amd; fi
	@if [ $(INTEL_SDK) ]; then make intel; fi
	@if [ $(NVIDIA_CUDA) ]; then make nvidia; fi

amd : $(BUILD_TARGETS:.x=.amd)

nvidia : $(BUILD_TARGETS:.x=.nvidia)

intel : $(BUILD_TARGETS:.x=.intel)

install : default
	@if [ $(AMD_SDK) ]; then \
	  install --mode=555 $(BUILD_TARGETS:.x=.amd) $(BIN_DIR); \
	fi
	@if [ $(INTEL_SDK) ]; then \
	  install --mode=555 $(BUILD_TARGETS:.x=.intel) $(BIN_DIR); \
	fi
	@if [ $(NVIDIA_CUDA) ]; then \
	  install --mode=555 $(BUILD_TARGETS:.x=.nvidia) $(BIN_DIR); \
	fi

clean :
	rm -f *.o *.amd *.nvidia *.intel
