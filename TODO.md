# Immediate
- Use mockfile functions throughout tests.
- init() for KVStore that sets the variables that open does now.
- open takes just the flags, datadir assigned in KVStore::KVStore()
- test_open for each KVError.code that can happen.
- use file permissions in testing_helpers mock files.
- TestHelpers::writeToOffset(stream, path, offset);
- Test for input stream before/after a get().
- Get should return a KVResult. Update and be specific with the errors where std::nullopt is.
- Finish using mock file creation in tests wherever needed.
- Make a testing wrapper function for making mock directory for dir/open/etc.. difficult with ~tempdirguard()
- Testing one api function cannot call another has to be seperate/simulated. Most should be on its own. But some can use other calls.
- Test restore() with a more complex series of puts, perhaps a roll-over and a put collision too. ALL SEPERATE TESTS. We can also test restore against an incomplete record too.
- Move datafile functionality to namespace DF::createPath(), DF::openActive();
- Store a static counter for temporary directories in a test_.cpp and construct the path somewhere incrementing, labelling the directory manually is inefficient and error-prone.

# Less Immediate
- stFlags.syncOnPut what is it, how do i implement if i haven't by accident.
- stFlags.readWrite not fully fleshed out and tested. 
- Data-align record class members will perhaps become a bottleneck eventually, could compare current version with a data-aligned version.
- What if user opens an existing store but changes the max filesize? reading from a file isnt always accurate as if a file is too big for the next write we roll-over and write to the next file which means the number of bytes in a full file will rarely be the actual max byte size. Perhaps we store a json or something in our datadir with some metadata, this probably solves multiple problems too.
- KVStore::open() should just be called in kvs constructor.
- Make a testing build in workflows.
- Make two instances and run on the same directory.

- How do I handle a mid-crash write? How does restore deal with a half complete record write? Do we have enough information to restore the data. If yes then restore it and if not then delete the data from disk and keyDir.

- How will restore() handle changes to StoreFlags?