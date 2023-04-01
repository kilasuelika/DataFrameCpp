#pragma once

// @kilasuelika
// May 19, 2022

#include <array>
#include <span>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <format>
#include <typeinfo>
#include <exception>
#include <concepts>
#include <fmt/format.h>
#include <fmt/color.h>
#include "date.hpp"
#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "util.hpp"

#define UNWRAP(...) __VA_ARGS__

namespace dfc {
using boost::gregorian::date;
using boost::gregorian::date_duration;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;

// Data type.
enum DType {
    NONE = 0,
    STRING,
    BOOL,
    INT,
    LONGLONG,
    FLOAT,
    DOUBLE,
    DATE,
    DATETIME,
    DATEDURATION,
    TIMEDURATION
};

static std::array<std::string, 11> DTypeName{"none", "string", "bool", "int",
                                             "long long", "float", "double", "date",
                                             "datetime", "date_dur", "time_dur"};
static std::unordered_map<std::type_index, DType> DTypeMap{
    {std::type_index(typeid(std::monostate)), DType::NONE},
    {std::type_index(typeid(std::string)), DType::STRING},
    {std::type_index(typeid(bool)), DType::BOOL},
    {std::type_index(typeid(int)), DType::INT},
    {std::type_index(typeid(long long)), DType::LONGLONG},
    {std::type_index(typeid(float)), DType::FLOAT},
    {std::type_index(typeid(double)), DType::DOUBLE},
    {std::type_index(typeid(date)), DType::DATE},
    {std::type_index(typeid(ptime)), DType::DATETIME},
    {std::type_index(typeid(date_duration)), DType::DATEDURATION},
    {std::type_index(typeid(time_duration)), DType::TIMEDURATION}};

template <typename T> struct DType_traits {
    inline static DType dtype = DTypeMap[std::type_index(typeid(T))];
    inline static std::string name = DTypeName[dtype];
};


using DataFrameShape = std::array<size_t, 2>;

template <typename T>
concept iloc_type = requires(T x)
{
    requires std::same_as<T, int> || std::same_as<T, size_t> || std::same_as<T, long long>;
};

inline static bool is_arithmetic(int i) { return i > 1 && i < 7; };

template <typename T>
concept arithmetic_type = requires(T v)
{
    std::is_same_v<std::remove_cvref_t<T>, bool> || std::is_same_v<std::remove_cvref_t<T>, int> ||
    std::is_same_v<std::remove_cvref_t<T>, long long> ||
    std::is_same_v<std::remove_cvref_t<T>, float> ||
    std::is_same_v<std::remove_cvref_t<T>, double>;
};

template <typename T>
concept date_time_type = requires(T v)
{
    std::is_same_v<std::remove_cvref_t<T>, date> || std::is_same_v<std::remove_cvref_t<T>, ptime> ||
    std::is_same_v<std::remove_cvref_t<T>, date_duration> ||
    std::is_same_v<std::remove_cvref_t<T>, time_duration>;
};
template <typename T>
concept supported_type = requires(T v)
{
    std::is_same_v<std::remove_cvref_t<T>, std::string> ||
    std::is_same_v<std::remove_cvref_t<T>, bool> ||
    std::is_same_v<std::remove_cvref_t<T>, int> ||
    std::is_same_v<std::remove_cvref_t<T>, long long> ||
    std::is_same_v<std::remove_cvref_t<T>, float> ||
    std::is_same_v<std::remove_cvref_t<T>, double> ||
    std::is_same_v<std::remove_cvref_t<T>, date> ||
    std::is_same_v<std::remove_cvref_t<T>, ptime> ||
    std::is_same_v<std::remove_cvref_t<T>, date_duration> ||
    std::is_same_v<std::remove_cvref_t<T>, time_duration>;
};

// static std::unordered_map<std::type_index, std::unordered_map<std::type_index, DType>>{
// };
using SeriesType =
std::variant<std::monostate, std::vector<std::string>, std::vector<bool>, std::vector<int>,
             std::vector<long long>, std::vector<float>, std::vector<double>, std::vector<date>,
             std::vector<ptime>, std::vector<date_duration>, std::vector<time_duration>>;

// using SeriesViewType =
//     std::variant<std::monostate, std::span<std::string>, std::span<bool>, std::span<int>,
//                  std::span<long long>, std::span<float>, std::span<double>>;

// Index
// TRIVALINDEX: store a start integer start.
// INTINDEX: int as key.
// STRINGINDEX: string as key.
enum IndexDType { TRIVALINDEX = 0, INTINDEX, STRINGINDEX };

static std::array<std::string, 3> IndexDTypeName{"trival", "int", "string"};
// A key can be mapped to multiple rows.
using IndexType = std::variant<long long, std::unordered_map<int, std::vector<size_t>>,
                               std::unordered_map<std::string, std::vector<size_t>>>;

enum ViewIndexType { RANGE = 0, SLICE, POS };

struct Slice {
    long long start = 0, step = 1, end = -1;

    std::string str() const {
        return std::format("Slice( start={}, end={}, step={})", start, end, step);
    };
};

struct CSVIOOptions {
    std::string _sep = ",";
    // Use -1 to represent "no" column name and "no" row index.
    long long _header = 0;
    long long _index = 0;

    long long guess_rows = 1000;   // Use first 1000 rows to decide column type.
    double double_threshold = 0.2; // If fraction of successful double casts exceed this threshold,
    // then this column is double and failed castings result in NaN.

    struct CSVIOOptions &sep(const std::string &_sep) {
        this->_sep = _sep;
        return *this;
    }

    struct CSVIOOptions &header(const long long &_header) {
        this->_header = _header;
        return *this;
    }

    struct CSVIOOptions &index(const long long &_index) {
        this->_index = _index;
        return *this;
    }
};

}; // namespace dfc

namespace fmt {
template <typename... Args> void cerr(Args ... args) { print(fg(color::red), args...); }
} // namespace fmt
