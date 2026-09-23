#pragma once

#include <string>
#include <cstdint>

constexpr uint32_t CRC32_POLY = 0xEDB88320;

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
        ~Record();
        
        void computeSetCRC32();
        uint32_t getCRC32();

        void setTimestampNow();
        uint64_t getTimestamp() const;

        void setKeySize(uint32_t ks);
        uint32_t getKeySize() const;

        void setValueSize(uint32_t vs);
        uint32_t getValueSize() const;

        void setKey(std::string k);
        std::string getKey() const;

        void setValue(std::string v);
        std::string getValue() const;
};