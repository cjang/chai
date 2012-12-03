include ../../Makefile.config

AMD_LDFLAGS = -L$(AMD_SDK)/lib/$(CPU_HOST_ARCH) -lOpenCL $(GLOBAL_LDFLAGS)
INTEL_LDFLAGS = -L$(INTEL_SDK)/usr/$(LIB_ARCH_DIR) -lOpenCL $(GLOBAL_LDFLAGS)
NVIDIA_LDFLAGS = -L$(NVIDIA_CUDA)/$(LIB_ARCH_DIR) -lOpenCL $(GLOBAL_LDFLAGS)

include ../../Makefile.common

.SUFFIXES : .amd .nvidia .intel

.o.amd :
	$(GNU_CXX) -o $@ $< $(AMD_LDFLAGS) -lchai -lchaiAMD -lm

.o.intel :
	$(GNU_CXX) -o $@ $< $(INTEL_LDFLAGS) -lchai -lchaiINTEL -lm

.o.nvidia :
	$(GNU_CXX) -o $@ $< $(NVIDIA_LDFLAGS) -lchai -lchaiNVIDIA -lm

default :
	@if [ $(AMD_SDK) ]; then make amd; fi
	@if [ $(INTEL_SDK) ]; then make intel; fi
	@if [ $(NVIDIA_CUDA) ]; then make nvidia; fi

link_to_devspec :
	if [ ! -f devspec.cfg ]; then ln -s ../../devspec.cfg; fi

BUILD_TARGETS = \
	link_to_devspec \
	boilerplate_chai.x \
	boilerplate_inline_cl.x \
	boilerplate_interop_cl.x \
	boilerplate_opencl.x

amd : $(BUILD_TARGETS:.x=.amd)

nvidia : $(BUILD_TARGETS:.x=.nvidia)

intel : $(BUILD_TARGETS:.x=.intel)

clean :
	rm -f *.o *.amd *.nvidia *.intel devspec.cfg
