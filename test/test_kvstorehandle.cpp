#include "kvstorehandle.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("KVStoreHandle sets read/write permissions for the store.", "KVStoreHandle::setReadWrite()") {
    
    KVStoreHandle stH;
    bool readWrite = true;
    stH.setReadWrite(readWrite);

    REQUIRE(stH.getReadWrite() == readWrite);
}


TEST_CASE("KVStoreHandle sets a datafile extension for the store", "setDatafileExt()") {
    
    KVStoreHandle stH;
    stH.setDatafileExt(".data");

    REQUIRE(stH.getDatafileExt() == ".data");
}

TEST_CASE("KVStoreHandle updates the active datafile id in datadir", "updateActiveFileID()") {

    KVStoreHandle stH;

    fs::path dataDir = createTempTestDir("test_kvstorehandle_1");
    TempDirGuard cleanup{dataDir};

    stH.setDatafileExt(".data"); // Only have default extension if KVStore::open ran.
    std::ofstream mockFile1 (dataDir / "0.aol.data");
    mockFile1.close();

    std::ofstream mockFile2 (dataDir / "5.aol.data");
    mockFile2.close();

    stH.updateActiveDatafileID(dataDir);

    REQUIRE(stH.getActiveDatafileID() == 5);
}

TEST_CASE("Reading sequentially over a full record correctly from start of file.", "KVStoreHandle::readField<T>(), KVStoreHandle::readString()") {

    fs::path dataDir = createTempTestDir("test_kvstorehandle_2");
    TempDirGuard cleanup{dataDir};

    KVStore kvs;
    StoreFlags flags;
    
    KVStoreHandle stH = kvs.open(dataDir, flags);
    kvs.put(stH, "key", "value");
    
    Record rec("key", "value");

    kvs.updateActiveInputStream(stH.getActiveDatafilePath());

    uint32_t crc = stH.readField<uint32_t>(kvs.getActiveInputStream());
    REQUIRE(crc == rec.getCRC32());

    uint64_t ts = stH.readField<uint64_t>(kvs.getActiveInputStream());
    uint32_t keySize = stH.readField<uint32_t>(kvs.getActiveInputStream());
    uint32_t valSize = stH.readField<uint32_t>(kvs.getActiveInputStream());

    std::string key = stH.readString(kvs.getActiveInputStream(), keySize);
    std::string val = stH.readString(kvs.getActiveInputStream(), valSize);

    REQUIRE(key == "key");
    REQUIRE(val == "value");

}

TEST_CASE("Reading fields via seeking in ifstream correctly", "KVStoreHandle::readField<T>()") {

    fs::path dataDir = createTempTestDir("test_kvstorehandle_2");
    TempDirGuard cleanup{dataDir};

    KVStore kvs;
    StoreFlags flags;
    
    KVStoreHandle stH = kvs.open(dataDir, flags);
    kvs.put(stH, "key", "value");

    kvs.updateActiveInputStream(stH.getActiveDatafilePath());
    kvs.getActiveInputStream().seekg(sizeof(uint32_t) + sizeof(uint64_t), std::ios_base::beg);

    uint32_t keySize = stH.readField<uint32_t>(kvs.getActiveInputStream());
    REQUIRE(keySize == 3); 

    uint32_t valSize = stH.readField<uint32_t>(kvs.getActiveInputStream());
    REQUIRE(valSize == 5);
}