#pragma once

#include <string>
#include <cstdint>

class Record {
    private:
        uint32_t crc;
        uint64_t timestamp;
        uint32_t keySize;
        uint32_t valSize;
        std::string key;
        std::string val;  

    public:
        Record(std::string key, std::string value);
        ~Record();
        
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