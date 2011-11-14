Publish-Subscribe Model for ROOT TObject Delivery
=================================================

This example code sends TObjects (TH1Fs) from a server to a client using the ZeroMQ publish-subscribe model. In this model, a server "publishes" (sends) data to many "subscribed" clients simultaneously.

For this example, "client" should be running before executing "server".

server.cpp
----------

The server creates a ZeroMQ socket and generates a random histogram. It then serializes the histogram (using a TBufferFile to generate a byte stream) and ships it out to clients, 10 times over.

client.cpp
----------

The client listens for incoming messages, interprets the message data as a TBufferFile buffer, and reconstructs the original histogram.

client.py
---------

Client, implemented in Python with PyROOT.

