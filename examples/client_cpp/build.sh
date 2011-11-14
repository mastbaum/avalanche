g++ client.cpp -o client -lzmq $RATROOT/lib/libRATEvent_Linux-g++.so -I$RATROOT/include -I$ROOTSYS/include `root-config --libs` -g

