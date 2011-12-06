CFLAGS = 
INCLUDE = -I$(ROOTSYS)/include -I$(AVALANCHE) -I$(RATROOT)/include
CXXFLAGS =
LFLAGS = -L/usr/local/lib -L$(ROOTSYS)/lib -L$(AVALANCHE) -L$(RATROOT)/lib

CC = g++
LIBS = -lzmq -lavalanche -lRATEvent_Linux-g++

all: client

client:
	$(CC) -o $@ client.cpp $(INCLUDE) $(CFLAGS) $(CXXFLAGS) $(LFLAGS) $(LIBS) `root-config --libs`

clean:
	-$(RM) client *.o

