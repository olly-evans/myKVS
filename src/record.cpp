#include "record.h"

#include <chrono>

Record::Record(std::string key, std::string value) {

    setTimestampNow();
    // set everything else for them.
}

void Record::setTimestampNow() {

    const auto p1 = std::chrono::system_clock::now();
    uint64_t ts = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    this->timestamp = ts;
}

uint64_t Record::getTimestamp() {
    return this->timestamp;
}

void Record::setKeySize(uint32_t ks) {

}

uint32_t Record::getKeySize() {

}

void Record::setValueSize(uint32_t vs) {

}

uint32_t Record::getValueSize() {

}

void Record::setKey(std::string k) {

}

std::string Record::getKey() {

}

void Record::setValue(std::string v) {

}

std::string Record::getValue() {

}
