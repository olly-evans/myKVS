#include "record.h"
#include  "crc32.h"

#include <chrono>

Record::Record(std::string k, std::string v) : 
    key(std::move(k)), val(std::move(v)) {

    setTimestampNow();
    setKeySize(sizeof(key));
    setValueSize(sizeof(val));
    
    computeSetCRC32();
}

void Record::computeSetCRC32() {

    uint32_t crc = 0xFFFFFFFFu;

    crcUpdate(crc, &timestamp, sizeof(timestamp));
    crcUpdate(crc, &keySize,   sizeof(keySize));
    crcUpdate(crc, &valSize,   sizeof(valSize));
    crcUpdate(crc, key.data(), keySize);
    crcUpdate(crc, val.data(), valSize);

    crc32 = crc ^ 0xFFFFFFFFu;
}


uint32_t Record::getCRC32() {
    return crc32;
}

void Record::setTimestampNow() {

    const auto p1 = std::chrono::system_clock::now();
    uint64_t ts = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    timestamp = ts;
}

uint64_t Record::getTimestamp() const {
    return timestamp;
}

void Record::setKeySize(uint32_t ks) {
    keySize = ks;
}

uint32_t Record::getKeySize() const {
    return keySize;
}

void Record::setValueSize(uint32_t vs) {
    valSize = vs;
}

uint32_t Record::getValueSize() const {
    return valSize;
}

void Record::setKey(std::string k) {
    key = k;
}

std::string Record::getKey() const {
    return key;
}

void Record::setValue(std::string v) {
    val = v;
}

std::string Record::getValue() const {
    return val;
}
