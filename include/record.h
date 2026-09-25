#pragma once

#include <string>
#include <cstdint>
#include <ostream>

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

        size_t byteSize();

        void setCRC32();
        [[nodiscard]] uint32_t getCRC32();

        void setTimestamp(uint64_t ts);
        void setTimestampNow();

        [[nodiscard]] uint64_t getTimestamp() const;
        [[nodiscard]] uint32_t getKeySize() const;
        [[nodiscard]] uint32_t getValueSize() const;
        [[nodiscard]] std::string getKey() const;
        [[nodiscard]] std::string getValue() const;
};