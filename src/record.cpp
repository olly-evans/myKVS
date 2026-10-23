#include "record.h"

/* Each object is responsible for its own serialization. */

Record::Record(std::string k, std::string v) :
    key(std::move(k)),
    value(std::move(v)) {
    
    uint32_t keySize = static_cast<uint32_t>(k.size());
    uint32_t valSize = static_cast<uint32_t>(v.size());

    header = Header(0, Versions::CURRENT_DATA_FILE_VERSION, header.getTimestampNow(), keySize, valSize);
}

std::vector<char>& Record::serialize() {

    // uint32_t crc = computeChecksum(); // header.computeChecksum()
    // header.setChecksum(crc);

    std::vector<char> buf;
    buf.reserve(getRecordSize());

    header.serialize(buf);

    std::memcpy(buf.data() + header.HEADER_SIZE, key.data(), getKeySize());
    std::memcpy(buf.data() + header.HEADER_SIZE + getKeySize(), value.data(), getValueSize());
    
    // out.write(key.data(), getKeySize());
    // out.write(val.data(), getValueSize());
    
    // out.flush();
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