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
    friend std::ostream &operator<<(std::ostream &os, const Series &df);

    // Subscripts
    template <typename T> T &iloc(long long i);
    template <typename T> const T &iloc(long long i) const;
    std::string iloc_str(long long i) const;
    std::string iloc_str_(long long i) const;

    // Conversion
    template <typename T> void astype();

    // arithmetic
    Series &operator+=(Series const &obj);
    Series &operator-=(Series const &obj);
    Series &operator*=(Series const &obj);
    Series &operator/=(Series const &obj);

    template <typename T1, typename T2> void add_assignment(Series const &obj);
    template <typename T1, typename T2> void sub_assignment(Series const &obj);
    template <typename T1, typename T2> void mul_assignment(Series const &obj);
    template <typename T1, typename T2> void div_assignment(Series const &obj);

    // Modification
    template <typename T> void reserve(size_t sz);

    // Assignment
    Series &operator=(const Series &A);

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

    size_t _cal_index(long long i) const;
};

}; // namespace dfc
