#pragma once

#include <string>
#include <cstdint>
#include <ostream>

class RecordHeader {

    RecordHeader(uint32_t cs, uint32_t ts, uint8_t ks, uint32_t vs);

    uint32_t checksum; 
    uint32_t timestamp;
    uint8_t keySize; 
    uint32_t valSize;

    static constexpr size_t CHECKSUM_SIZE  = sizeof(checksum);
    static constexpr size_t TIMESTAMP_SIZE = sizeof(timestamp);
    static constexpr size_t KEYSIZE_SIZE   = sizeof(keySize);
    static constexpr size_t VALUESIZE_SIZE = sizeof(valSize);

    static constexpr size_t HEADER_SIZE = CHECKSUM_SIZE + TIMESTAMP_SIZE +
                                          KEYSIZE_SIZE  + VALUESIZE_SIZE;

    static constexpr size_t CHECKSUM_OFFSET = 0;
    static constexpr size_t TIMESTAMP_OFFSET = CHECKSUM_OFFSET + TIMESTAMP_SIZE;
    static constexpr size_t KEYSIZE_OFFSET = TIMESTAMP_OFFSET + KEYSIZE_SIZE;
    static constexpr size_t VALUESIZE_OFFSET = KEYSIZE_OFFSET + VALUESIZE_SIZE;
};

class Record {
    private:

        uint32_t crc32;
        uint64_t timestamp;
        uint32_t keySize;
        uint32_t valSize;

        std::string key;
        std::string val;  

    public:
        Record(std::string key, std::string value);
        ~Record() = default;
        
        void serialize(std::ostream& out) const;

        // static Record deserialize(std::istream& in);

        size_t byteSize() const;

        [[nodiscard]] uint32_t getCRC32() const;
        [[nodiscard]] uint32_t getExpectedCRC32(uint64_t timestamp);
        void setCRC32();

        [[nodiscard]] uint64_t getTimestamp() const;
        void setTimestamp(uint64_t ts);
        void setTimestampNow();

        [[nodiscard]] uint32_t getKeySize() const;
        [[nodiscard]] uint32_t getValueSize() const;
        [[nodiscard]] std::string getKey() const;
        [[nodiscard]] std::string getValue() const;
};