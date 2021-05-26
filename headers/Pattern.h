#ifndef LINDA_PATTERN_H
#define LINDA_PATTERN_H

#include <utility>
#include <variant>
#include <string>
#include <iostream>
#include <sstream>

#include "Tuple.h"
#include "ISerializable.h"

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
        using PatternValue = std::pair<PatternEntryType, Linda::TupleEntry::TupleValue>;

        PatternEntry(PatternEntryType type, const std::variant<int, float, std::string>& value) : pattern(std::make_pair(type, value)){}
        explicit PatternEntry(PatternValue patternValue) : pattern(std::move(patternValue)){}
        ~PatternEntry() = default;
        PatternEntry(const PatternEntry&);

        [[nodiscard]] const PatternValue& getPattern() const;

        [[nodiscard]] const Linda::TupleEntry::TupleValue& getValue() const {
            return pattern.second;
        }

        [[nodiscard]] TupleEntryType getTupleType() const;

        [[nodiscard]] PatternEntryType getType() const;

        [[nodiscard]] std::string to_string() const;

    private:
        PatternValue pattern;
    };

    class Pattern {
    public:
        using PatternsVector = std::vector<PatternEntry>;

        Pattern() = default;
        ~Pattern() = default;
        Pattern(const Pattern&);

        template<PatternEntryType TYPE>
        void add(int);

        template<PatternEntryType TYPE>
        void add(float);

        template<PatternEntryType TYPE>
        void add(const std::string&);

        template<PatternEntryType TYPE>
        void add(TupleEntryType);

        bool check(const Tuple&) const;

        const PatternsVector& getPatterns() const { return entries; }
        std::string path() const { return treePath.str(); }
        std::string to_string() const;

        void clear(){ entries.clear(); treePath.clear(); }

        PatternEntry& operator[](std::size_t index){ return entries[index]; }
        const PatternEntry& operator[](std::size_t index) const { return entries[index]; }
        size_t size() const { return entries.size(); }
        auto begin() { return entries.begin(); }
        auto end() { return entries.end(); }

    private:
        PatternsVector entries;
        std::stringstream treePath;
    };
}

template<Linda::PatternEntryType TYPE>
void Linda::Pattern::add(int i) {
    if (TYPE == PatternEntryType::Any) {
        throw std::runtime_error("Can't use Any with specific value.");
    }else{
        entries.emplace_back(TYPE, i);
        treePath << "i";
    }
}

template<Linda::PatternEntryType TYPE>
void Linda::Pattern::add(float f) {
    switch (TYPE) {
        case PatternEntryType::Any:
            throw std::runtime_error("Can't use Any with specific value.");
        case PatternEntryType::Equal:
            throw std::runtime_error("Can't use Equal on float value..");
        default:
            entries.emplace_back(TYPE, f);
            treePath << "f";
            break;
    }
}

template<Linda::PatternEntryType TYPE>
void Linda::Pattern::add(const std::string& str) {
    if (TYPE == PatternEntryType::Any) {
            throw std::runtime_error("Can't use Any with specific value.");
    }else{
        entries.emplace_back(TYPE, str);
        treePath << "s";
    }
}

std::ostream& operator<<(std::ostream&, const Linda::PatternEntryType&);
std::ostream& operator<<(std::ostream&, const Linda::PatternEntry&);
std::ostream& operator<<(std::ostream&, const Linda::Pattern&);

#endif //LINDA_PATTERN_H