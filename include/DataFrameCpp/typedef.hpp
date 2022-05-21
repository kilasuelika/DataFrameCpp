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
#include <typeinfo>
namespace dfc {
enum DType { NONE, STRING, BOOL, INT, LONGLONG, FLOAT, DOUBLE };
static std::array<std::string, 7> DTypeName{"none",      "string", "bool",  "int",
                                            "long long", "float",  "double"};
static std::unordered_map<std::type_index, DType> DTypeMap{
    {std::type_index(typeid(std::monostate)), DType::NONE},
    {std::type_index(typeid(std::string)), DType::STRING},
    {std::type_index(typeid(bool)), DType::BOOL},
    {std::type_index(typeid(int)), DType::INT},
    {std::type_index(typeid(long long)), DType::LONGLONG},
    {std::type_index(typeid(float)), DType::FLOAT},
    {std::type_index(typeid(double)), DType::DOUBLE}};
using DataFrameShape = std::array<size_t, 2>;

using SeriesType =
    std::variant<std::monostate, std::vector<std::string>, std::vector<bool>, std::vector<int>,
                 std::vector<long long>, std::vector<float>, std::vector<double>>;
using SeriesViewType =
    std::variant<std::monostate, std::span<std::string>, std::span<bool>, std::span<int>,
                 std::span<long long>, std::span<float>, std::span<double>>;

struct CSVIOOptions {
    std::string _sep = ",";
    long long _header = 0;
    long long _index = 0;
    long long guess_rows = 1000;   // Use first 1000 rows to decide column type.
    double double_threshold = 0.2; // If fraction of successful double casts exceed this threshold,
                                   // then this column is double..

    struct CSVIOOptions &sep(const std::string &_sep) {
        this->_sep = _sep;
        return *this;
    }
};
}; // namespace dfc
