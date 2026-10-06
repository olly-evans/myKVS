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

    fs::path dataDir = TestHelpers::create_tmp_dir("test_kvstorehandle_1");
    TempDirGuard cleanup{dataDir};

    stH.setDatafileExt(".data"); // Only have default extension if KVStore::open ran.
    std::ofstream mockFile1 (dataDir / "1.aol.data");
    mockFile1.close();

    std::ofstream mockFile2 (dataDir / "5.aol.data");
    mockFile2.close();

    stH.updateActiveDatafileID(dataDir);

    REQUIRE(stH.getActiveDatafileID() == 5);
}

TEST_CASE("Reading sequentially over a full record correctly from zero offset.", "KVStoreHandle::readRecord()") {

    fs::path dataDir = TestHelpers::create_tmp_dir("test_kvstorehandle_2");
    TempDirGuard cleanup{dataDir};

    KVStore kvs;
    StoreFlags flags;
    
    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    auto putresult = kvs.put(stH, "key", "value");
    REQUIRE(putresult);
    
    Record rec("key", "value");

    kvs.updateActiveInputStream(stH.getActiveDatafilePath());

    Record readRec = stH.readRecord(kvs.getActiveInputStream(), 0);

    REQUIRE(readRec.byteSize() == rec.byteSize());
    REQUIRE(readRec.getCRC32() == rec.getCRC32());

    REQUIRE(readRec.getKeySize() == rec.getKeySize());
    REQUIRE(readRec.getValueSize() == rec.getValueSize());

    REQUIRE(readRec.getKey() == rec.getKey());
    REQUIRE(readRec.getValue() == rec.getValue());
}

TEST_CASE("Reading sequentially over a full record correctly from offset.", "KVStoreHandle::readRecord()") {

    fs::path dataDir = TestHelpers::create_tmp_dir("test_kvstorehandle_3");
    TempDirGuard cleanup{dataDir};

    KVStore kvs;
    StoreFlags flags;
    
    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    auto putresult1 = kvs.put(stH, "key", "value");
    REQUIRE(putresult1);

    auto putresult2 = kvs.put(stH, "key2", "value2");
    REQUIRE(putresult1);

    Record rec("key", "value");

    kvs.updateActiveInputStream(stH.getActiveDatafilePath());

    /* Offset to second record. */
    Record readRec = stH.readRecord(kvs.getActiveInputStream(), rec.byteSize());

    REQUIRE(readRec.getKeySize() == 4);
    REQUIRE(readRec.getValueSize() == 6);

    REQUIRE(readRec.getKey() == "key2");
    REQUIRE(readRec.getValue() == "value2");

}
TEST_CASE("Reading fields via seeking in ifstream correctly", "KVStoreHandle::readField<T>()") {

    fs::path dataDir = TestHelpers::create_tmp_dir("test_kvstorehandle_4");
    TempDirGuard cleanup{dataDir};

    KVStore kvs;
    StoreFlags flags;
    
    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    auto putresult = kvs.put(stH, "key", "value");
    REQUIRE(putresult);

    kvs.updateActiveInputStream(stH.getActiveDatafilePath());

    uint64_t off = sizeof(uint32_t) + sizeof(uint64_t);
    kvs.getActiveInputStream().seekg(static_cast<std::streamoff>(off), std::ios_base::beg);

    uint32_t keySize = stH.readField<uint32_t>(kvs.getActiveInputStream());
    REQUIRE(keySize == 3); 

    uint32_t valSize = stH.readField<uint32_t>(kvs.getActiveInputStream());
    REQUIRE(valSize == 5);
}