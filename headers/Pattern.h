#pragma once

//FIXME
// - rewrite this cause its trash

#include <utility>
#include <variant>
#include <string>
#include <iostream>
#include <sstream>

#include "Tuple.h"

enum class PatternEntryType{
    Equal,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
    Any
};

std::ostream& operator<<(std::ostream&, PatternEntryType);

class PatternEntry{
public:
    using PatternValue = std::pair<PatternEntryType, std::variant<int, float, std::string>>;

    PatternEntry(PatternEntryType type, std::variant<int, float, std::string> value) : pattern(std::make_pair(type, value)){}
    explicit PatternEntry(PatternValue patternValue) : pattern(std::move(patternValue)){}
    ~PatternEntry() = default;

    [[nodiscard]] const PatternValue& getPattern() const {
        return pattern;
    }

    [[nodiscard]] std::string to_string() const;

private:
    PatternValue pattern;
};

std::ostream& operator<<(std::ostream&, const PatternEntry&);

class Pattern {
public:
    using PatternsVector = std::vector<PatternEntry>;

    Pattern() = default;
    ~Pattern() = default;
    Pattern(const Pattern& ) = default;

    template<PatternEntryType TYPE>
    void add(int);

    template<PatternEntryType TYPE>
    void add(float);

    template<PatternEntryType TYPE>
    void add(const std::string&);

    template<PatternEntryType TYPE>
    void add(TupleEntryType);

    bool check(Tuple) const;

    const PatternsVector& getPatterns() const { return patterns; }
    std::string path() const { return treePath.str(); }
    std::string to_string() const;

    void clear(){ patterns.clear(); }

    auto begin() { return patterns.begin(); }
    auto end() { return patterns.end(); }

    friend std::ostream &operator<<(std::ostream &os, const Pattern &pattern);

private:
    PatternsVector patterns;
    std::stringstream treePath;
};

template<PatternEntryType TYPE>
void Pattern::add(int i) {
    patterns.emplace_back(TYPE, i);
    treePath << "i";
}

template<PatternEntryType TYPE>
void Pattern::add(float f) {
    patterns.emplace_back(TYPE, f);
    treePath << "f";
}

template<PatternEntryType TYPE>
void Pattern::add(const std::string& str) {
    patterns.emplace_back(TYPE, str);
    treePath << "s";
}

// FIXME: this is bad fix this pls
template<PatternEntryType TYPE>
void Pattern::add(TupleEntryType type) {
    if(TYPE != PatternEntryType::Any){
        throw std::runtime_error("Only Any type can skip value!");
    }else{
        switch (type) {
            case TupleEntryType::Int:
                patterns.emplace_back(TYPE, 0);
                break;
            case TupleEntryType::Float:
                patterns.emplace_back(TYPE, 0.0f);
                break;
            case TupleEntryType::String:
                patterns.emplace_back(TYPE, "");
                break;
            case TupleEntryType::Unknown:
                // FIXME: WTF IS THAT KILL THIS
                throw std::runtime_error("Unknown type!");
        }
    }
}