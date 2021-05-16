#pragma once

#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <variant>
#include <sstream>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

enum class TupleEntryType : size_t{
    Unknown = std::variant_npos,
    Int = 0,
    Float,
    String
};

std::ostream& operator<<(std::ostream&, TupleEntryType);

class TupleEntry{
public:
    using TupleValue = std::variant<int, float, std::string>;

    explicit TupleEntry(TupleValue value = 0) : value(std::move(value)){}

    [[nodiscard]] const TupleValue& getValue() const {
        return value;
    }

    [[nodiscard]] TupleEntryType getType() const {
        switch (value.index()) {
            case 0:
                return TupleEntryType::Int;
            case 1:
                return TupleEntryType::Float;
            case 2:
                return TupleEntryType::String;
            case std::variant_npos:
            default:
                return TupleEntryType::Unknown;
        }
    }

private:
    TupleValue value;
};

std::ostream& operator<<(std::ostream&, const TupleEntry&);

class Tuple {
public:
    Tuple() = default;
    ~Tuple() = default;
    Tuple(const Tuple&);

    using EntriesVector = std::vector<TupleEntry>;

    void push(int);

    void push(float);

    void push(std::string);

    const EntriesVector& getEntries() const { return entries; }
    std::string path() const { return treePath.str(); }
    std::string to_string() const;

    void clear(){ entries.clear(); }

    auto begin() { return entries.begin(); }
    auto end() { return entries.end(); }

private:
    EntriesVector entries;
    std::stringstream treePath;
};

std::ostream& operator<<(std::ostream&, const Tuple&);