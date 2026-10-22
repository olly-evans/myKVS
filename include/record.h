#pragma once

#include "versions.h"
#include "header.h"

class Record {
    private:

        // Precomputed metadata.

        RecordHeader header;

        std::string key;
        std::string val;  

    public:
        Record(std::string key, std::string value);
        ~Record() = default;
        
        void serialize(std::ostream& out) const;
        static Record deserialize(std::istream& in);

        [[nodiscard]] size_t getRecordSize() const;

        [[nodiscard]] uint32_t getKeySize() const;
        [[nodiscard]] uint32_t getValueSize() const;

        [[nodiscard]] std::string getKey() const;
        [[nodiscard]] std::string getValue() const;
};