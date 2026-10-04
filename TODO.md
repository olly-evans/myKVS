- setActiveDatafile() needs to take flags for read/write.
- How to handle collisions in hash table.

- INPUT AND OUTPUT FILESTREAM, ONE AT A TIME. LOCKED PERHAPS IDK.


- what if user opens an existing store but changes the max filesize? reading from a file isnt always accurate as if a file is too big for the next write we roll-over and write to the next file which means the number of bytes in a full file will rarely be the actual max byte size. Perhaps we store a json or something in our datadir with some metadata, this probably solves multiple problems too.

- .open() should just be called in kvs constructor.
- Make a testing branch.

- Make two instances and run on the same directory.

- FINISH TEST FOR RESTORE()

- Functions for restoreRecord() in restore()
- SetTimestamp() in restore().

- activeDatafilePath and getters/setters to kvstorehandle.

- File stuff in datafile Object or namespace, if it goes in object it'll need more stuff w
- kvstore_api.cpp, kvstore_files.cpp... I think almost all getters/setters in kvstore can be put into Handle.

- ReadKeySize... etc.. down the restored record then read record in loop.

- test for replacing key. should just update it to new value.
