INCLUDE = -I/usr/local/include
LFLAGS = -L/usr/local/lib

CC = g++ -g
LIBS = -lzmq

all: proxy

proxy:
	$(CC) -o proxy proxy.cpp $(INCLUDE) $(LFLAGS) $(LIBS)

clean:
	-$(RM) *.o proxy

