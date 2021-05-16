#pragma once

#include <utility>
#include <variant>
#include <string>

enum PatternEntryType{
    Equal,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual
};

class PatternEntry{
public:
    using PatternValue = std::pair<PatternEntryType, std::variant<int, float, std::string>>;


private:
    PatternValue pattern;
};

class Pattern {
public:
    Pattern() = default;
    ~Pattern() = default;
    Pattern(const Pattern& ) = default;

    void add();
};

