#pragma once

// @kilasuelika
// May 19, 2022

#include "../Series.hpp"
#include "../SeriesView.hpp"

namespace dfc {

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
    return std::get<std::vector<T>>(_values)[_cal_index(i)];
}
std::string Series::iloc_str(long long i) const {
    auto n = _cal_index(i);
    return iloc_str_(n);
}
std::string Series::iloc_str_(long long i) const {
    // std::string res;
    std::ostringstream res;
    switch (dtype()) {
    case DType::STRING:
        res << iloc<std::string>(i);
        break;
    case DType::BOOL:
        res << iloc<bool>(i);
        break;
    case DType::INT:
        res << iloc<int>(i);
        break;
    case DType::LONGLONG:
        res << iloc<long long>(i);
        break;
    case DType::FLOAT:
        res << iloc<float>(i);
        break;
    case DType::DOUBLE:
        res << iloc<double>(i);
        break;
    default:
        break;
    }
    return res.str();
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

size_t Series::_cal_index(long long i) const {
    if (i >= 0) {
        return i;
    } else {
        return _size + i;
    }
}

std::ostream &operator<<(std::ostream &os, const Series &df) {
    os << df.name() << std::endl << df.dtype_name() << std::endl;
    for (auto i = 0; i < df._size; ++i) {
        os << df.iloc_str(i) << std::endl;
    }
    os << "Length: " << df.size() << std::endl;
    return os;
}

#define _SERIES_ARITH_ASSIGNMENT_IMPL_(OPNAME, transformop)                                        \
    template <typename T1, typename T2> void Series::OPNAME##_assignment(Series const &obj) {      \
        if (DTypeMap[std::type_index(typeid(T1))] != dtype()) {                                    \
            astype<T1>();                                                                          \
        }                                                                                          \
        auto &lhs = std::get<std::vector<T1>>(_values);                                            \
        auto &rhs = obj.vector<T2>();                                                              \
        std::transform(std::execution::par, lhs.cbegin(), lhs.cend(), rhs.cbegin(), lhs.begin(),   \
                       std::transformop<>{});                                                      \
    }

_SERIES_ARITH_ASSIGNMENT_IMPL_(add, plus);
_SERIES_ARITH_ASSIGNMENT_IMPL_(sub, minus);
_SERIES_ARITH_ASSIGNMENT_IMPL_(mul, multiplies);
_SERIES_ARITH_ASSIGNMENT_IMPL_(div, divides);

