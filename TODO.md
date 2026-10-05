# Long-term
- What if user opens an existing store but changes the max filesize? reading from a file isnt always accurate as if a file is too big for the next write we roll-over and write to the next file which means the number of bytes in a full file will rarely be the actual max byte size. Perhaps we store a json or something in our datadir with some metadata, this probably solves multiple problems too.
- KVStore::open() should just be called in kvs constructor.
- Make a testing branch.
- Make two instances and run on the same directory.

# Immediate
- Test restore() with a more complex series of puts, and perhaps a roll-over and a put collision too. ALL SEPERATE PLEASE.
- Store a counter for temporary directories in a test_.cpp and construct the path somewhere incrementing.