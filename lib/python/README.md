Avalanche Client Python Package
===============================
API Documenation
----------------
For detailed documentation of the avalanche client API, build the Sphinx documentation in `doc` (requires sphinx):

    $ cd doc
    $ make html

HTML output is located at `doc/_build/index.html`.

Installation
------------
To install the package, run

    python setup.py install

avalanche.Client
-----------------
### Creating a Client ###

    avalanche.Client()

Example:

    client = new avalanche.Client();
    client.add_dispatcher("tcp://localhost:7777");
    client.add_db("http://localhost:5984", "mydb", doc_object_map);

This will connect to an avalanche server running on localhost port 7777 via TCP and the changes feed of a CouchDB database at `http://localhost:5984/mydb`.

#### Talking to CouchDB ####
The avalanche client always returns a `TObject`, so when connecting to a database you must give it a function (or callable) that will convert CouchDB documents. This is the `doc_object_map` in the above example.

This function must accept a dictionary-like object as a single argument, and return a ROOT.TObject or None. For example:

    def mapper(doc):
        if doc["type"] != "foo":
            return None
        o = TFoo() # TFoo is some kind of TObject
        o.bar = doc["bar"]
        return o

    client.add_db("http://localhost:5984", "mydb", mapper);
 
For SNO+ users, this mapping function is provided. `import avalanche.ratdb` and use:

    client.add_db("http://localhost:5984", "mydb", avalanche.ratdb.doc_to_record);

### Receiving Objects ###

    avalanche.Client.recv(blocking=false)

Example:

    while True:
        rec = client.recv();
        if rec is not None:
            print 'got some data!'

By default, `recv` is non-blocking; `blocking=true` will cause recv() to wait until data is available before returning. Note that `recv` returns a `TObject*`, which must be cast appropriately, likely using `TObject::IsA()`.

