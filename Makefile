CFLAGS = -fPIC -g -DDEBUG=0
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

SRCS = src/client.cpp src/stream.cpp contrib/json/json_reader.cpp contrib/json/json_writer.cpp contrib/json/json_value.cpp

all: libavalanche

libavalanche:
	test -d lib || mkdir lib
	$(CC) -o $@.so $(SRCS) $(INCDIR) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIBS)
	mv $@.so lib

clean:
	-$(RM) *.o lib/*.so
	-$(RM) -r *.dSYM

