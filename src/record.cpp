#include "record.h"

#include <chrono>
#include <boost/crc.hpp>
#include <iostream>

Record::Record(std::string k, std::string v) : 
    keySize(static_cast<uint32_t>(k.size())),
    valSize(static_cast<uint32_t>(v.size())),
    key(std::move(k)),
    val(std::move(v)) {

    setTimestampNow();
    setCRC32();
}

void Record::serialize(std::ostream& out) const {

    out.write(reinterpret_cast<const char*>(&crc32), sizeof(crc32));
    out.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    out.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
    out.write(reinterpret_cast<const char*>(&valSize), sizeof(valSize));

    out.write(key.data(), key.size());
    out.write(val.data(), val.size());
    
    out.flush();
}

size_t Record::byteSize() {
    return sizeof(crc32) + sizeof(timestamp) + sizeof(keySize) +
    sizeof(valSize) + keySize + valSize;
}

void Record::setCRC32() {
    boost::crc_32_type result;
    result.process_bytes(&timestamp, sizeof(timestamp));
    result.process_bytes(&keySize,   sizeof(keySize));
    result.process_bytes(&valSize,   sizeof(valSize));
    result.process_bytes(key.data(), keySize);
    result.process_bytes(val.data(), valSize);
    crc32 = result.checksum();
}


uint32_t Record::getCRC32() {
    return crc32;
}

void Record::setTimestamp(uint64_t ts) {
    timestamp = ts;
}

void Record::setTimestampNow() {

    const auto p1 = std::chrono::system_clock::now();
    uint64_t ts = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    timestamp = ts;
}

uint64_t Record::getTimestamp() const {
    return timestamp;
}

uint32_t Record::getKeySize() const {
    return keySize;
}

uint32_t Record::getValueSize() const {
    return valSize;
}

std::string Record::getKey() const {
    return key;
}

std::string Record::getValue() const {
    return val;
}