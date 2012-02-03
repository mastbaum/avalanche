CFLAGS = -shared -fPIC -g
INCLUDE = -I$(ROOTSYS)/include -I.
RATINCLUDE = -I$(RATROOT)/include
LFLAGS = -L/usr/local/lib -L$(ROOTSYS)/lib

CC = g++
LIBS = -lzmq -lpthread -lcurl -lCore -lCint -lRIO -ldl -rdynamic

all: libavalanche

libavalanche:
	$(CC) -o libavalanche.so server.cpp client.cpp stream.cpp json/json_reader.cpp json/json_writer.cpp json/json_value.cpp $(INCLUDE) $(CFLAGS) $(CXXFLAGS) $(LFLAGS) $(LIBS) `root-config --libs`

clean:
	-$(RM) *.o *.so

