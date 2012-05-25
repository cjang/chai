include Makefile.common
include Makefile.config

all : \
	Random123_ \
	misc_ \
	vendor_ \
	openclhack_ \
	autotune_ \
	kernel_ \
	bytecode_ \
	common_ \
	interp_ \
	jitast_ \
	jitbc_ \
	jitstmt_ \
	jitx_ \
	runtime_ \
	api_ \
	applib_ \
	libchai_ \
	tools_

libchai_ :
	@if [ $(SHARED_LDFLAGS) ]; \
	then \
	  cd _install/.lib; \
	  rm -f libchai.so; \
	  $(GNU_CXX) $(SHARED_LDFLAGS) -o libchai.so *.o; \
	  install libchai.so ../lib; \
	else \
	  cd _install/.lib; \
	  rm -f libchai.a; \
	  $(AR) qc libchai.a *.o; \
	  $(RANLIB) libchai.a; \
	  install libchai.a ../lib; \
	fi

api_ : _installdirs
	cd api; make install; cd -

applib_ : _installdirs
	cd applib; make install; cd -

autotune_ : _installdirs
	cd autotune; make install; cd -

bytecode_ : _installdirs
	cd bytecode; make install; cd -

common_ : _installdirs
	cd common; make install; cd -

interp_ : _installdirs
	cd interp; make install; cd -

jitast_ : _installdirs
	cd jitast; make install; cd -

jitbc_ : _installdirs
	cd jitbc; make install; cd -

jitstmt_ : _installdirs
	cd jitstmt; make install; cd -

jitx_ : _installdirs
	cd jitx; make install; cd -

kernel_ : _installdirs
	cd kernel; make install; cd -

misc_ : _installdirs
	cd misc; make install; cd -

openclhack_ : _installdirs
	@if [ -d /usr/include/CL ]; \
	then \
	  echo "OpenCL found in /usr/include, do nothing"; \
	else \
	  echo "need dummy OpenCL link for compiler preprocessor"; \
	fi
	@if [ ! -d /usr/include/CL ]; \
	then \
	  if [ $(AMD_SDK) ]; \
	  then \
	    echo "creating link to AMD OpenCL header files"; \
	    rm -f _install/.include/CL; \
	    ln -s $(AMD_SDK)/include/CL _install/.include; \
	  fi; \
	fi
	@if [ ! -d /usr/include/CL ]; \
	then \
	  if [ $(INTEL_SDK) ]; \
	  then \
	    echo "creating link to INTEL OpenCL header files"; \
	    rm -f _install/.include/CL; \
	    ln -s $(INTEL_SDK)/usr/include/CL _install/.include; \
	  fi; \
	fi
	@if [ ! -d /usr/include/CL ]; \
	then \
	  if [ $(NVIDIA_CUDA) ]; \
	  then \
	    echo "creating link to NVIDIA OpenCL header files"; \
	    rm -f _install/.include/CL; \
	    ln -s $(NVIDIA_CUDA)/include/CL _install/.include; \
	  fi; \
	fi
	@if [ ! -d /usr/include/CL ]; \
	then \
	  if [ ! -h _install/.include/CL ]; \
	  then \
	    echo "no OpenCL found, build will fail"; \
	  fi; \
	fi

Random123_ : _installdirs
	cd Random123; make install; cd -

runtime_ : _installdirs
	cd runtime; make install; cd -

tools_ : _installdirs
	cd tools; make install; cd -

vendor_ : _installdirs
	@if [ $(AMD_SDK) ]; then make libchaiAMD_; fi
	@if [ $(INTEL_SDK) ]; then make libchaiINTEL_; fi
	@if [ $(NVIDIA_CUDA) ]; then make libchaiNVIDIA_; fi

libchaiAMD_ :
	@if [ $(SHARED_LDFLAGS) ]; then \
	  if [ ! -f vendor/libchaiAMD.so ]; then \
	    cd vendor; make -f Makefile.AMD clean install; cd -; \
	  fi; \
	else \
	  if [ ! -f vendor/libchaiAMD.a ]; then \
	    cd vendor; make -f Makefile.AMD clean install; cd -; \
	  fi; \
	fi

libchaiINTEL_ :
	@if [ $(SHARED_LDFLAGS) ]; then \
	  if [ ! -f vendor/libchaiINTEL.so ]; then \
	    cd vendor; make -f Makefile.INTEL clean install; cd -; \
	  fi; \
	else \
	  if [ ! -f vendor/libchaiINTEL.a ]; then \
	    cd vendor; make -f Makefile.INTEL clean install; cd -; \
	  fi; \
	fi

libchaiNVIDIA_ :
	@if [ $(SHARED_LDFLAGS) ]; then \
	  if [ ! -f vendor/libchaiNVIDIA.so ]; then \
	    cd vendor; make -f Makefile.NVIDIA clean install; cd -; \
	  fi; \
	else \
	  if [ ! -f vendor/libchaiNVIDIA.a ]; then \
	    cd vendor; make -f Makefile.NVIDIA clean install; cd -; \
	  fi; \
	fi

_installdirs :
	install -d _install/.include
	install -d _install/.lib
	install -d _install/include/chai
	install -d _install/lib
	install -d _install/bin

install : all
	install -d $(CHAI_INSTALLDIR_PREFIX)/include/chai
	install -d $(CHAI_INSTALLDIR_PREFIX)/lib
	install -d $(CHAI_INSTALLDIR_PREFIX)/bin
	install -v --mode=644 _install/include/chai/* $(CHAI_INSTALLDIR_PREFIX)/include/chai
	install -v _install/lib/* $(CHAI_INSTALLDIR_PREFIX)/lib
	install -v _install/bin/* $(CHAI_INSTALLDIR_PREFIX)/bin

clean :
	cd api; make clean; cd -
	cd applib; make clean; cd -
	cd autotune; make clean; cd -
	cd bytecode; make clean; cd -
	cd common; make clean; cd -
	cd interp; make clean; cd -
	cd jitast; make clean; cd -
	cd jitbc; make clean; cd -
	cd jitstmt; make clean; cd -
	cd jitx; make clean; cd -
	cd kernel; make clean; cd -
	cd misc; make clean; cd -
	cd Random123; make clean; cd -
	cd runtime; make clean; cd -
	cd tools; make clean; cd -
	cd vendor; make -f Makefile.AMD clean; cd -
	cd vendor; make -f Makefile.NVIDIA clean; cd -
	cd vendor; make -f Makefile.INTEL clean; cd -
	rm -rf _install
	wc -l \
		api/*.h* api/*.c* \
		applib/*.h* api/*.c* \
		autotune/*.h* autotune/*.c* \
		bytecode/*.h* bytecode/*.c* \
		common/*.h* common/*.c* \
		interp/*.h* interp/*.c* \
		jitast/*.h* jitast/*.c* \
		jitbc/*.h* jitbc/*.c* \
		jitstmt/*.h* jitstmt/*.c* \
		jitx/*.h* jitx/*.c* \
		kernel/*.h* kernel/*.c* \
		misc/*.h* misc/*.c* \
		Random123/*.h* Random123/*.c* \
		runtime/*.h* runtime/*.c* \
		tools/*.c* \
		vendor/*.h* vendor/*.c*

distclean : clean
	rm -f ENV.AMD ENV.INTEL ENV.NVIDIA Makefile.config
