#include "header.h"

Header::Header(uint32_t cs, uint8_t v, uint32_t ts, uint32_t ks, uint32_t vs) {
    checksum = cs;
    version = v;
    timestamp = ts;
    keySize = ks;
    valSize = vs;
    
    recordSize = HEADER_SIZE + keySize + valSize;
}

void Header::serialize(std::vector<char>& buf) const {

    if (buf.size() < HEADER_SIZE)
        throw std::invalid_argument("Buffer too small to serialize header - check call site.");
        
    std::memcpy(buf.data(), &checksum, CHECKSUM_SIZE);
    std::memcpy(buf.data() + VERSION_OFFSET, &version, VERSION_SIZE);
    std::memcpy(buf.data() + TIMESTAMP_OFFSET, &timestamp, TIMESTAMP_SIZE);
    std::memcpy(buf.data() + KEYSIZE_OFFSET, &keySize, KEYSIZE_SIZE);
    std::memcpy(buf.data() + VALUESIZE_OFFSET, &valSize, VALUESIZE_SIZE);
}

size_t Header::getRecordSize() const {
    return recordSize;
}

uint32_t Header::getChecksum() const {
    return checksum;
}

unsigned int Header::getComputedChecksum() const {
    boost::crc_32_type result;
    result.process_bytes(&timestamp, TIMESTAMP_SIZE);
    result.process_bytes(&keySize,   KEYSIZE_SIZE);
    result.process_bytes(&valSize,   VALUESIZE_SIZE);
    return result.checksum();
}

void Header::setChecksum(uint32_t cs) {
    checksum = cs;
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
