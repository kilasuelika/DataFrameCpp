#pragma once

// @kilasuelika
// May 19, 2022

#include "../Series.hpp"
#include "../SeriesView.hpp"

namespace dfc {

dfc::Series::Series(size_t n) : _size(n) {}

template <typename T> Series::Series(const std::vector<T> &data) {
    _size = data.size();
    _values = data;
}

template <typename T> Series::Series(std::vector<T> &&data) {
    _size = data.size();
    _values = std::move(data);
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
// template <typename T>
// Series::Series(const std::string &name, size_t size)
//     : _name(name), _size(size), _values(std::vector<T>(size)) {}

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
    if constexpr (!std::is_same_v<T, const char *>) {
        std::vector<T> data(l);
        _size = data.size();
        _values = std::move(data);
    }else {
        std::vector<std::string> data(l.size());
        int i = 0;
        for (auto& ele: l) {
            data[i]=ele;
            ++i;
        }
        _size =i;
        _values = std::move(data);
    }
}
template <typename T> Series::Series(const std::string &name, std::initializer_list<T> l) {
    _name = name;
    if constexpr (!std::is_same_v<T, const char*>) {
          std::vector<T> data(l);
        _size = data.size();
        _values = std::move(data);
    } else {
        std::vector<std::string> data(l.size());
        int i = 0;
        for (auto &e : l) {
            data[i] = e;
            ++i;
        }
        _size = data.size();
        _values = std::move(data);
    }
    
}

inline Series::Series(const Series &new_series)
    : _name(new_series._name), _size(new_series._size), _values(new_series._values) {}

inline Series::Series(Series &&new_series)
    : _name(std::move(new_series._name)), _size(new_series._size),
      _values(std::move(new_series._values)) {}

dfc::Series::Series(const SeriesView &view) : _name(view.name()), _size(view.size()) {
    std::visit(
        [&view, this](auto &&v) -> void {
            using VecT = std::decay_t<decltype(v)>;
            if constexpr (not std::is_same_v<VecT, std::monostate>) {
                using EleT = typename VecT::value_type;
                VecT res(view.size());
                for (int i = 0; i < view.size(); ++i) {
                    res[i] = view.iloc_<EleT>(i);
                }
                this->_values = res;
            };
        },
        view._values->_values);
}

inline Series &Series::operator=(const Series &A) {
    _name = A._name;
    _size = A._size;
    _values = A._values;
    return *this;
}

inline const std::string &Series::name() const { return _name; };
inline void Series::rename(const std::string &name) { _name = name; }
inline DType Series::dtype() const { return static_cast<DType>(_values.index()); };
inline std::string Series::dtype_name() const { return DTypeName[_values.index()]; };
inline size_t Series::size() const { return _size; };
inline bool Series::empty() const { return _values.index() == 0; }

template <typename T> inline T &Series::iloc(long long i) {
    return const_cast<T &>(const_cast<const Series *>(this)->iloc<T>(i));
}
template <typename T> inline const T &Series::iloc(long long i) const {
    return std::get<std::vector<T>>(_values)[_cal_index(i)];
}

template <typename T> inline const T &Series::iloc_(size_t i) const {
    return std::get<std::vector<T>>(_values)[i];
}
template <typename T> inline T &Series::iloc_(size_t i) {
    return const_cast<T &>(const_cast<const Series *>(this)->iloc_<T>(i));
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

dfc::SeriesView dfc::Series::iloc(const std::vector<long long> &pos) { return SeriesView(this); }
// Even if _values is in monostate, the result is correct as a new vector res is created.
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

void dfc::Series::resize(size_t sz) {
    if (sz == 0)
        return;
    switch (_values.index()) {
        case 0:
        std::cerr << "Column is not set to any type. Can't resize."<<std::endl;
        break;
case 1:
    std::get<std::vector<std::string>>(_values).resize(sz);
        break;
case 2:
    std::get<std::vector<bool>>(_values).resize(sz);
    break;
case 3:
    std::get<std::vector<int>>(_values).resize(sz);
    break;
case 4:
    std::get<std::vector<long long>>(_values).resize(sz);
    break;
case 5:
    std::get<std::vector<float>>(_values).resize(sz);
    break;
case 6:
    std::get<std::vector<double>>(_values).resize(sz);
    break;
default:
    break;
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
template <typename T> std::vector<T> dfc::Series::asvector() const {
    Series new_series = *this;
    new_series.astype<T>();
    return std::move(new_series.vector<T>());
}
template <typename T> const std::vector<T> &Series::vector() const {
    return std::get<std::vector<T>>(_values);
};

template <typename T> std::vector<T> &dfc::Series::_vector() {
    return std::get<std::vector<T>>(_values);
}

size_t Series::_cal_index(long long i) const {
    if (i >= 0) {
        return i;
    } else {
        return _size + i;
    }
}
std::vector<size_t> dfc::Series::_cal_index(const std::vector<long long> &i) const {
    std::vector<size_t> res(i.size());
    std::transform(i.begin(), i.end(), res.begin(), [this](long long j) { return _cal_index(j); });
    return res;
}

std::ostream &operator<<(std::ostream &os, const Series &df) {
    os << df.name() << std::endl << df.dtype_name() << std::endl;
    for (auto i = 0; i < df._size; ++i) {
        os << df.iloc_str(i) << std::endl;
    }
    os << "Length: " << df.size() << std::flush;
    return os;
}

// Series-Series bin op can utilize parallel execution.
#define DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(OPNAME, transformop)                     \
    template <typename T1, typename T2> void Series::OPNAME##_assignment(const Series &obj) {      \
        if (DTypeMap[std::type_index(typeid(T1))] != dtype()) {                                    \
            astype<T1>();                                                                          \
        }                                                                                          \
        auto &lhs = std::get<std::vector<T1>>(_values);                                            \
        auto &rhs = obj.vector<T2>();                                                              \
        std::transform(std::execution::par, lhs.cbegin(), lhs.cend(), rhs.cbegin(), lhs.begin(),   \
                       std::transformop<>{});                                                      \
    }

DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(add, plus);
DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(sub, minus);
DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(mul, multiplies);
DataFrameCpp_SERIES_SERIES_BINOP_ASSIGNMENT_IMPL_(div, divides);

#include "AssignmentMacro.hpp"
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, +=, add);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, -=, sub);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, *=, mul);
DataFrameCpp_SERIES_OR_VIEW_BINOP_ASSIGNMENT_IMPL_(Series, SeriesView, /=, div);

DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, +=, add_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, -=, sub_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, *=, mul_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, Series, /=, div_assignment);

DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, +=, add_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, -=, sub_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, *=, mul_assignment);
DataFrameCpp_SERIES_OR_VIEW_SELF_ARITHMETIC_BIN_OP_IMPL(Series, SeriesView, /=, div_assignment);

DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, add, +);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, sub, -);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, mul, *);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, SeriesView, div, /);

DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, add, +);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, sub, -);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, mul, *);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_(Series, Series, div, /);

DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, add, +);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, sub, -);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, mul, *);
DataFrameCpp_SERIES_OR_VIEW_ARITHMETIC_BIN_OP_(Series, SeriesView, div, /);

DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(+, add_assignment);
DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(-, sub_assignment);
DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(*, mul_assignment);
DataFrameCpp_SERIES_SERIES_ARITH_BIN_OP_IMPL_(/, div_assignment);

#include "UndefMacro.hpp"

// template <typename T> dfc::SeriesIterator<T> &dfc::SeriesIterator<T>::operator++() {
//     if ()
//     ++ptr;
//     return *this;
// }

}; // namespace dfc