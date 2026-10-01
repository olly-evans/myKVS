- If you open an existing store you cannot use a new datafile extension. Or maybe you can...?
- Why is crc for same data giving different values on repeated execution?
- setActiveDatafile() needs to take flags for read/write.
- How to handle collisions in hash table.
- Do I need an activeFilestream for puts?

- INPUT AND OUTPUT FILESTREAM, ONE AT A TIME. LOCKED PERHAPS IDK.

- remove testing boilerplate into main, test_kvstore, think when we open a store we

- what if user opens an existing store but changes the max filesize? reading from a file isnt always accurate as if a file is too big for the next write we roll-over and write to the next file which means the number of bytes in a full file will rarely be the actual max byte size. Perhaps we store a json or something in our datadir with some metadata, this probably solves multiple problems too.

- Make a testing branch.

- Make two instances and run on the same directory.