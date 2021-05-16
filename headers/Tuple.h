#pragma once

#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <variant>
#include <sstream>

class TupleEntry{
public:
    using TupleValue = std::variant<int, float, std::string>;

    explicit TupleEntry(TupleValue value = 0) : value(std::move(value)){}

    [[nodiscard]] const TupleValue& getValue() const {
        return value;
    }

private:
    TupleValue value;
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::ostream& operator<<(std::ostream&, const TupleEntry&);

class Tuple {
public:
    Tuple() = default;
    ~Tuple() = default;
    Tuple(const Tuple&) = default;



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
    std::stringstream treePath;
};

template<>
void Tuple::push<int>(int);

template<>
void Tuple::push<float>(float);

template<>
void Tuple::push<std::string>(std::string);

std::ostream& operator<<(std::ostream&, const Tuple&);