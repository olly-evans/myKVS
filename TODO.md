# Immediate
- Test restore() with a more complex series of puts, perhaps a roll-over and a put collision too. ALL SEPERATE TESTS.
- Store a counter for temporary directories in a test_.cpp and construct the path somewhere incrementing, labelling the directory manually is inefficient and error-prone.
- Minimum file bytes should not be less than minimum record bytes.
- Choose an error handling system and stay consistent with it. Especially in put().

# Less Immediate
- What if user opens an existing store but changes the max filesize? reading from a file isnt always accurate as if a file is too big for the next write we roll-over and write to the next file which means the number of bytes in a full file will rarely be the actual max byte size. Perhaps we store a json or something in our datadir with some metadata, this probably solves multiple problems too.
- KVStore::open() should just be called in kvs constructor.
- Make a testing build in workflows.
- Make two instances and run on the same directory.
- How do I handle a mid-crash write? How does restore deal with a half complete record write?
- How will restore() handle changes to StoreFlags?