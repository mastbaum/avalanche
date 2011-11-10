g++ server.cpp -o server -lzmq -I$ROOTSYS/include `root-config --libs` -g
g++ client.cpp -o client -lzmq -I$ROOTSYS/include `root-config --libs` -g

