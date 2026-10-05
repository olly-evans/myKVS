- setActiveDatafile() needs to take flags for read/write.
- How to handle collisions in hash table.

- INPUT AND OUTPUT FILESTREAM, ONE AT A TIME. LOCKED PERHAPS IDK.


- what if user opens an existing store but changes the max filesize? reading from a file isnt always accurate as if a file is too big for the next write we roll-over and write to the next file which means the number of bytes in a full file will rarely be the actual max byte size. Perhaps we store a json or something in our datadir with some metadata, this probably solves multiple problems too.

- .open() should just be called in kvs constructor.
- Make a testing branch.

- Make two instances and run on the same directory.


# 1 Test restore() with a more complex series of puts, and perhaps a rollOver too.
# 2 store a counter for temporary directores in a test_.cpp and construct the path somewhere.
# 3 Migrate get() to use std::expected perhaps with an error struct.