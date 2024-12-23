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

class SeriesView;
template <typename T> class SeriesIterator;

// A Series doesn't have index and can only be subscripted by number.
class Series {
  public:
    // Constructor
    // mono-state
    explicit Series(size_t n);
    template <typename T> explicit Series(const std::vector<T> &data);
    template <typename T> explicit Series(std::vector<T> &&data);
    template <typename T> Series(const std::string &name, std::vector<T> data);
    template <typename T> Series(const std::string &name, std::vector<T> &&data);

    // Blank column
    // template <typename T>
    // Series(const std::string &name , size_t size); // Blank series with specific size.
    Series(const std::string &name, DType type, size_t sz = 0);
    template <typename T> Series(std::initializer_list<T> l);
    template <typename T> Series(const std::string &name, std::initializer_list<T> l);

    Series(const Series &new_series);
    Series(Series &&new_series);

    explicit Series(const SeriesView &view);

    // Assignment
    Series &operator=(const Series &A);

    Series() {} // Default constructor, do nothing.

    // info
    const std::string &name() const;
    void rename(const std::string &name);
    size_t size() const;
    DType dtype() const;
    std::string dtype_name() const;
    bool empty() const;
    friend std::ostream &operator<<(std::ostream &os, const Series &df);

    // Subscripts
    template <typename T> T &iloc(long long i);
    template <typename T> const T &iloc(long long i) const;

    template <typename T> T &iloc_(size_t i);
    template <typename T> const T &iloc_(size_t i) const;

    std::string iloc_str(long long i) const;
    std::string iloc_str_(long long i) const;

    SeriesView iloc(const std::vector<long long> &pos);

    // Iterator
    // template <typename T> SeriesIterator<T> begin() const;
    // template <typename T> SeriesIterator<T> end() const;

    // Conversion
    template <typename T> void astype();
    bool is_numeric_dtype() const { return is_arithmetic(dtype()); }
    bool is_string_dtype() const { return dtype() == DType::STRING; }

    // Modification
    template <typename T> void reserve(size_t sz);
    void resize(size_t sz);

    // Insert a blank elements.
    void push_back();
    template <typename T> void push_back_();
    template <typename T> void push_back(const T &v);
    //
    const SeriesType &values() const;
    template <typename T> std::vector<T> asvector() const;
    template <typename T> const std::vector<T> &vector() const;
    template <typename T> std::vector<T> &_vector();

    // arithmetic
    Series &operator+=(const Series &obj);
    Series &operator-=(const Series &obj);
    Series &operator*=(const Series &obj);
    Series &operator/=(const Series &obj);

    Series &operator+=(const SeriesView &obj);
    Series &operator-=(const SeriesView &obj);
    Series &operator*=(const SeriesView &obj);
    Series &operator/=(const SeriesView &obj);

    template <typename T1, typename T2> void add_assignment(const Series &obj);
    template <typename T1, typename T2> void sub_assignment(const Series &obj);
    template <typename T1, typename T2> void mul_assignment(const Series &obj);
    template <typename T1, typename T2> void div_assignment(const Series &obj);

    template <typename T1, typename T2> void add_assignment(const SeriesView &obj);
    template <typename T1, typename T2> void sub_assignment(const SeriesView &obj);
    template <typename T1, typename T2> void mul_assignment(const SeriesView &obj);
    template <typename T1, typename T2> void div_assignment(const SeriesView &obj);

    template <typename T1, typename T2, typename Ret> Series add(const SeriesView &obj);
    template <typename T1, typename T2, typename Ret> Series sub(const SeriesView &obj);
    template <typename T1, typename T2, typename Ret> Series mul(const SeriesView &obj);
    template <typename T1, typename T2, typename Ret> Series div(const SeriesView &obj);

    template <typename T1, typename T2, typename Ret> Series add(const Series &obj);
    template <typename T1, typename T2, typename Ret> Series sub(const Series &obj);
    template <typename T1, typename T2, typename Ret> Series mul(const Series &obj);
    template <typename T1, typename T2, typename Ret> Series div(const Series &obj);

