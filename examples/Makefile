INCLUDE = -I$(ROOTSYS)/include -I$(AVALANCHE)/src -I$(AVALANCHE)/contrib -I$(RATROOT)/include
LFLAGS = -L/usr/local/lib -L$(ROOTSYS)/lib -L$(AVALANCHE)/lib -L$(RATROOT)/lib

CC = g++
LIBS = -lavalanche -lRATEvent_Linux-g++

all: client watch_db

client:
	$(CC) -o $@ client.cpp $(INCLUDE) $(CFLAGS) $(CXXFLAGS) $(LFLAGS) $(LIBS) `root-config --libs`

watch_db:
	$(CC) -o $@ watch_db.cpp $(INCLUDE) $(CFLAGS) $(CXXFLAGS) $(LFLAGS) $(LIBS) `root-config --libs`

clean:
	-$(RM) client watch_db *.o

