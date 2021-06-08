#ifndef LINDA_PATTERN_H
#define LINDA_PATTERN_H

#include <utility>
#include <variant>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include "Tuple.h"
#include "ISerializable.h"
#include "Constants.h"
#include "Exceptions.h"

namespace Linda{
    enum class PatternEntryType{
        Equal,
        Less,
        LessOrEqual,
        Greater,
        GreaterOrEqual,
        Any
    };

    class PatternEntry{
    public:
        using PatternValue = std::pair<PatternEntryType, TupleEntry::TupleValue>;

        PatternEntry(PatternEntryType type, const TupleEntry::TupleValue& value) : pattern(std::make_pair(type, value)){}
        explicit PatternEntry(PatternValue patternValue) : pattern(std::move(patternValue)){}
        ~PatternEntry() = default;
        PatternEntry(const PatternEntry&);

        [[nodiscard]] const PatternValue& getPattern() const;

        [[nodiscard]] const Linda::TupleEntry::TupleValue& getValue() const {
            if(pattern.first == PatternEntryType::Any){
                throw Linda::Exception::Pattern::AnyException("PatternEntryType::Any doesn't have any value!");
            }
            return pattern.second;
        }

        [[nodiscard]] TupleEntryType getTupleType() const;

        [[nodiscard]] PatternEntryType getType() const;

        [[nodiscard]] std::string to_string() const;

    private:
        PatternValue pattern;
    };

    class Pattern : public ISerializable {
    public:
        using PatternsVector = std::vector<PatternEntry>;

        Pattern() = default;
        ~Pattern() = default;
        Pattern(const Pattern&);
        explicit Pattern(const std::vector<ISerializable::serialization_type>&);

        template<PatternEntryType TYPE>
        void add(TupleEntry::int_type);

        template<PatternEntryType TYPE>
        void add(TupleEntry::float_type);

        template<PatternEntryType TYPE>
        void add(const TupleEntry::string_type &);

        template<PatternEntryType TYPE>
        void add(TupleEntryType);

        void add(PatternEntryType type, TupleEntry::int_type i);
        void add(PatternEntryType type, TupleEntry::float_type f);
        void add(PatternEntryType type, TupleEntry::string_type s);
        void add(PatternEntryType pType, Linda::TupleEntryType tType);

        bool check(const Tuple&) const;

        const PatternsVector& getPatterns() const { return entries; }
        std::string path() const { return treePath.str(); }
        std::string to_string() const;
        std::vector<std::string> all_paths() const;

        void clear(){ entries.clear(); treePath.clear(); }

        PatternEntry& operator[](std::size_t index){ return entries[index]; }
        const PatternEntry& operator[](std::size_t index) const { return entries[index]; }
        size_t size() const { return entries.size(); }
        auto begin() { return entries.begin(); }
        auto end() { return entries.end(); }

        std::vector<serialization_type> serialize() override;
        void deserialize(const std::vector<serialization_type> &vector) override;

    private:
        enum SerializationCodes{
            START = 0x80,
            END,
            INT,
            FLOAT,
            STRING,
            EQUAL,
            LESS,
            LESS_OR_EQUAL,
            GREATER,
            GREATER_OR_EQUAL,
            ANY
        };
        static SerializationCodes typeToSerializationCode(PatternEntryType);
        static PatternEntryType serializationCodeToType(SerializationCodes);

        void reset();

        PatternsVector entries;
        std::stringstream treePath;
    };
}

template<Linda::PatternEntryType TYPE>
void Linda::Pattern::add(TupleEntry::int_type i) {
    add(TYPE, i);
}

template<Linda::PatternEntryType TYPE>
void Linda::Pattern::add(TupleEntry::float_type f) {
    add(TYPE, f);
}

template<Linda::PatternEntryType TYPE>
void Linda::Pattern::add(const TupleEntry::string_type& str) {
    add(TYPE, str);
}

std::ostream& operator<<(std::ostream&, const Linda::PatternEntryType&);
std::ostream& operator<<(std::ostream&, const Linda::PatternEntry&);
std::ostream& operator<<(std::ostream&, const Linda::Pattern&);

#endif //LINDA_PATTERN_H