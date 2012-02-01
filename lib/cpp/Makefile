CFLAGS = -shared -fPIC -g
INCLUDE = -I$(ROOTSYS)/include
LFLAGS = -L/usr/local/lib -L$(ROOTSYS)/lib

CC = g++
LIBS = -lzmq -lpthread -lCore -lCint -lRIO -ldl -rdynamic

all: libavalanche.so

libavalanche.so:
	$(CC) -o libavalanche.so server.cpp client.cpp $(INCLUDE) $(CFLAGS) $(CXXFLAGS) $(LFLAGS) $(LIBS) `root-config --libs`

clean:
	-$(RM) *.o *.so

