.. avalanche documentation master file, created by
   sphinx-quickstart on Fri Feb  3 18:09:37 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Introduction
============
This is the reference documentation for the class structure of the avalanche Python package. For instructions and examples of normal usage, see ``README.md``.

avalanche API Documentation
===========================
The only class most users will need is ``avalanche.Client``. The client can be connected to many data streams, including dispatchers (avalanche servers) and CouchDB databases.

.. automodule:: avalanche.avalanche
   :members:

SNO+-specific Functions
-----------------------
A function is included for turning SNO+ CouchDB header records into RAT::DS::PackedRec objects:

.. automodule:: avalanche.ratdb
   :members:

avalanche Internals
===================
The interaction with data streams is handled by specialized classes, the "main" function of which runs in a thread. These can be used for general-purpose, direct interaction with data sources, but this is not usually needed.

avalanche.dispatch
------------------

.. automodule:: avalanche.dispatch
   :members:

avalanche.db
------------

.. automodule:: avalanche.db
   :members:

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

