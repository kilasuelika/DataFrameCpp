#pragma once

// @kilasuelika
// May 19, 2022

#include <algorithm>
#include <array>
#include <execution>
#include <initializer_list>
#include <iostream>
#include <numeric>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <variant>
#include <vector>

#include "typedef.hpp"
#include <boost/lexical_cast.hpp>
namespace dfc {

// A Series doesn't have index and can only be subscripted by number.
class Series {
  public:
    // Constructor
    template <typename T> Series(std::vector<T> data);
    template <typename T> Series(std::vector<T> &&data);
    template <typename T> Series(const std::string &name, std::vector<T> data);
    template <typename T> Series(const std::string &name, std::vector<T> &&data);
    // Blank column
    Series(const std::string &name, DType type);
    template <typename T> Series(std::initializer_list<T> l);
    template <typename T> Series(const std::string &name, std::initializer_list<T> l);

    Series(const Series &new_series);
    Series(Series &&new_series);

    Series(){}; // Default constructor, do nothing.

    // info
    const std::string &name() const;
    void rename(const std::string &name);

    size_t size() const;
    DType dtype() const;
    std::string dtype_name() const;
    bool empty() const;

    // Subscripts
    template <typename T> T &iloc(long long i);
    template <typename T> const T &iloc(long long i) const;

    // Conversion
    template <typename T> void astype();

    // Modification
    template <typename T> void reserve(size_t sz);
    // Insert a blank elements.
    void push_back();
    template <typename T> void push_back_();
    template <typename T> void push_back(const T &v);
    //
    const SeriesType &values() const;
    template <typename T> const std::vector<T> &vector() const;

    SeriesType _values;

  private:
    std::string _name;
    size_t _size;
};

template <typename T> Series::Series(std::vector<T> data) {
    _size = data.size();
    _values = std::move(data);
}

template <typename T> Series::Series(std::vector<T> &&data) {
    _size = data.size();
    _values = data;
}
template <typename T> Series::Series(const std::string &name, std::vector<T> data) {
    _size = data.size();
    _name = name;
    _values = std::move(data);
}
template <typename T> Series::Series(const std::string &name, std::vector<T> &&data) {
    _size = data.size();
    _name = name;
    _values = data;
}
Series::Series(const std::string &name, DType type) {
    _name = name;
    switch (type) {
    case DType::STRING:
        _values = std::vector<std::string>();
        break;
    case DType::BOOL:
        _values = std::vector<bool>();
        break;
    case DType::INT:
        _values = std::vector<int>();
        break;
    case DType::LONGLONG:
        _values = std::vector<long long>();
        break;
    case DType::FLOAT:
        _values = std::vector<float>();
        break;
    case DType::DOUBLE:
        _values = std::vector<double>();
        break;
    default:
        std::cout << std::format("Invalid data type {}.", static_cast<int>(type)) << std::endl;
        break;
    }
}

template <typename T> Series::Series(std::initializer_list<T> l) {
    std::vector<T> data(l);
    _size = data.size();
    _values = std::move(data);
}
template <typename T> Series::Series(const std::string &name, std::initializer_list<T> l) {
    _name = name;
    std::vector<T> data(l);
    _size = data.size();
    _values = std::move(data);
}
Series::Series(const Series &new_series)
    : _name(new_series._name), _size(new_series._size), _values(new_series._values) {}
Series::Series(Series &&new_series)
    : _name(std::move(new_series._name)), _size(std::move(new_series._size)),
      _values(std::move(new_series._values)) {}

const std::string &Series::name() const { return _name; };
void Series::rename(const std::string &name) { _name = name; }
DType Series::dtype() const { return static_cast<DType>(_values.index()); };
std::string Series::dtype_name() const { return DTypeName[_values.index()]; };
size_t Series::size() const { return _size; };
bool Series::empty() const { return _values.index() == 0; }

template <typename T> T &Series::iloc(long long i) {
    return const_cast<T &>(const_cast<const Series *>(this)->iloc<T>(i));
}
template <typename T> const T &Series::iloc(long long i) const {
    if (i >= 0) {
        return std::get<std::vector<T>>(_values)[i];
    } else {
        return std::get<std::vector<T>>(_values)[_size + i];
    }
}

template <typename T> void Series::astype() {
    if constexpr (std::is_same_v<T, std::string>) {
        // Current is numeric type, convert to string.
        if (_values.index() > 1) {
            std::vector<std::string> res(_size);
            std::visit(
                [&res, this](auto &&v) {
                    using VecT = std::decay_t<decltype(v)>;
                    if constexpr (not std::is_same_v<VecT, std::monostate>) {
                        using EleT = typename VecT::value_type;
                        if constexpr (not std::is_same_v<EleT, std::string>) {
                            std::transform(
                                std::execution::par, v.begin(), v.end(), res.begin(),
                                [](const EleT &e) -> std::string { return std::to_string(e); });
                        }
                    }
                },
                _values);
            _values = res;
        }
    } else {
        if (_values.index() > 1) {
            // Current is numeric, convert to another numeric type.
            std::vector<T> res(_size);
            std::visit(
                [&res, this](auto &&v) {
                    using VecT = std::decay_t<decltype(v)>;
                    if constexpr (not std::is_same_v<VecT, std::monostate>) {
                        using EleT = typename VecT::value_type;
                        if constexpr (not std::is_same_v<EleT, std::string>) {
                            std::transform(std::execution::par, v.begin(), v.end(), res.begin(),
                                           [](const EleT &e) -> T { return static_cast<T>(e); });
                        }
                    };
                },
                _values);
            _values = res;
        } else if (_values.index() == 1) {
            // Current is string, convert to numeric.
            std::vector<T> res(_size);
            auto &v = std::get<std::vector<std::string>>(_values);
            std::transform(std::execution::par, v.begin(), v.end(), res.begin(),
                           [](const std::string &e) -> T {
                               try {
                                   auto v = boost::lexical_cast<T>(e);
                                   return v;
                               } catch (...) {
                                   return std::numeric_limits<double>::quiet_NaN();
                               }
                           });
            _values = res;
        }
    }
}

template <typename T> void Series::reserve(size_t sz) {
    if (_values.index() == 0) {
        // Current no data.
        std::vector<T> res;
        res.reserve(sz);
        _values = std::move(res);
    } else {
        if (DTypeMap[std::type_index(typeid(T))] == _values.index()) {
            // T and current data type are the same
            std::get<T>(_values).reserve(sz);
        } else {
            // Not equal, do nothing.
            std::cerr << "[Series]: You try to reserve " << typeid(T).name()
                      << " elments. But the current data container has " << dtype_name()
                      << " elements. Nothing is done." << std::endl;
        }
    }
}

void Series::push_back() {
    if (_values.index() != 0) {
        std::visit(
            [this](auto &&v) {
                using VecT = std::decay_t<decltype(v)>;
                if constexpr (not std::is_same_v<VecT, std::monostate>) {
                    v.emplace_back();
                };
            },
            _values);
    }
    ++_size;
}

template <typename T> void Series::push_back_() {
    std::get<std::vector<T>>(_values).emplace_back();
}

template <typename T> void Series::push_back(const T &e) {
    std::get<std::vector<T>>(_values).emplace_back(e);
    _size++;
}

const SeriesType &Series::values() const { return _values; };
template <typename T> const std::vector<T> &Series::vector() const {
    return std::get<std::vector<T>>(_values);
};
}; // namespace dfc
