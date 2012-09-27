CFLAGS = -fPIC -g -DDEBUG=0 -fpermissive
INCDIR = -Isrc -Icontrib -I$(RATZDAB_ROOT)/src
LDFLAGS = -L$(RATZDAB_ROOT)/lib

CLANG := $(shell which clang)
ifneq ($(CLANG),)
    CC = clang++
    CFLAGS += 
    INCDIR += -I$(shell root-config --incdir)
else
    CC = g++
    CFLAGS += -shared
    INCDIR += -I$(ROOTSYS)/include
endif

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    LDFLAGS += -L$(shell root-config --libdir) -dynamiclib #-install_name /usr/local/lib/$(@F).so #-undefined dynamic_lookup
endif
ifeq ($(UNAME), Linux)
    LDFLAGS += -L$(ROOTSYS)/lib -rdynamic
endif

LIBS = -lzdispatch -lpthread -lcurl -ldl $(shell root-config --libs)

DICT_SRCS = http.hpp stream.hpp avalanche.hpp

SRCS = src/client.cpp src/stream.cpp contrib/json/json_reader.cpp contrib/json/json_writer.cpp contrib/json/json_value.cpp

all: libavalanche.so rootlib

rat: all rootlib_rat

rootlib:
	test -d build || mkdir build
	cd src && $(ROOTSYS)/bin/rootcint -f ../build/avalanche_dict.cxx -c -p -I. $(INCDIR) $(DICT_SRCS) linkdef.h
	$(CC) -o lib/avalanche_root.so -shared -fPIC $(CFLAGS) build/avalanche_dict.cxx $(SRCS) $(INCDIR) $(LDFLAGS) $(LIBS)

rootlib_rat:
	test -d build || mkdir build
	cd src && $(ROOTSYS)/bin/rootcint -f ../build/avalanche_rat_dict.cxx -c -p -I. -I../contrib $(INCDIR) -I$(RATROOT)/include avalanche_rat.hpp $(DICT_SRCS) linkdef_rat.h
	$(CC) -o lib/avalanche_rat.so -shared -fPIC $(CFLAGS) build/avalanche_rat_dict.cxx $(SRCS) $(INCDIR) -I$(RATROOT)/include $(LDFLAGS) $(LIBS)

libavalanche.so:
	test -d lib || mkdir lib
	$(CC) -o $@ $(SRCS) $(INCDIR) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIBS)
	mv $@ lib

clean:
	-$(RM) *.o lib/*.so
	-$(RM) -r *.dSYM

