#include "record.h"

#include <chrono>

Record::Record(std::string key, std::string value) : 
    val(value), key(key) {

    setTimestampNow();
    setKeySize(sizeof(key));
    setValueSize(sizeof(value));

    // computeSetCRC16();
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
