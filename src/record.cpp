#include "record.h"
#include  "crc32.h"

#include <chrono>

constexpr uint32_t CRC32_POLY = 0x15a0849e7;

Record::Record(std::string k, std::string v) : 
    val(v), key(k) {

    setTimestampNow();
    setKeySize(sizeof(k));
    setValueSize(sizeof(v));
    
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
    return this->crc32;
}

void Record::setTimestampNow() {

    const auto p1 = std::chrono::system_clock::now();
    uint64_t ts = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    this->timestamp = ts;
}

uint64_t Record::getTimestamp() const {
    return this->timestamp;
}

void Record::setKeySize(uint32_t ks) {
    this->keySize = ks;
}

uint32_t Record::getKeySize() const {
    return this->keySize;
}

void Record::setValueSize(uint32_t vs) {
    this->valSize = vs;
}

uint32_t Record::getValueSize() const {
    return this->valSize;
}

void Record::setKey(std::string k) {
    this->key = k;
}

std::string Record::getKey() const {
    return this->key;
}

void Record::setValue(std::string v) {
    this->val = v;
}

std::string Record::getValue() const {
    return this->val;
}
