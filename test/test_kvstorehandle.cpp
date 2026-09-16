void test_kvshandle_sets_absolute_directory() {

    KVStoreHandle handle;
    handle.setAbsDirPath();

    assert(std::filesystem::exists(handle.getAbsDirPath()));
}