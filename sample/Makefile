include ../Makefile.config

AMD_LDFLAGS = -L$(AMD_SDK)/lib/$(CPU_HOST_ARCH) -lOpenCL $(GLOBAL_LDFLAGS)
INTEL_LDFLAGS = -L$(INTEL_SDK)/usr/$(LIB_ARCH_DIR) -lOpenCL $(GLOBAL_LDFLAGS)
NVIDIA_LDFLAGS = -L$(NVIDIA_CUDA)/$(LIB_ARCH_DIR) -lOpenCL $(GLOBAL_LDFLAGS)

include ../Makefile.common

EXTENSION_LIBRARY = ../_install/lib/libchaiExt.a

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

extension_library :
	@if [ ! -f $(EXTENSION_LIBRARY) ]; then \
	  echo "building extension library..."; \
	  cd ../extensions; make install; cd -; \
	fi

extnop.amd : extension_library extnop.o
	$(GNU_CXX) -o $@ extnop.o $(AMD_LDFLAGS) -lchaiExt -lchai -lchaiAMD -lm

BUILD_TARGETS = \
	cg.x cg64.x \
	index.x \
	kirch.x \
	loopsum_uni.x loopsum_vec.x loopsum_omp.x loopsum_pth.x \
	matmul_uni.x matmul_vec.x matmul_omp.x matmul_pth.x \
	matmul64_uni.x matmul64_vec.x matmul64_omp.x matmul64_pth.x \
	mingle.x mingle64.x \
	monte.x \
	pi.x \
	sum_uni.x sum_vec.x sum_omp.x sum_pth.x

amd : $(BUILD_TARGETS:.x=.amd)

nvidia : $(BUILD_TARGETS:.x=.nvidia)

intel : $(BUILD_TARGETS:.x=.intel)

clean :
	rm -f *.o *.amd *.nvidia *.intel
