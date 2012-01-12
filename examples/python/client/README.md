Publish-Subscribe Model for RAT Packed Record Delivery
======================================================
This client code receives PackedRec objects published by the RAT DispatchEvents processor.

Note: Be sure the avalanche module (in lib/python) is in your PYTHONPATH.

client.py
---------

The client listens for incoming event data (PackedRec objects), and reconstructs the original data.

N.B. You can connect to multiple dispatcher streams at the same time. Add them like this:

    cli.add_server('tcp://host:port')

