#include "record.h"

/* Each object is responsible for its own serialization. */

Record::Record(std::string k, std::string v) {
    
    uint32_t keySize = static_cast<uint32_t>(k.size());
    uint32_t valSize = static_cast<uint32_t>(v.size());

    header = RecordHeader(0, header.getTimestampNow(), keySize, valSize);
    key = k.data();
    val = v.data();

}

void Record::serialize(std::ostream& out) const {

    // out.write(reinterpret_cast<const char*>(&crc32), sizeof(crc32));
    // out.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    // out.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
    // out.write(reinterpret_cast<const char*>(&valSize), sizeof(valSize));

    out.write(key.data(), getKeySize());
    out.write(val.data(), getValueSize());
    
    out.flush();
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
    return val;
}