#include "record.h"

/* Each object is responsible for its own serialization and desrerialization. */

Record::Record(std::string k, std::string v) :
    key(std::move(k)),
    value(std::move(v)) {
    
    uint32_t keySize = static_cast<uint32_t>(k.size());
    uint32_t valSize = static_cast<uint32_t>(v.size());

    header = Header(Versions::CURRENT_DATA_FILE_VERSION, header.getTimestampNow(), keySize, valSize);
}

std::vector<char> Record::serialize() {

    std::vector<char> buf(getRecordSize());

    /* passing in vector, offsets will be set*/
    // halodb checksum isnt over key/value????? is that a bug...

    header.serialize(buf);

    std::memcpy(buf.data() + header.HEADER_SIZE, key.data(), getKeySize());
    std::memcpy(buf.data() + header.HEADER_SIZE + getKeySize(), value.data(), getValueSize());

    uint32_t crc = getComputedChecksum(buf.data() + header.VERSION_OFFSET, 
                                       getRecordSize() - header.CHECKSUM_SIZE); 
    header.setChecksum(crc);

    std::memcpy(buf.data(), &crc, header.CHECKSUM_OFFSET);

    return buf;
}

// Record Record::deserialize(std::ifstream& in, const uint64_t recFileOffset) {

//     if (recFileOffset > 0)
//         in.seekg(static_cast<std::streamoff>(recFileOffset), std::ios_base::beg);

//     uint32_t crc       = readField<uint32_t>(in);
//     uint64_t timestamp = readField<uint64_t>(in);
//     uint32_t keySize   = readField<uint32_t>(in);
//     uint32_t valSize   = readField<uint32_t>(in);
//     std::string key    = readString(in, keySize);
//     std::string val    = readString(in, valSize);

//     Record rec(key, val);

//     if (rec.verifyChecksum(crc))
//         throw std::runtime_error("Read CRC and expected CRC not equal.");
        
//     return rec;
// }

bool Record::verifyChecksum(uint32_t checksum) {
    serialize();
    return checksum == getChecksum();
}

uint32_t Record::getComputedChecksum(const char* buf, const size_t size) const {
    boost::crc_32_type result;
    result.process_bytes(buf, size);
    return static_cast<uint32_t>(result.checksum());   
}

uint32_t Record::getChecksum() const {
    return header.getChecksum();
}

uint32_t Record::getTimestamp() const {
    return header.getTimestamp();
}

size_t Record::getRecordSize() const {
    return header.getRecordSize();
}

uint32_t Record::getKeySize() const {
    return header.getKeySize();
}

uint32_t Record::getValueSize() const {
    return header.getValueSize();
}

std::string Record::getKey() const {
    return key;
}

std::string Record::getValue() const {
    return value;
}