    Series operator+(const SeriesView &obj);
    Series operator-(const SeriesView &obj);
    Series operator*(const SeriesView &obj);
    Series operator/(const SeriesView &obj);

    Series operator+(const Series &obj);
    Series operator-(const Series &obj);
    Series operator*(const Series &obj);
    Series operator/(const Series &obj);

    friend Series operator>(const Series &lhs, const Series &rhs);
    friend Series operator<(const Series &lhs, const Series &rhs);
    friend Series operator>=(const Series &lhs, const Series &rhs);
    friend Series operator<=(const Series &lhs, const Series &rhs);
    friend Series operator==(const Series &lhs, const Series &rhs);
    friend Series operator!=(const Series &lhs, const Series &rhs);
    friend Series operator&&(const Series &lhs, const Series &rhs);
    friend Series operator||(const Series &lhs, const Series &rhs);

    friend Series operator>(const Series &lhs, const SeriesView &rhs);
    friend Series operator<(const Series &lhs, const SeriesView &rhs);
    friend Series operator>=(const Series &lhs, const SeriesView &rhs);
    friend Series operator<=(const Series &lhs, const SeriesView &rhs);
    friend Series operator==(const Series &lhs, const SeriesView &rhs);
    friend Series operator!=(const Series &lhs, const SeriesView &rhs);
    friend Series operator&&(const Series &lhs, const SeriesView &rhs);
    friend Series operator||(const Series &lhs, const SeriesView &rhs);

    friend Series operator>(const SeriesView &lhs, const Series &rhs);
    friend Series operator<(const SeriesView &lhs, const Series &rhs);
    friend Series operator>=(const SeriesView &lhs, const Series &rhs);
    friend Series operator<=(const SeriesView &lhs, const Series &rhs);
    friend Series operator==(const SeriesView &lhs, const Series &rhs);
    friend Series operator!=(const SeriesView &lhs, const Series &rhs);
    friend Series operator&&(const SeriesView &lhs, const Series &rhs);
    friend Series operator||(const SeriesView &lhs, const Series &rhs);

    template <supported_type S> friend Series operator>(const Series &lhs, const S &rhs);
    template <supported_type S> friend Series operator<(const Series &lhs, const S &rhs);
    template <supported_type S> friend Series operator>=(const Series &lhs, const S &rhs);
    template <supported_type S> friend Series operator<=(const Series &lhs, const S &rhs);
    template <supported_type S> friend Series operator==(const Series &lhs, const S &rhs);
    template <supported_type S> friend Series operator!=(const Series &lhs, const S &rhs);
    template <supported_type S> friend Series operator&&(const Series &lhs, const S &rhs);
    template <supported_type S> friend Series operator||(const Series &lhs, const S &rhs);

    template <supported_type S> friend Series operator>(const S &rhs, const Series &lhs);
    template <supported_type S> friend Series operator<(const S &rhs, const Series &lhs);
    template <supported_type S> friend Series operator>=(const S &rhs, const Series &lhs);
    template <supported_type S> friend Series operator<=(const S &rhs, const Series &lhs);
    template <supported_type S> friend Series operator==(const S &rhs, const Series &lhs);
    template <supported_type S> friend Series operator!=(const S &rhs, const Series &lhs);
    template <supported_type S> friend Series operator&&(const S &rhs, const Series &lhs);
    template <supported_type S> friend Series operator||(const S &rhs, const Series &lhs);

    template <bool inplace = false, supported_functor_type Func = std::function<double(double)>>
    auto apply(Func f);
    template <bool inplace = false, template <typename> typename TemplateFunc,
              supported_type_pack... TargetArgumentTypes>
    auto apply();

    SeriesType _values;

    friend class SeriesView;

  private:
    std::string _name;
    size_t _size;

    size_t _cal_index(long long i) const;
    std::vector<size_t> _cal_index(const std::vector<long long> &i) const;
};

// template <typename T> struct SeriesIterator {
//     SeriesIterator(Series *data) : data(data) {}
//     Series *data_ptr;
//     T *ptr;
//
//     SeriesIterator<T> &operator++();
//     SeriesIterator<T> &operator--();
// };
}; // namespace dfc
