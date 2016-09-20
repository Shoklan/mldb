DEFAULTGXX:=aarch64-linux-gnu-g++
DEFAULTGCC:=aarch64-linux-gnu-gcc
ARCHFLAGS:=-fPIC -fno-omit-frame-pointer -I$(BUILD)/$(ARCH)/osdeps/usr/include -I$(BUILD)/$(ARCH)/osdeps/usr/include/aarch64-linux-gnu -I/usr/local/cuda-8.0/targets/aarch64-linux/include
PORT_LIBRARY_DIRS := \
	$(BUILD)/$(ARCH)/osdeps/usr/lib/aarch64-linux-gnu \
	$(BUILD)/$(ARCH)/osdeps/usr/lib \
	$(BUILD)/$(ARCH)/osdeps/lib/aarch64-linux-gnu \
	$(BUILD)/$(ARCH)/osdeps/usr/lib/lapack \
	$(BUILD)/$(ARCH)/osdeps/usr/lib/libblas \
	/usr/local/cuda-8.0/targets/aarch64-linux/lib


PORT_LINK_FLAGS:=$(foreach dir,$(PORT_LIBRARY_DIRS), -L$(dir) -Wl,--rpath,$(dir))

TCMALLOC_ENABLED:=0
port:=ubuntu1404
