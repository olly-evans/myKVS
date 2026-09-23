#pragma once

#include <string>
#include <cstdint>

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
        // ~Record();
        
        void computeSetCRC32();
        [[nodiscard]] uint32_t getCRC32();

        void setTimestampNow();
        [[nodiscard]] uint64_t getTimestamp() const;

        void setKeySize(uint32_t ks);
        [[nodiscard]] uint32_t getKeySize() const;

        void setValueSize(uint32_t vs);
        [[nodiscard]] uint32_t getValueSize() const;

        void setKey(std::string k);
        [[nodiscard]] std::string getKey() const;

        void setValue(std::string v);
        [[nodiscard]] std::string getValue() const;
};