include Makefile.common

all : libchai.a

libchai.a : \
	kernel_ \
	misc_ \
	vendor_ \
	bytecode_ \
	common_ \
	interp_ \
	jit_ \
	runtime_ \
	api_
	cd install/.lib; $(AR) qc $@ *.o; $(RANLIB) $@; install $@ ../lib

api_ : installdirs
	cd api; make install; cd -

bytecode_ : installdirs
	cd bytecode; make install; cd -

common_ : installdirs
	cd common; make install; cd -

interp_ : installdirs
	cd interp; make install; cd -

jit_ : installdirs
	cd jit; make install; cd -

kernel_ : installdirs
	cd kernel; make install; cd -

misc_ : installdirs
	cd misc; make install; cd -

runtime_ : installdirs
	cd runtime; make install; cd -

vendor_ : installdirs
	@if [ $(ATISTREAMSDKROOT) ]; then make libchaiATI.a; fi
	@if [ $(NVIDIACUDASDKROOT) ]; then make libchaiNVIDIA.a; fi

libchaiATI.a :
	cd vendor; make -f Makefile.ATI clean install; cd -

libchaiNVIDIA.a :
	cd vendor; make -f Makefile.NVIDIA clean install; cd -

installdirs :
	install -d install/.include
	install -d install/.lib
	install -d install/include
	install -d install/lib

clean :
	cd api; make clean; cd -
	cd bytecode; make clean; cd -
	cd common; make clean; cd -
	cd interp; make clean; cd -
	cd jit; make clean; cd -
	cd kernel; make clean; cd -
	cd misc; make clean; cd -
	cd runtime; make clean; cd -
	cd vendor; make -f Makefile.ATI clean; cd -
	cd vendor; make -f Makefile.NVIDIA clean; cd -
	rm -rf install
	wc -l \
		api/*.h* api/*.c* \
		bytecode/*.h* bytecode/*.c* \
		common/*.h* common/*.c* \
		interp/*.h* interp/*.c* \
		jit/*.h* jit/*.c* \
		kernel/*.h* kernel/*.c* \
		misc/*.h* misc/*.c* \
		runtime/*.h* runtime/*.c* \
		vendor/*.h* vendor/*.c*