#define _SERIES_SELF_ARITHMETIC_BIN_OP_IMPL(op, MEMBER_NAME)                                       \
    Series &Series::operator op(Series const &obj) {                                               \
        if (_size != obj._size) {                                                                  \
            std::string info = std::format("[Series]: two series has different length: {}, {}.",   \
                                           _size, obj._size);                                      \
            std::cerr << info << std::endl;                                                        \
            return *this;                                                                          \
        } else {                                                                                   \
            if ((static_cast<int>(dtype()) <= 1) || (static_cast<int>(obj.dtype()) <= 1)) {        \
                std::string info = std::format(                                                    \
                    "[Series]: Arithmetic operations are not allowed on non-numeric series. ");    \
                std::cerr << info << std::endl;                                                    \
                return *this;                                                                      \
            } else {                                                                               \
                switch (dtype()) {                                                                 \
                case DType::BOOL:                                                                  \
                    switch (obj.dtype()) {                                                         \
                    case DType::BOOL:                                                              \
                        MEMBER_NAME##_assignment<bool, bool>(obj);                                 \
                        break;                                                                     \
                    case DType::INT:                                                               \
                        MEMBER_NAME##_assignment<int, int>(obj);                                   \
                        break;                                                                     \
                    case DType::LONGLONG:                                                          \
                        MEMBER_NAME##_assignment<long long, long long>(obj);                       \
                        break;                                                                     \
                    case DType::FLOAT:                                                             \
                        MEMBER_NAME##_assignment<float, float>(obj);                               \
                        break;                                                                     \
                    case DType::DOUBLE:                                                            \
                        MEMBER_NAME##_assignment<double, double>(obj);                             \
                        break;                                                                     \
                    default:                                                                       \
                        break;                                                                     \
                    }                                                                              \
                    break;                                                                         \
                case DType::INT:                                                                   \
                    switch (obj.dtype()) {                                                         \
                    case DType::BOOL:                                                              \
                        MEMBER_NAME##_assignment<int, bool>(obj);                                  \
                        break;                                                                     \
                    case DType::INT:                                                               \
                        MEMBER_NAME##_assignment<int, int>(obj);                                   \
                        break;                                                                     \
                    case DType::LONGLONG:                                                          \
                        MEMBER_NAME##_assignment<long long, long long>(obj);                       \
                        break;                                                                     \
                    case DType::FLOAT:                                                             \
                        MEMBER_NAME##_assignment<float, float>(obj);                               \
                        break;                                                                     \
                    case DType::DOUBLE:                                                            \
                        MEMBER_NAME##_assignment<double, double>(obj);                             \
                        break;                                                                     \
                    default:                                                                       \
                        break;                                                                     \
                    }                                                                              \
                    break;                                                                         \
                case DType::LONGLONG:                                                              \
                    switch (obj.dtype()) {                                                         \
                    case DType::BOOL:                                                              \
                        MEMBER_NAME##_assignment<long long, bool>(obj);                            \
                        break;                                                                     \
                    case DType::INT:                                                               \
                        MEMBER_NAME##_assignment<long long, int>(obj);                             \
                        break;                                                                     \
                    case DType::LONGLONG:                                                          \
                        MEMBER_NAME##_assignment<long long, long long>(obj);                       \
                        break;                                                                     \
                    case DType::FLOAT:                                                             \
                        MEMBER_NAME##_assignment<double, float>(obj);                              \
                        break;                                                                     \
                    case DType::DOUBLE:                                                            \
                        MEMBER_NAME##_assignment<double, double>(obj);                             \
                        break;                                                                     \
                    default:                                                                       \
                        break;                                                                     \
                    }                                                                              \
                    break;                                                                         \
                case DType::FLOAT:                                                                 \
                    switch (obj.dtype()) {                                                         \
                    case DType::BOOL:                                                              \
                        MEMBER_NAME##_assignment<float, bool>(obj);                                \
                        break;                                                                     \
                    case DType::INT:                                                               \
                        MEMBER_NAME##_assignment<float, int>(obj);                                 \
                        break;                                                                     \
                    case DType::LONGLONG:                                                          \
                        MEMBER_NAME##_assignment<double, long long>(obj);                          \
                        break;                                                                     \
                    case DType::FLOAT:                                                             \
                        MEMBER_NAME##_assignment<float, float>(obj);                               \
                        break;                                                                     \
                    case DType::DOUBLE:                                                            \
                        MEMBER_NAME##_assignment<double, double>(obj);                             \
                        break;                                                                     \
                    default:                                                                       \
                        break;                                                                     \
                    }                                                                              \
                    break;                                                                         \
                case DType::DOUBLE:                                                                \
                    switch (obj.dtype()) {                                                         \
                    case DType::BOOL:                                                              \
                        MEMBER_NAME##_assignment<double, bool>(obj);                               \
                        break;                                                                     \
                    case DType::INT:                                                               \
                        MEMBER_NAME##_assignment<double, int>(obj);                                \
                        break;                                                                     \
                    case DType::LONGLONG:                                                          \
                        MEMBER_NAME##_assignment<double, long long>(obj);                          \
                        break;                                                                     \
                    case DType::FLOAT:                                                             \
                        MEMBER_NAME##_assignment<double, float>(obj);                              \
                        break;                                                                     \
                    case DType::DOUBLE:                                                            \
                        MEMBER_NAME##_assignment<double, double>(obj);                             \
                        break;                                                                     \
                    default:                                                                       \
                        break;                                                                     \
                    }                                                                              \
                    break;                                                                         \
                default:                                                                           \
                    break;                                                                         \
                }                                                                                  \
                return *this;                                                                      \
            }                                                                                      \
        }                                                                                          \
    }

_SERIES_SELF_ARITHMETIC_BIN_OP_IMPL(+=, add);
_SERIES_SELF_ARITHMETIC_BIN_OP_IMPL(-=, sub);
_SERIES_SELF_ARITHMETIC_BIN_OP_IMPL(*=, mul);
_SERIES_SELF_ARITHMETIC_BIN_OP_IMPL(/=, div);

}; // namespace dfc