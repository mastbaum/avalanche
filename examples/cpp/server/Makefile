CFLAGS = 
INCLUDE = -I$(ROOTSYS)/include -I$(AVALANCHE)
CXXFLAGS =
LFLAGS = -L/usr/local/lib -L$(ROOTSYS)/lib -L$(AVALANCHE)

CC = g++
LIBS = -lzmq -lavalanche

all: server

server:
	$(CC) -o $@ server.cpp $(INCLUDE) $(CFLAGS) $(CXXFLAGS) $(LFLAGS) $(LIBS) `root-config --libs`

clean:
	-$(RM) server *.o

