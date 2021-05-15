#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <variant>

class TupleEntry{
public:
    TupleEntry(std::variant<int, float, std::string> value) : value(value){

    }
    std::variant<int, float, std::string> value;
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::ostream& operator<<(std::ostream& stream, const TupleEntry& tupleEntry);

class Tuple {
    typedef std::vector<TupleEntry> EntriesVector;
private:
    EntriesVector entries;
public:

    template<typename T>
    void push([[maybe_unused]] T value){
        throw std::runtime_error("Wrong entry type!");
    }

    const EntriesVector& getEntries(){ return entries; }
};