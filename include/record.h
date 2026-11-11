#pragma once

#include <span>

#include "versions.h"
#include "header.h"

class Record {
    private:

        // Precomputed metadata.

        Header header;

        std::string key;
        std::string value;  

    public:
        Record(std::string key, std::string value);
        ~Record() = default;
        
        std::vector<char> serialize();
        static Record deserialize(std::vector<char> buf);

        [[nodiscard]] bool verifyChecksum(uint32_t checksum);

        [[nodiscard]] uint32_t getComputedChecksum(const char* buf, const size_t size) const;

        [[nodiscard]] size_t getRecordSize() const;

        [[nodiscard]] uint32_t getChecksum() const;

        [[nodiscard]] uint32_t getTimestamp() const;

        [[nodiscard]] uint32_t getKeySize() const;
        [[nodiscard]] uint32_t getValueSize() const;

        [[nodiscard]] std::string getKey() const;
        [[nodiscard]] std::string getValue() const;

};