#include "header.h"

Header::Header(uint32_t cs, uint8_t v, uint32_t ts, uint32_t ks, uint32_t vs) {
    checksum = cs;
    version = v;
    timestamp = ts;
    keySize = ks;
    valSize = vs;
    
    recordSize = HEADER_SIZE + keySize + valSize;
}

size_t Header::getRecordSize() const {
    return recordSize;
}

uint32_t Header::getChecksum() const {
    return checksum;
}

uint32_t Header::getExpectedChecksum(uint32_t timestamp) {
    setTimestamp(timestamp);
    computeHeaderChecksum();
    return checksum;
}

void Header::computeHeaderChecksum() {
    boost::crc_32_type result;
    result.process_bytes(&timestamp, TIMESTAMP_SIZE);
    result.process_bytes(&keySize,   KEYSIZE_SIZE);
    result.process_bytes(&valSize,   VALUESIZE_SIZE);
    checksum = static_cast<uint32_t>(result.checksum());
}

void Header::setTimestamp(uint32_t ts) {
    timestamp = ts;
}

uint32_t Header::getTimestampNow() {

    namespace ch = std::chrono;

    const auto p1 = ch::system_clock::now();
    uint32_t ts = ch::duration_cast<ch::seconds>(p1.time_since_epoch()).count();

    return ts;
}

uint32_t Header::getTimestamp() const {
    return timestamp;
}

uint32_t Header::getKeySize() const {
    return keySize;
}

uint32_t Header::getValueSize() const {
    return valSize;
}
