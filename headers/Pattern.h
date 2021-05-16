#pragma once

#include <utility>
#include <variant>
#include <string>
#include <iostream>

#include "Tuple.h"

enum class PatternEntryType{
    Equal,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual
};

std::ostream& operator<<(std::ostream&, PatternEntryType);

class PatternEntry{
public:
    using PatternValue = std::pair<PatternEntryType, std::variant<int, float, std::string>>;


private:
    PatternValue pattern;
};

class Pattern {
public:
    using PatternsVector = std::vector<PatternEntry>;

    Pattern() = default;
    ~Pattern() = default;
    Pattern(const Pattern& ) = default;

    template<PatternEntryType TYPE>
    void add(int value){
        std::cout<<TYPE<<" "<<value<<'\n';
    }

    template<PatternEntryType TYPE>
    void add(float value){
        std::cout<<TYPE<<" "<<value<<'\n';
    }

    template<PatternEntryType TYPE>
    void add(const std::string& value){
        std::cout<<TYPE<<" "<<value<<'\n';
    }

    bool check(Tuple) const;

private:
    PatternsVector patterns;
    std::string treePath;
};