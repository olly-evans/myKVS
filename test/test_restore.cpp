#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Does restore reboot for one put", "KVStore::restore()") {
    
    TempDirGuard guard(TestHelpers::create_tmp_dir());

    KVStore kvs(guard.dir);
    StoreFlags flags;

    fs::path mFilePath1 = guard.dir/ "0.aol.data";
    std::ofstream mockFile1(mFilePath1);

    Record rec("key", "val");

    // Simulating a put.
    kvs.setActiveOutputStream(std::move(mockFile1));
    rec.serialize(kvs.getActiveOutputStream());

    kvs.getActiveOutputStream().close();

    // Note: We aren't updating the keyDir here as we want to restore it.

    KVExpected result = kvs.open(flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();
    
    REQUIRE(HandleTestAccess::keyDir(stH).size() == 1);

    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at("key");
    REQUIRE(entry.fileID == 0);
    REQUIRE(entry.tstamp == rec.getTimestamp());
    REQUIRE(entry.valFileOffset == 23);
    REQUIRE(entry.valSz == 3);
}

TEST_CASE("Does restore work for two full datafiles", "KVStore::restore()") {
        
    TempDirGuard guard(TestHelpers::create_tmp_dir());

    KVStore kvs(guard.dir);
    StoreFlags flags;

    flags.maxDatafileBytes = 30; /* Put sizes both 30 bytes. */

    KVExpected openresult = kvs.open(flags);
    
    KVResult put1 = kvs.put(openresult.value(), "key1", "value1");
    KVResult put2 = kvs.put(openresult.value(), "key2", "value2");

    REQUIRE(openresult.value().getActiveDatafileID() == 1); 

    KVStore restorekvs(guard.dir);
    KVExpected restoreresult = restorekvs.open(flags);
    KVStoreHandle stH = restoreresult.value();

    REQUIRE(stH.getActiveDatafileID() == 1);
    REQUIRE(HandleTestAccess::keyDir(stH).size() == 2);

    KVResult getresult1 = restorekvs.get(stH, "key1"); 
    REQUIRE(getresult1.value() == "value1");
    
    KVResult getresult2 = restorekvs.get(stH, "key2");
    REQUIRE(getresult2.value() == "value2");

}

TEST_CASE("Does restore retrieve correct keys for a full datafile and a roll-over", "KVStore::restore()") {
        
    TempDirGuard guard(TestHelpers::create_tmp_dir());

    KVStore kvs(guard.dir);
    StoreFlags flags;

    Record rec1("key1", "value1");
    Record rec2("key2", "value2");
    Record rec3("key3", "value3");

    flags.maxDatafileBytes = rec1.byteSize() * 2;

    KVExpected openresult = kvs.open(flags);
    KVStoreHandle stH = openresult.value();

    KVResult put1 = kvs.put(stH, rec1.getKey(), rec1.getValue());
    KVResult put2 = kvs.put(stH, rec2.getKey(), rec2.getValue());
    KVResult put3 = kvs.put(stH, rec3.getKey(), rec3.getValue());
    REQUIRE(stH.getActiveDatafileID() == 1);  

    KVStore restorekvs(guard.dir);
    KVExpected restoreresult = restorekvs.open(flags);
    KVStoreHandle newstH = restoreresult.value();

    REQUIRE(newstH.getActiveDatafileID() == 1);
    REQUIRE(HandleTestAccess::keyDir(newstH).size() == 3);

    KVResult getresult1 = restorekvs.get(newstH, rec1.getKey()); 
    REQUIRE(getresult1.value() == rec1.getValue());
    
    KVResult getresult2 = restorekvs.get(newstH, rec2.getKey());
    REQUIRE(getresult2.value() == rec2.getValue()); /*failing, id issues too i think had one error.*/

    KVResult getresult3 = restorekvs.get(newstH, rec3.getKey());
    REQUIRE(getresult3.value() == rec3.getValue());
}

TEST_CASE("Does restore handle a hash table collision", "KVStore::restore()") {
        
    TempDirGuard guard(TestHelpers::create_tmp_dir());

    KVStore kvs(guard.dir);
    StoreFlags flags;

    Record rec1("key1", "value1");
    Record rec2("key1", "value2");

    KVExpected openresult = kvs.open(flags);
    KVStoreHandle stH = openresult.value();

    KVResult put1 = kvs.put(stH, rec1.getKey(), rec1.getValue());
    KVResult put2 = kvs.put(stH, rec2.getKey(), rec2.getValue());
    
    KVResult get = kvs.get(stH, rec1.getKey());
    REQUIRE(get.value() == rec2.getValue());

    /* One keyDir entry as insert_or_assign() used. */
    REQUIRE(HandleTestAccess::keyDir(stH).size() == 1);

    fs::path datafile = stH.getActiveDatafilePath();

    /* Will need to require old data is tombstoned. But 60 bytes is correct. */
    REQUIRE(fs::file_size(datafile) == 60);
}