#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <variant>
#include <sstream>

class TupleEntry{
public:

    using TupleValue = std::variant<int, float, std::string>;
    TupleEntry(TupleValue value) : value(value){

    }
    TupleValue value;
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::ostream& operator<<(std::ostream& stream, const TupleEntry& tupleEntry);

class Tuple {
public:
    using EntriesVector = std::vector<TupleEntry>;

    template<typename T>
    void push([[maybe_unused]] T value){
        throw std::runtime_error("Wrong entry type!");
    }

    const EntriesVector& getEntries(){ return entries; }

    auto begin() { return entries.begin(); }
    auto end() { return entries.end(); }

    std::string to_string() const;
private:
    EntriesVector entries;
};

template<>
void Tuple::push<int>(int value);

template<>
void Tuple::push<float>(float value);

template<>
void Tuple::push<std::string>(std::string value);

std::ostream& operator<<(std::ostream& stream, const Tuple& tuple);