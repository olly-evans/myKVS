#pragma once

#include <string>
#include <cstdint>
#include <chrono>
#include <boost/crc.hpp>

class RecordHeader {

    public:
        RecordHeader() = default;
        ~RecordHeader() = default;

        RecordHeader(uint32_t cSum, uint8_t ver, uint32_t tStamp, uint32_t kSize, uint32_t vSize);
        
        std::vector<char> serialize();
        static RecordHeader deserialize(std::vector<char> buf);

        [[nodiscard]] size_t getRecordSize() const;

        [[nodiscard]] uint32_t getChecksum() const;
        [[nodiscard]] uint32_t getExpectedChecksum(uint32_t timestamp);
        void computeHeaderChecksum();

        [[nodiscard]] static uint32_t getTimestampNow();
        [[nodiscard]] uint32_t getTimestamp() const;
        void setTimestamp(uint32_t ts);

        [[nodiscard]] uint32_t getKeySize() const;
        [[nodiscard]] uint32_t getValueSize() const;

    private:

        size_t recordSize;
        
        uint32_t checksum; 
        uint8_t version;
        uint32_t timestamp;
        uint32_t keySize; 
        uint32_t valSize;

        static constexpr size_t CHECKSUM_SIZE  = sizeof(checksum);
        static constexpr size_t VERSION_SIZE   = sizeof(version);
        static constexpr size_t TIMESTAMP_SIZE = sizeof(timestamp);
        static constexpr size_t KEYSIZE_SIZE   = sizeof(keySize);
        static constexpr size_t VALUESIZE_SIZE = sizeof(valSize);

        static constexpr size_t HEADER_SIZE = CHECKSUM_SIZE + TIMESTAMP_SIZE +
                                            VERSION_SIZE  + KEYSIZE_SIZE  + 
                                            VALUESIZE_SIZE;

        static constexpr size_t CHECKSUM_OFFSET = 0;
        static constexpr size_t VERSION_OFFSET = CHECKSUM_OFFSET + VERSION_SIZE;
        static constexpr size_t TIMESTAMP_OFFSET = VERSION_OFFSET + TIMESTAMP_SIZE;
        static constexpr size_t KEYSIZE_OFFSET = TIMESTAMP_OFFSET + KEYSIZE_SIZE;
        static constexpr size_t VALUESIZE_OFFSET = KEYSIZE_OFFSET + VALUESIZE_SIZE;

};
